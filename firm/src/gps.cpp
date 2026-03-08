#include "gps.h"

bool gpsInit() {
    Serial.println("=== GPS Init ===");

    // Led alive init
    pinMode(LED_ALIVE, OUTPUT);
    digitalWrite(LED_ALIVE, LOW);  // LED alive off
    Serial.print("Led alive pin: ");
    Serial.print(LED_ALIVE);
    Serial.println(" inizializzato come output.");

    // Enable GPS module
    pinMode(GPS_EN_PIN, OUTPUT);
    digitalWrite(GPS_EN_PIN, LOW);  // bring down GPS_EN signal to enable (pmos)
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
    Serial2.begin(GPSBAUD);  // start uart1
    Serial.println("UART1 started on Serial2, listening for GPS data...");

    // Wait at least 10 sec for a GPS string.
    int i;
    for (i = 0; i < 1000 && (Serial2.available() == 0); ++i) delay(1);

    if (i == 1000) {
        Serial.println("ERROR: Canot start the GPS module");
        return false;
    }
    Serial.println("OK!");

    return true;
}

/*
void EmptyGpsBuffer(){
  while(Serial2.available()) {
    Serial2.read();  // Read and discard
  }
}
*/

bool nmea_gps_parse(String* nmea_message, struct parsed_nmea* nmea_ptr) {
    bool state = true;

    if (nmea_ptr == NULL) {
        Serial.println("ERROR in nmea_gps_parse: nmea struct is NULL");
        return false;
    }

    switch (minmea_sentence_id(nmea_message->c_str(), true)) {
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc* frame = &(nmea_ptr->parsed_rmc);
            if (minmea_parse_rmc(frame, nmea_message->c_str()))
                print_NMEA_rmc(frame);
            else
                (state = false);
        } break;

        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga* frame = &(nmea_ptr->parsed_gga);
            if (minmea_parse_gga(frame, nmea_message->c_str()))
                print_NMEA_gga(frame);
            else
                (state = false);
        } break;

        case MINMEA_SENTENCE_GST: {
            struct minmea_sentence_gst* frame = &(nmea_ptr->parsed_gst);
            if (minmea_parse_gst(frame, nmea_message->c_str()))
                print_NMEA_gst(frame);
            else
                (state = false);
        } break;

        case MINMEA_SENTENCE_GSV: {
            struct minmea_sentence_gsv* frame = &(nmea_ptr->parsed_gsv);
            if (minmea_parse_gsv(frame, nmea_message->c_str()))
                print_NMEA_gsv(frame);
            else
                (state = false);
        } break;

        case MINMEA_SENTENCE_VTG: {
            struct minmea_sentence_vtg* frame = &(nmea_ptr->parsed_vtg);
            if (minmea_parse_vtg(frame, nmea_message->c_str()))
                print_NMEA_vtg(frame);
            else
                (state = false);
        } break;

        case MINMEA_SENTENCE_ZDA: {
            struct minmea_sentence_zda* frame = &(nmea_ptr->parsed_zda);
            if (minmea_parse_zda(frame, nmea_message->c_str()))
                print_NMEA_zda(frame);
            else
                (state = false);
        } break;
        default: {
            (state = false);
        }
    }

    if (state)
        return true;
#if DEBUG
    Serial.print("WARNING: Could not parse this nmea string ");
    Serial.println(nmea_message->c_str());
#endif
    return false;
}

bool gpsAcquire(enum minmea_sentence_id sentence_type, struct parsed_nmea* nmea_ptr) {
    String nmea_message;

    digitalWrite(LED_ALIVE, HIGH);

    for (int i = 0; i < GPS_ACQUIRE_MAX_TRIES; ++i) {
        for (int c = 0; Serial2.available() == 0; ++c) delay(1);

        nmea_message = Serial2.readStringUntil('\n');
        digitalWrite(LED_ALIVE, LOW);

        if (minmea_sentence_id(nmea_message.c_str(), true) == sentence_type) {
            if (nmea_gps_parse(&nmea_message, nmea_ptr))
                return true;
        }
    }

#if DEBUG
    Serial.println(
        "WARNING in gpsAcquire: max tries reached before correctly "
        "acquiring the requested gps message\n");
#endif
    return false;
}

bool GetPosition_and_Satellites(struct parsed_nmea* nmea_ptr) {
    if (gpsAcquire(MINMEA_SENTENCE_GGA, nmea_ptr)) {
        return true;
    }
#if DEBUG
    Serial.println("WARNING in GetPosition_and_Satellites: failed to get data");
#endif
    return false;
}

bool GetDate_and_Time(struct parsed_nmea* nmea_ptr) {
    if (gpsAcquire(MINMEA_SENTENCE_RMC, nmea_ptr)) {
        return true;
    }
#if DEBUG
    Serial.println("WARNING in GetDate_and_Time: failed to get data");
#endif
    return false;
}

bool GPS_sync(struct parsed_nmea* nmea_ptr, bool force) {
    // retain value across function calls
    static unsigned long lastGPSsync = 0, lastGPSwrite = 0;

    if ((millis() - lastGPSsync > GPS_SYNC_TIMEOUT_MSEC) || force) {
        if (GetPosition_and_Satellites(nmea_ptr) && GetDate_and_Time(nmea_ptr)) {
            lastGPSsync = millis();
            if (((millis() - lastGPSwrite) > GPS_WRITE_FREQ) ||
                force)  // Write only at a certain frequency otherwise flash will wear
                        // out
            {
                if (save_gps_last(nmea_ptr))
                    lastGPSwrite = millis();
                else
                    return false;
            }
        } else  // It's only a warning so return true at the end
        {
#if DEBUG
            Serial.println("WARNING in GPS_sync: failed to get data");
#endif
        }
    }
    return true;
}

bool save_gps_last(struct parsed_nmea* nmea_ptr) {
    if (inPrinting || driveConnected || updated)
        return false;

    struct minmea_sentence_gga* gga = &(nmea_ptr->parsed_gga);
    if (gga->fix_quality <= 0)
        return false;
    char line[96];
    float lat = minmea_tocoord(&(gga->latitude));
    float lon = minmea_tocoord(&(gga->longitude));

    inPrinting = true;
    FILE* f    = fopen(GPS_LAST_PATH, "w");  // sovrascrive sempre
    if (!f) {
        inPrinting = false;
        return false;
    }
    int n = snprintf(line, sizeof(line), "%.6f,%.6f\n", lat, lon);
    fwrite(line, 1, (size_t)n, f);
    fclose(f);
    inPrinting = false;
    return true;
}

bool save_gps_log20(struct parsed_nmea* nmea_ptr) {
    if (inPrinting || driveConnected || updated)
        return false;

    struct minmea_sentence_rmc* rmc = &(nmea_ptr->parsed_rmc);
    if (!rmc->valid)
        return false;

    // nuova riga da aggiungere
    char newline[96];
    float lat = minmea_tocoord(&(rmc->latitude));
    float lon = minmea_tocoord(&(rmc->longitude));
    int newn  = snprintf(newline, sizeof(newline), "%.6f,%.6f\n", lat, lon);
    if (newn <= 0)
        return false;

    // buffer per massimo 20 righe
    char lines[GPS_MAX_LINES][96] = {{0}};
    int count                     = 0;

    inPrinting = true;

    // 1) leggi file esistente (se c'è)
    FILE* f = fopen(GPS_LOG_PATH, "r");
    if (f) {
        char tmp[96];
        while (fgets(tmp, sizeof(tmp), f)) {
            // copia la riga, limitando dimensione
            strncpy(lines[count % GPS_MAX_LINES], tmp, sizeof(lines[0]) - 1);
            lines[count % GPS_MAX_LINES][sizeof(lines[0]) - 1] = '\0';
            count++;
            if (count > 200)
                break;  // safety (non dovrebbe mai succedere)
        }
        fclose(f);
    }

    // 2) costruisci nuova lista: ultime 19 + nuova
    int start    = (count > GPS_MAX_LINES - 1) ? (count - (GPS_MAX_LINES - 1)) : 0;
    int outCount = 0;

    char out[GPS_MAX_LINES][96] = {{0}};
    for (int i = start; i < count; i++) {
        strncpy(out[outCount], lines[i % GPS_MAX_LINES], sizeof(out[0]) - 1);
        out[outCount][sizeof(out[0]) - 1] = '\0';
        outCount++;
    }

    // aggiungi nuova riga
    strncpy(out[outCount], newline, sizeof(out[0]) - 1);
    out[outCount][sizeof(out[0]) - 1] = '\0';
    outCount++;

    // 3) riscrivi file con esattamente outCount righe (max 20)
    f = fopen(GPS_LOG_PATH, "w");
    if (!f) {
        inPrinting = false;
        return false;
    }

    for (int i = 0; i < outCount; i++) {
        size_t n = strlen(out[i]);
        if (n)
            fwrite(out[i], 1, n, f);
    }
    fclose(f);

    inPrinting = false;
    return true;
}

void print_NMEA_rmc(void* frame) {
#if DEBUG
    struct minmea_sentence_rmc* rmc = (struct minmea_sentence_rmc*)frame;
    Serial.printf("$xxRMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n", rmc->latitude.value,
                  rmc->latitude.scale, rmc->longitude.value, rmc->longitude.scale, rmc->speed.value,
                  rmc->speed.scale);
    Serial.printf(
        "$xxRMC fixed-point coordinates and speed scaled to three "
        "decimal places: (%d,%d) %d\n",
        minmea_rescale(rmc->latitude, 1000), minmea_rescale(rmc->longitude, 1000),
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
    Serial.printf(
        "$xxGST: raw latitude,longitude and altitude error deviation: "
        "(%d/%d,%d/%d,%d/%d)\n",
        gst->latitude_error_deviation.value, gst->latitude_error_deviation.scale,
        gst->longitude_error_deviation.value, gst->longitude_error_deviation.scale,
        gst->altitude_error_deviation.value, gst->altitude_error_deviation.scale);
    Serial.printf(
        "$xxGST fixed point latitude,longitude and altitude error "
        "deviation scaled to one decimal place: (%d,%d,%d)\n",
        minmea_rescale(gst->latitude_error_deviation, 10),
        minmea_rescale(gst->longitude_error_deviation, 10),
        minmea_rescale(gst->altitude_error_deviation, 10));
    Serial.printf(
        "$xxGST floating point degree latitude, longitude and altitude "
        "error deviation: (%f,%f,%f)",
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

void print_NMEA_zda(void* frame) {
#if DEBUG
    struct minmea_sentence_zda* zda = (struct minmea_sentence_zda*)frame;
    Serial.printf("$xxZDA: %d:%d:%d %02d.%02d.%d UTC%+03d:%02d\n", zda->time.hours,
                  zda->time.minutes, zda->time.seconds, zda->date.day, zda->date.month,
                  zda->date.year, zda->hour_offset, zda->minute_offset);
#endif
    return;
}

void print_NMEA_vtg(void* frame) {
#if DEBUG
    struct minmea_sentence_vtg* vtg = (struct minmea_sentence_vtg*)frame;
    Serial.printf("$xxVTG: true track degrees = %f\n", minmea_tofloat(vtg->true_track_degrees));
    Serial.printf("magnetic track degrees = %f\n", minmea_tofloat(vtg->magnetic_track_degrees));
    Serial.printf("speed knots = %f\n", minmea_tofloat(vtg->speed_knots));
    Serial.printf("speed kph = %f\n", minmea_tofloat(vtg->speed_kph));
#endif
    return;
}

void gpsSerialDebugLog() {
    static char nmea[MINMEA_MAX_SENTENCE_LENGTH];
    static uint8_t idx = 0;

    while (Serial2.available()) {
        char c = Serial2.read();

        if (c == '\n') {
            nmea[idx] = '\0';

            Serial.print("GPS RAW: ");
            Serial.println(nmea);

            idx = 0;  // reset buffer
        } else {
            if (idx < MINMEA_MAX_SENTENCE_LENGTH - 1) {
                nmea[idx++] = c;
            }
        }
    }
}

void check_GPS_sync_req(queue_t* recv_queue, queue_t* send_queue, struct parsed_nmea* nmea_ptr) {
    if (!queue_is_empty(recv_queue)) {
        uint8_t cmd;
        queue_remove_blocking(recv_queue, &cmd);  // read the item in the queue
        if (cmd == GPS_SYNC_REQ) {
            if (GPS_sync(nmea_ptr, true))
                exit(EXIT_FAILURE);
            cmd = GPS_SYNC_ACK;
            if (queue_try_add(send_queue, &cmd))
                ;
        }
    }
    return;
}
