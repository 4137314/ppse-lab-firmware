#include "gps.h"

// GLOBAL VARIABLES
struct parsed_nmea global_parsed_nmea = {0};


bool gpsInit() {
  Serial.println("=== GPS Init ===");

  // Led alive init
  pinMode(LED_ALIVE, OUTPUT);
  digitalWrite(LED_ALIVE, LOW); // LED alive off
  Serial.print("Led alive pin: ");
  Serial.print(LED_ALIVE);
  Serial.println(" inizializzato come output.");

  // Enable GPS module
  pinMode(GPS_EN_PIN, OUTPUT);
  digitalWrite(GPS_EN_PIN, LOW); // bring down GPS_EN signal to enable (pmos)
  Serial.print("GPS enable pin: ");
  Serial.print(GPS_EN_PIN);
  Serial.println(" alimentato.");

  // Enable input for Pulse signal from gps
  pinMode(GPS_PPS_PIN, INPUT);
  Serial.print("Gps PPS pin: ");
  Serial.print(GPS_PPS_PIN);
  Serial.println(" inizializzato come input.");

  // UART1 for the GPS. Serial2 maps to UART1 in Arduino framework!
  Serial2.setTX(UART1_TX_PIN);
  Serial2.setRX(UART1_RX_PIN);
  Serial2.begin(GPSBAUD); // start uart1
  Serial.println("UART1 started on Serial2, listening for GPS data...");

  // Wait at least 10 sec for a GPS string.
  int i;
  for(i=0; i < 1000 && (Serial2.available() == 0); ++i) delay(1);

  if (i == 1000) {
    Serial.println("ERROR: Canot start the GPS module");
    return false;
  }
  Serial.println("OK!");

  return true;
}


void EmptyGpsBuffer() {
  Serial2.flush(); // waits for transmission to finish
  while (Serial2.available()) {
    Serial2.read(); // Read and discard
  }
  Serial2.readStringUntil('\n');
}


bool nmea_gps_parse(String *nmea_message) {
  bool state = true;

  switch (minmea_sentence_id(nmea_message->c_str(), true)) {
    case MINMEA_SENTENCE_RMC: {
      struct minmea_sentence_rmc *frame = &global_parsed_nmea.parsed_rmc;
      if (minmea_parse_rmc(frame, nmea_message->c_str()))
        print_NMEA_rmc(frame);
      else 
        (state = false);
    } break;

    case MINMEA_SENTENCE_GGA: {
      struct minmea_sentence_gga *frame = &global_parsed_nmea.parsed_gga;
      if (minmea_parse_gga(frame, nmea_message->c_str()))
        print_NMEA_gga(frame);
      else 
        (state = false);
    } break;

    case MINMEA_SENTENCE_GST: {
      struct minmea_sentence_gst *frame = &global_parsed_nmea.parsed_gst;
      if (minmea_parse_gst(frame, nmea_message->c_str()))
        print_NMEA_gst(frame);
      else 
        (state = false);
    } break;

    case MINMEA_SENTENCE_GSV: {
      struct minmea_sentence_gsv *frame = &global_parsed_nmea.parsed_gsv;
      if (minmea_parse_gsv(frame, nmea_message->c_str()))
        print_NMEA_gsv(frame);
      else 
        (state = false);
    } break;

    case MINMEA_SENTENCE_VTG: {
      struct minmea_sentence_vtg *frame = &global_parsed_nmea.parsed_vtg;
      if (minmea_parse_vtg(frame, nmea_message->c_str()))
        print_NMEA_vtg(frame);
      else 
        (state = false);
    } break;

    case MINMEA_SENTENCE_ZDA: {
      struct minmea_sentence_zda *frame = &global_parsed_nmea.parsed_zda;
      if (minmea_parse_zda(frame, nmea_message->c_str()))
        print_NMEA_zda(frame);
      else 
        (state = false);
    } break;
    default: {
      (state = false);
    }
  }

  if (state) return true;
  #if DEBUG
  Serial.print("WARNING: Could not parse this nmea string ");
  Serial.println(nmea_message->c_str());
  #endif
  return false;
}



bool gpsAcquire(enum minmea_sentence_id sentence_type) {
  String nmea_message;

  digitalWrite(LED_ALIVE, HIGH);

    for (int i = 0; i < GPS_ACQUIRE_MAX_TRIES; ++i) {
      EmptyGpsBuffer();
      for(int c=0; Serial2.available() == 0; ++c) delay(1);

      nmea_message = Serial2.readStringUntil('\n');
      digitalWrite(LED_ALIVE, LOW);

      if (minmea_sentence_id(nmea_message.c_str(), true) == sentence_type) {
        if (nmea_gps_parse(&nmea_message)) return true;
      }
    }

  #if DEBUG
  Serial.println("WARNING in gpsAcquire: max tries reached before correctly "
                  "acquiring the requested gps message\n");
  #endif
  return false;
} 


bool GetDate_and_Time() {

  if (gpsAcquire(MINMEA_SENTENCE_RMC)) {
    return true;
  }
  #if DEBUG
  Serial.println("WARNING in GetDate_and_Time: failed to get data");
  #endif
  return false;
}



bool GetPosition_and_Satellites() {
  if (gpsAcquire(MINMEA_SENTENCE_GGA)) {
    return true;
  }
  #if DEBUG
  Serial.println("WARNING in GetPosition_and_Satellites: failed to get data");
  #endif
  return false;
}



bool GPS_sync() {
  // retain value across function calls
  static uint32_t lastGPSsync = 0;

  if (millis() - lastGPSsync > GPS_SYNC_TIMEOUT_MSEC) {
    if (GetDate_and_Time() && GetPosition_and_Satellites()) {
      lastGPSsync = millis();
      return true;
    }
  }
  return false;
}



/* Salva su Filesystem della FLASH */
bool save_gps_data() {
  struct minmea_sentence_rmc *rmc = &global_parsed_nmea.parsed_rmc;
  char buffer[128]="";

  FILE* f = fopen("GPS_LOG_PATH", "a");

  if (f == NULL)
  {
    Serial.printf("ERROR in save_gps_data: cannot open gps log file");
  }

  // Check for log size, if 20*buffer keep only the last logged data
  fseek(f, 0, SEEK_END);
  if(ftell(f) > (20*sizeof(buffer)))
  {
    fseek(f, 0, SEEK_SET); // reset to beginning
    while( fgets(buffer, sizeof(buffer), f)!=NULL ); //read all lines till the EOF
    fclose(f);
    fopen("GPS_LOG_PATH", "w"); // wipes data in file

    if (f == NULL)
    {
      Serial.printf("ERROR in save_gps_data: cannot open gps log file");
      return false;
    }
  }

  // Aggiorniamo buffer solo se il fix è valido (Status 'A' in RMC)
  if (rmc->valid) {
    float lat = minmea_tocoord(&(rmc->latitude));
    float lon = minmea_tocoord(&(rmc->longitude));

    // Formato CSV: HHMMSS, LAT, LON, SPEED
    snprintf(buffer, sizeof(buffer), "%02d%02d%02d,%.6f,%.6f,%.2f\n",
             rmc->time.hours, rmc->time.minutes, rmc->time.seconds, lat, lon); 
  }

  if(buffer != "") fwrite(buffer, sizeof(char), 128, f);
  fclose(f);
  return true;
}


// Print functions (for debug)
// Print functions for each NMEA sentence type
void print_NMEA_rmc(void* frame) {
  #if DEBUG
  struct minmea_sentence_rmc* rmc = (struct minmea_sentence_rmc*)frame;
  Serial.printf("$xxRMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n",
                rmc->latitude.value, rmc->latitude.scale,
                rmc->longitude.value, rmc->longitude.scale,
                rmc->speed.value, rmc->speed.scale);
  Serial.printf("$xxRMC fixed-point coordinates and speed scaled to three "
                "decimal places: (%d,%d) %d\n",
                minmea_rescale(rmc->latitude, 1000),
                minmea_rescale(rmc->longitude, 1000),
                minmea_rescale(rmc->speed, 1000));
  Serial.printf("$xxRMC floating point degree coordinates and speed: (%f,%f) %f\n",
                minmea_tocoord(rmc->latitude), minmea_tocoord(rmc->longitude),
                minmea_tofloat(rmc->speed));
  #endif
  return;
}

void print_NMEA_gga(void* frame) {
    struct minmea_sentence_gga* gga = (struct minmea_sentence_gga*)frame;
    Serial.printf("$xxGGA: fix quality: %d\n", gga->fix_quality);
    return;
}

void print_NMEA_gst(void* frame) {
  #if DEBUG
  struct minmea_sentence_gst* gst = (struct minmea_sentence_gst*)frame;
  Serial.printf("$xxGST: raw latitude,longitude and altitude error deviation: (%d/%d,%d/%d,%d/%d)\n",
                gst->latitude_error_deviation.value, gst->latitude_error_deviation.scale,
                gst->longitude_error_deviation.value, gst->longitude_error_deviation.scale,
                gst->altitude_error_deviation.value, gst->altitude_error_deviation.scale);
  Serial.printf("$xxGST fixed point latitude,longitude and altitude error deviation scaled to one decimal place: (%d,%d,%d)\n",
                minmea_rescale(gst->latitude_error_deviation, 10),
                minmea_rescale(gst->longitude_error_deviation, 10),
                minmea_rescale(gst->altitude_error_deviation, 10));
  Serial.printf("$xxGST floating point degree latitude, longitude and altitude error deviation: (%f,%f,%f)",
                minmea_tofloat(gst->latitude_error_deviation),
                minmea_tofloat(gst->longitude_error_deviation),
                minmea_tofloat(gst->altitude_error_deviation));
  #endif
  return;
}

void print_NMEA_gsv(void* frame) {
  #if DEBUG
  struct minmea_sentence_gsv* gsv = (struct minmea_sentence_gsv*)frame;
  Serial.printf("$xxGSV: message %d of %d\n", gsv->msg_nr, gsv->total_msgs);
  Serial.printf("$xxGSV: satellites in view: %d\n", gsv->total_sats);
  for (int i = 0; i < 4; i++) {
      Serial.printf("$xxGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
                    gsv->sats[i].nr, gsv->sats[i].elevation, gsv->sats[i].azimuth,
                    gsv->sats[i].snr);
  }
  #endif
  return;
}

void print_NMEA_vtg(void* frame) {
  #if DEBUG
  struct minmea_sentence_vtg* vtg = (struct minmea_sentence_vtg*)frame;
  Serial.printf("$xxVTG: true track degrees = %f\n", minmea_tofloat(vtg->true_track_degrees));
  Serial.printf("magnetic track degrees = %f\n", minmea_tofloat(vtg->magnetic_track_degrees));
  Serial.printf("speed knots = %f\n", minmea_tofloat(vtg->speed_knots));
  Serial.printf("speed kph = %f\n", minmea_tofloat    (vtg->speed_kph));
  #endif
  return;
}

void print_NMEA_zda(void* frame) {
  #if DEBUG
  struct minmea_sentence_zda* zda = (struct minmea_sentence_zda*)frame;
  Serial.printf("$xxZDA: %d:%d:%d %02d.%02d.%d UTC%+03d:%02d\n",
                zda->time.hours, zda->time.minutes, zda->time.seconds,
                zda->date.day, zda->date.month, zda->date.year,
                zda->hour_offset, zda->minute_offset);
  #endif
  return;
}
