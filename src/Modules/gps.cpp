#include "gps.h"

bool gpsInit(){
    Serial.println("=== GPS Init ===");
    
    pinMode(LED_ALIVE, OUTPUT);
    digitalWrite(LED_ALIVE, LOW);
    // LED alive off

    Serial.print("Led alive: pin ");
    Serial.print(LED_ALIVE);
    Serial.println(" inizializzato come output.");

    // Enable GPS module
    pinMode(GPS_EN_PIN, OUTPUT);
    digitalWrite(GPS_EN_PIN, LOW);  //bring down GPS_EN signal to enable (pmos)
    Serial.print("GPS enable: pin ");
    Serial.print(GPS_EN_PIN);
    Serial.println(" alimentato.");

    
    //UART1 for the GPS. Serial2 maps to UART1 in Arduino framework!
    Serial2.setTX(UART1_TX_PIN);
    Serial2.setRX(UART1_RX_PIN);
    Serial2.begin(GPSBAUD); // start uart1
    Serial.print("UART1 started on Serial2, listening for GPS data...");


    int i=0;
    while( i<1000 && (Serial2.available() == 0) ) //Wait 10 sec for a GPS string.
    { 
        delay(1);
        ++i;
    }
    
    if (i==1000)
    {
        Serial.println("");
        Serial.println("ERROR: No data from GPS module");
        return false;
    }
    Serial.println("OK!");

    return true;
}

bool gpsAcquire(String* nmea_message){
    String Buffer[NMEA_SENTENCE_COUNT];

    digitalWrite(LED_ALIVE, LOW);

    if( Serial2.available() > 0) //check buffer of serial (max 64B)
    {
        digitalWrite(LED_ALIVE, HIGH);
        
        // Void UART1 buffer
        Serial2.flush();  
        while(Serial2.available()) {
            Serial2.read();  // Read and discard
        }
        Serial2.readStringUntil('\n');
        Serial2.flush(); //waits for transmission to finish
        for(int i=0; i<NMEA_SENTENCE_COUNT; ++i)
        {
            Buffer[i]=Serial2.readStringUntil('\n');
        }

        digitalWrite(LED_ALIVE, LOW);

        for(int j=0; j<NMEA_SENTENCE_COUNT; ++j)
        {   
            if( Buffer[j].length() > MINMEA_MAX_SENTENCE_LENGTH-2 )
            {
                Serial.println("ERROR: NIMEA_MAX_SENTENCE_LENGTH is too small for a GPS sentence");
                return false;
            }
           // if(Buffer[j].charAt(0)=='$'){   //
                nmea_message[j]=Buffer[j]; //save the temp buffer
                #if DEBUG == 1
                    Serial.print(nmea_message[j]);
                #endif
            //}

            #if DEBUG == 1
                Serial.println();
            #endif
        }
    }
    else
    {
        #if DEBUG == 1
        Serial.println("WARNING: No data to read from GPS module");  
        #endif
        return false;
    }
     return true;
}


bool minmea_gps_parse (String* nmea_message){

    //GLOBAL VARIABLE
    struct parsed_nmea global_parsed_nmea;

    switch ( minmea_sentence_id(nmea_message->c_str(), false)) {
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