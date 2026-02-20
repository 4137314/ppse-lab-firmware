 #include "gps.h"

struct parsed_nmea global_parsed_nmea; // global variable to hold parsed nmea sentences

bool gpsInit(){
    Serial.println("=== GPS Init ===");
    
    // Led alive init
    pinMode(LED_ALIVE, OUTPUT);
    digitalWrite(LED_ALIVE, LOW);     // LED alive off
    Serial.print("Led alive pin: ");
    Serial.print(LED_ALIVE);
    Serial.println(" inizializzato come output.");

    // Enable GPS module
    pinMode(GPS_EN_PIN, OUTPUT);
    digitalWrite(GPS_EN_PIN, LOW);  //bring down GPS_EN signal to enable (pmos)
    Serial.print("GPS enable pin: ");
    Serial.print(GPS_EN_PIN);
    Serial.println(" alimentato.");

    // Enable input for Pulse signal from gps
    pinMode(GPS_PPS_PIN, INPUT);
    Serial.print("Gps PPS pin: ");
    Serial.print(GPS_PPS_PIN);
    Serial.println(" inizializzato come input.");
    
    //UART1 for the GPS. Serial2 maps to UART1 in Arduino framework!
    Serial2.setTX(UART1_TX_PIN);
    Serial2.setRX(UART1_RX_PIN);
    Serial2.begin(GPSBAUD); // start uart1
    Serial.println("UART1 started on Serial2, listening for GPS data...");


    int i=0;
    while( i<1000 && (Serial2.available() == 0) ) //Wait at least 10 sec for a GPS string.
    { 
        delay(1);
        ++i;
    }
    
    if (i==1000)
    {
        Serial.println("ERROR: No data from GPS module");
        return false;
    }
    Serial.println("OK!");

    return true;
}

/*
void EmptyGpsBuffer(){
    Serial2.flush(); //waits for transmission to finish
    while(Serial2.available()) {
        Serial2.read();  // Read and discard
    }
    Serial2.readStringUntil('\n');
    Serial2.flush(); //waits for transmission to finish
}
*/


void EmptyGpsBuffer(){
    while(Serial2.available()) {
        Serial2.read();  // Read and discard
    }
}



bool nmea_gps_parse (String* nmea_message){

    //GLOBAL VARIABLE
    //struct parsed_nmea global_parsed_nmea;

    switch ( minmea_sentence_id(nmea_message->c_str(), true)) {
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc *frame = &global_parsed_nmea.parsed_rmc;
            if (minmea_parse_rmc(frame, nmea_message->c_str())) {
                #if DEBUG == 1
                Serial.printf("$xxRMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n",
                        frame->latitude.value, frame->latitude.scale,
                        frame->longitude.value, frame->longitude.scale,
                        frame->speed.value, frame->speed.scale);
                Serial.printf("$xxRMC fixed-point coordinates and speed scaled to three decimal places: (%d,%d) %d\n",
                        minmea_rescale(frame->latitude, 1000),
                        minmea_rescale(frame->longitude, 1000),
                        minmea_rescale(frame->speed, 1000));
                Serial.printf("$xxRMC floating point degree coordinates and speed: (%f,%f) %f\n",
                        minmea_tocoord(frame->latitude),
                        minmea_tocoord(frame->longitude),
                        minmea_tofloat(frame->speed));
                #endif
            }
            else {
                #if DEBUG == 1
                Serial.printf("$xxRMC sentence is not parsed\n");
                #endif
                return false;
            }
        } break;

        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga *frame = &global_parsed_nmea.parsed_gga;
            if (minmea_parse_gga(frame, nmea_message->c_str())) {
                #if DEBUG == 1
                Serial.printf("$xxGGA: fix quality: %d\n", frame->fix_quality);
                #endif
            }
            else {
                #if DEBUG == 1
                Serial.printf("$xxGGA sentence is not parsed\n");
                #endif
                return false;
            }
        } break;
    
        case MINMEA_SENTENCE_GST: {
            struct minmea_sentence_gst *frame = &global_parsed_nmea.parsed_gst;
            if (minmea_parse_gst(frame, nmea_message->c_str())) {
                #if DEBUG == 1
                Serial.printf("$xxGST: raw latitude,longitude and altitude error deviation: (%d/%d,%d/%d,%d/%d)\n",
                        frame->latitude_error_deviation.value, frame->latitude_error_deviation.scale,
                        frame->longitude_error_deviation.value, frame->longitude_error_deviation.scale,
                        frame->altitude_error_deviation.value, frame->altitude_error_deviation.scale);
                Serial.printf("$xxGST fixed point latitude,longitude and altitude error deviation"
                        " scaled to one decimal place: (%d,%d,%d)\n",
                        minmea_rescale(frame->latitude_error_deviation, 10),
                        minmea_rescale(frame->longitude_error_deviation, 10),
                        minmea_rescale(frame->altitude_error_deviation, 10));
                Serial.printf("$xxGST floating point degree latitude, longitude and altitude error deviation: (%f,%f,%f)",
                        minmea_tofloat(frame->latitude_error_deviation),
                        minmea_tofloat(frame->longitude_error_deviation),
                        minmea_tofloat(frame->altitude_error_deviation));
                #endif
            }
            else {
                #if DEBUG == 1
                Serial.printf("$xxGST sentence is not parsed\n");
                #endif
                return false;
            }
        } break;

        case MINMEA_SENTENCE_GSV: {
            struct minmea_sentence_gsv *frame = &global_parsed_nmea.parsed_gsv;
            if (minmea_parse_gsv(frame, nmea_message->c_str())) {
                #if DEBUG == 1
                Serial.printf("$xxGSV: message %d of %d\n", frame->msg_nr, frame->total_msgs);
                Serial.printf("$xxGSV: satellites in view: %d\n", frame->total_sats);
                for (int i = 0; i < 4; i++)
                    Serial.printf("$xxGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
                        frame->sats[i].nr,
                        frame->sats[i].elevation,
                        frame->sats[i].azimuth,
                        frame->sats[i].snr);
                #endif
            }
            else {
                #if DEBUG == 1
                Serial.printf("$xxGSV sentence is not parsed\n");
                #endif
                return false;
            }
        } break;

        case MINMEA_SENTENCE_VTG: {
            struct minmea_sentence_vtg *frame = &global_parsed_nmea.parsed_vtg;
            if (minmea_parse_vtg(frame, nmea_message->c_str())) {
                #if DEBUG == 1
                Serial.printf("$xxVTG: true track degrees = %f\n",
                        minmea_tofloat(frame->true_track_degrees));
                Serial.printf("        magnetic track degrees = %f\n",
                        minmea_tofloat(frame->magnetic_track_degrees));
                Serial.printf("        speed knots = %f\n",
                        minmea_tofloat(frame->speed_knots));
                Serial.printf("        speed kph = %f\n",
                        minmea_tofloat(frame->speed_kph));
                #endif
            }
            else {
                #if DEBUG == 1
                Serial.printf("$xxVTG sentence is not parsed\n");
                #endif
                return false;
            }
        } break;

        case MINMEA_SENTENCE_ZDA: {
            struct minmea_sentence_zda *frame = &global_parsed_nmea.parsed_zda;
            if (minmea_parse_zda(frame, nmea_message->c_str())) {
                #if DEBUG == 1
                Serial.printf("$xxZDA: %d:%d:%d %02d.%02d.%d UTC%+03d:%02d\n",
                        frame->time.hours,
                        frame->time.minutes,
                        frame->time.seconds,
                        frame->date.day,
                        frame->date.month,
                        frame->date.year,
                        frame->hour_offset,
                        frame->minute_offset);
                #endif
            }
            else {
                #if DEBUG == 1
                Serial.printf("$xxZDA sentence is not parsed\n");
                #endif
                return false;
            }
        } break;

        case MINMEA_INVALID: {
            #if DEBUG == 1
            Serial.print("WARNING: Could not parse this nmea string ");
            Serial.println(nmea_message->c_str());
            #endif
        } return false;

        default: {
            #if DEBUG == 1
            Serial.print("WARNING: Could not parse this nmea string ");
            Serial.println(nmea_message->c_str());
            #endif
        } return false;
    }

    return true;
}


/*
bool gpsAcquire(enum minmea_sentence_id sentence_type){
    
String nmea_message;

digitalWrite(LED_ALIVE, LOW);

if( Serial2.available() > 0) //check buffer for data (max 64B)
{
    digitalWrite(LED_ALIVE, HIGH);
    
    EmptyGpsBuffer();
    
    for(int i=0; i<GPS_ACQUIRE_MAX_TRIES; ++i)
    {   
        nmea_message = Serial2.readStringUntil('\n');
        
        if ( minmea_sentence_id(nmea_message.c_str(), true) == sentence_type){
            if(nmea_gps_parse(&nmea_message)) {
                digitalWrite(LED_ALIVE, LOW);
                return true;
            }
        }
        EmptyGpsBuffer();
        
    }
    digitalWrite(LED_ALIVE, LOW);
    #if DEBUG == 1 
    Serial.println("ERROR in gpsAcquire: max tries reached before correctly acquiring the requested gps message");
    #endif
    return false;
}
else
{
    #if DEBUG == 1
    Serial.println("WARNING: No data to read from GPS module");  
    #endif
    return false;
}
}
//*/


bool gpsAcquire(enum minmea_sentence_id sentence_type) {
  uint32_t start = millis();

  while (millis() - start < 1200) { // finestra 1.2s
    if (Serial2.available()) {
      String nmea = Serial2.readStringUntil('\n');

      if (minmea_sentence_id(nmea.c_str(), true) == sentence_type) {
        return nmea_gps_parse(&nmea);
      }
    }
    delay(1);
  }
  return false;
}



bool GetDate_and_Time(){

    if(gpsAcquire(MINMEA_SENTENCE_RMC)){
        return true;
    }
    #if DEBUG == 1
        Serial.println("ERROR in GetDate_and_Time: failed to get data");
    #endif
    return false;
}




bool GetPosition_and_Satellites(){
    if(gpsAcquire(MINMEA_SENTENCE_GGA)){
        return true;
    }
    #if DEBUG == 1
    Serial.println("ERROR in GetPosition_and_Satellites: failed to get data");
    #endif
    return false;
}

bool gpsGetLatLon(float* latitude, float* longitude){
    if(!latitude || !longitude){
        #if DEBUG == 1
        Serial.println("ERROR in gpsGetLatLon: null pointer provided");
        #endif
        return false;
    }

    if(!gpsAcquire(MINMEA_SENTENCE_RMC)){
        #if DEBUG == 1
        Serial.println("ERROR in gpsGetLatLon: failed to get data");
        #endif
        return false;
    }

    const auto& rmc = global_parsed_nmea.parsed_rmc;

    if(!rmc.valid){
        #if DEBUG == 1
        Serial.println("ERROR in gpsGetLatLon: no fix available");
        #endif
        return false;
    }

    float lat = minmea_tocoord(&rmc.latitude);
    float lon = minmea_tocoord(&rmc.longitude);


    if(isnan(lat) || isnan(lon)){
        #if DEBUG == 1
        Serial.println("ERROR in gpsGetLatLon: latitude or longitude is NaN");
        #endif
        return false;
    }
    if(!(lat>=-90.0f && lat<=90.0f && lon>=-180.0f && lon<=180.0f)){
        #if DEBUG == 1
        Serial.println("ERROR in gpsGetLatLon: invalid latitude or longitude");
        #endif
        return false;
    }
    *latitude = lat;
    *longitude = lon;
    return true;

}


void printGpsStatus() {
    if(!gpsAcquire(MINMEA_SENTENCE_RMC) || !gpsAcquire(MINMEA_SENTENCE_GGA)){
        Serial.println("ERROR in printGpsStatus: failed to acquire GPS data");
        return;
    }

    auto &rmc = global_parsed_nmea.parsed_rmc;
    auto &gga = global_parsed_nmea.parsed_gga;

    Serial.println("---- GPS STATUS ----");

    Serial.print("Fix quality: ");
    Serial.println(gga.fix_quality);

    Serial.print("Satellites tracked: ");
    Serial.println(gga.satellites_tracked);
    Serial.print("HDOP: ");
    Serial.println(minmea_tofloat(&gga.hdop));  
    Serial.print("Altitude: "); 
    Serial.println(minmea_tofloat(&gga.altitude));
    Serial.print("Speed (knots): ");
    Serial.println(minmea_tofloat(&rmc.speed)); 
    Serial.print("Course (degrees): ");
    Serial.println(minmea_tofloat(&rmc.course));
    Serial.print("Date (DD/MM/YYYY): ");
    Serial.print(rmc.date.day); Serial.print("/");
    Serial.print(rmc.date.month); Serial.print("/");    
    Serial.println(rmc.date.year);
    Serial.print("Time (HH:MM:SS): ");
    Serial.print(rmc.time.hours + 1 ); Serial.print(":");
    Serial.print(rmc.time.minutes); Serial.print(":");    
    Serial.println(rmc.time.seconds);


    Serial.print("RMC valid: ");
    Serial.println(rmc.valid ? "YES" : "NO");

    if (rmc.valid && gga.fix_quality > 0) {
        float lat = minmea_tocoord(&rmc.latitude);
        float lon = minmea_tocoord(&rmc.longitude);

        Serial.print("Latitude : ");
        Serial.println(lat, 6);

        Serial.print("Longitude: ");
        Serial.println(lon, 6);
    } else {
        Serial.println("NO FIX");
    }

    Serial.println("--------------------");
}


// Ritorna true se fix valido e lat/lon disponibili
bool GPS_getFix(float &lat, float &lon, float &alt, int &sats, int &fixQ) {
  auto &gga = global_parsed_nmea.parsed_gga;
  auto &rmc = global_parsed_nmea.parsed_rmc;

  fixQ = gga.fix_quality;
  sats = gga.satellites_tracked;

  bool fix = (gga.fix_quality > 0) &&
             rmc.valid &&
             (gga.latitude.scale != 0) &&
             (gga.longitude.scale != 0);

  if (!fix) return false;

  lat = minmea_tocoord(&gga.latitude);
  lon = minmea_tocoord(&gga.longitude);

  alt = (gga.altitude.scale != 0) ? minmea_tofloat(&gga.altitude) : NAN;

  return true;
}

void GPS_sendToPC_periodic() {
  static bool firstFixSent = false;
  static uint32_t lastSendMs = 0;

  const uint32_t PERIOD_MS = 30UL * 60UL * 1000UL;

  float lat, lon, alt;
  int sats, fixQ;

  // prova a leggere un po’ (se già fai GPS_pollOnce altrove, puoi anche ometterlo)
  // GPS_pollOnce();

  bool ok = GPS_getFix(lat, lon, alt, sats, fixQ);
  if (!ok) {
    // se perdi fix, permetti nuovo "primo fix" quando torna
    firstFixSent = false;
    return;
  }

  uint32_t now = millis();

  // manda subito al primo fix
  if (!firstFixSent) {
    firstFixSent = true;
    lastSendMs = now;
  } else {
    // poi ogni 30 min
    if (now - lastSendMs < PERIOD_MS) return;
    lastSendMs = now;
  }

  Serial.print("GPS,");
  Serial.print(lat, 6); Serial.print(",");
  Serial.print(lon, 6); Serial.print(",");
  if (isnan(alt)) Serial.print("NA");
  else Serial.print(alt, 1);
  Serial.print(",");
  Serial.print(sats); Serial.print(",");
  Serial.println(fixQ);
}

bool GPS_poolOnce(int timeoutMs) {
    delay(timeoutMs) ; //give some time to gps to respond
    auto &gga = global_parsed_nmea.parsed_gga;
    auto &rmc = global_parsed_nmea.parsed_rmc;
    bool okGGA = gpsAcquire(MINMEA_SENTENCE_GGA);
    bool okRMC = gpsAcquire(MINMEA_SENTENCE_RMC);
    Serial.print("GPS_poolOnce: GGA ");    Serial.print(okGGA ? "OK" : "FAIL");
    Serial.print(", RMC ");    Serial.println(okRMC ? "OK" : "FAIL");

    return okGGA || okRMC;
}
/*


// nuovo gps.cpp

// gps.cpp (estratto rifattorizzato)

#include "gps.h"
#include <Arduino.h>
#include <string.h>
#include <math.h>

struct parsed_nmea global_parsed_nmea;

// Timestamp ultimo aggiornamento (ms)
static uint32_t lastRmcMs = 0;
static uint32_t lastGgaMs = 0;

// Buffer linea NMEA (evita String -> più stabile)
static char nmeaLine[MINMEA_MAX_SENTENCE_LENGTH + 4];
static size_t nmeaLen = 0;

// --- Parsing su C-string (no String) ---
static bool nmea_gps_parse_cstr(const char *line) {
    switch (minmea_sentence_id(line, true)) {
        case MINMEA_SENTENCE_RMC: {
            auto *frame = &global_parsed_nmea.parsed_rmc;
            if (!minmea_parse_rmc(frame, line)) return false;
            lastRmcMs = millis();
            return true;
        }
        case MINMEA_SENTENCE_GGA: {
            auto *frame = &global_parsed_nmea.parsed_gga;
            if (!minmea_parse_gga(frame, line)) return false;
            lastGgaMs = millis();
            return true;
        }
        // Se vuoi, puoi aggiungere anche GSA/GSV/VTG/ZDA, ma NON è necessario per fix base.
        default:
        return false;
    }
}

// --- Init: solo hardware/UART ---
bool gpsInit() {
    Serial.println("=== GPS Init ===");
    
    pinMode(LED_ALIVE, OUTPUT);
    digitalWrite(LED_ALIVE, LOW);
    
    pinMode(GPS_EN_PIN, OUTPUT);
    digitalWrite(GPS_EN_PIN, LOW); // active low enable
    
    pinMode(GPS_PPS_PIN, INPUT);
    
    Serial2.setTX(UART1_TX_PIN);
    Serial2.setRX(UART1_RX_PIN);
    Serial2.begin(GPSBAUD);
    
    Serial.println("UART1 started on Serial2, listening for GPS data...");
    // Non bloccare qui ad aspettare dati: arrivano quando arrivano.
    return true;
}

// --- Poll non bloccante: chiamala spesso nel loop() ---
void gpsPoll() {
    // LED activity (facoltativo): acceso se arrivano byte
    bool any = false;
    
    while (Serial2.available() > 0) {
        any = true;
        char c = (char)Serial2.read();
        
        // Gestione CR/LF
        if (c == '\r') continue;
        
        if (c == '\n') {
            // Fine linea: chiudi stringa e parse
            nmeaLine[nmeaLen] = '\0';
            if (nmeaLen > 6) {
                nmea_gps_parse_cstr(nmeaLine);
            }
            nmeaLen = 0; // reset per prossima linea
            continue;
        }
        
        // Accumula se spazio disponibile
        if (nmeaLen < MINMEA_MAX_SENTENCE_LENGTH) {
            nmeaLine[nmeaLen++] = c;
        } else {
            // Overflow: reset (linea corrotta)
            nmeaLen = 0;
        }
    }
    
    digitalWrite(LED_ALIVE, any ? HIGH : LOW);
}

// --- API: non leggono Serial2, leggono SOLO lo stato ---
bool gpsHasFix(uint32_t freshnessMs = 2000) {
    const auto &gga = global_parsed_nmea.parsed_gga;
    const auto &rmc = global_parsed_nmea.parsed_rmc;
    
    uint32_t now = millis();
    bool fresh = (now - lastGgaMs <= freshnessMs) && (now - lastRmcMs <= freshnessMs);
    
    return fresh && (gga.fix_quality > 0) && rmc.valid;
}

bool gpsGetLatLon(float *latitude, float *longitude) {
    if (!latitude || !longitude) return false;
    
    const auto &rmc = global_parsed_nmea.parsed_rmc;
    if (!rmc.valid) return false;
    
    float lat = minmea_tocoord(&rmc.latitude);
    float lon = minmea_tocoord(&rmc.longitude);
    
    if (isnan(lat) || isnan(lon)) return false;
    if (!(lat >= -90.0f && lat <= 90.0f && lon >= -180.0f && lon <= 180.0f)) return false;
    
    *latitude = lat;
    *longitude = lon;
    return true;
}

// Versione coerente della tua GPS_getFix (ora non chiama gpsAcquire)
bool GPS_getFix(float &lat, float &lon, float &alt, int &sats, int &fixQ) {
    auto &gga = global_parsed_nmea.parsed_gga;
    auto &rmc = global_parsed_nmea.parsed_rmc;
    
    fixQ = gga.fix_quality;
    sats = gga.satellites_tracked;
    
    // Richiede fix + RMC valido
    bool fix = (gga.fix_quality > 0) && rmc.valid &&
    (gga.latitude.scale != 0) && (gga.longitude.scale != 0);
    
    if (!fix) return false;
    
    lat = minmea_tocoord(&gga.latitude);
    lon = minmea_tocoord(&gga.longitude);
    alt = (gga.altitude.scale != 0) ? minmea_tofloat(&gga.altitude) : NAN;
    
    return true;
}

// Periodico verso PC: presuppone che gpsPoll() venga chiamata nel loop
void GPS_sendToPC_periodic() {
    static bool firstFixSent = false;
    static uint32_t lastSendMs = 0;
    
    const uint32_t PERIOD_MS = 30UL * 60UL * 1000UL;
    float lat, lon, alt;
    int sats, fixQ;
    
    bool ok = GPS_getFix(lat, lon, alt, sats, fixQ);
    if (!ok) {
        firstFixSent = false;
        return;
    }
    
    uint32_t now = millis();
    
    if (!firstFixSent) {
        firstFixSent = true;
        lastSendMs = now;
    } else {
        if (now - lastSendMs < PERIOD_MS) return;
        lastSendMs = now;
    }
    
    Serial.print("GPS,");
    Serial.print(lat, 6); Serial.print(",");
    Serial.print(lon, 6); Serial.print(",");
    if (isnan(alt)) Serial.print("NA");
    else Serial.print(alt, 1);
    Serial.print(",");
    Serial.print(sats); Serial.print(",");
    Serial.println(fixQ);
}

*/