#include "gps.h"

bool gpsInit(){
    Serial.println("=== GPS Init ===");
    
    pinMode(LED_ALIVE, OUTPUT);
    digitalWrite(LED_ALIVE, LOW);
    // LED alive
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

    //Serial2.setTimeout(100);

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

    // Allocate NMEA message space in memory
    Serial.print("Allocating array for NMEA messages...");

    return true;
}

bool gpsRead(String* nmea_message){
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
            //Serial2.flush(); //waits for transmission to finish
            //while(Serial2.available()<MINMEA_MAX_SENTENCE_LENGTH);

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
            if(Buffer[j].charAt(0)=='$'){
            nmea_message[j]=Buffer[j]; //save the temp buffer
            Serial.print(nmea_message[j]);
            }
            Serial.println();
        }
    }
    else
    {
        Serial.println("WARNING: No data to read from GPS module");  
        return false;
    }
     return true;
}

bool gpsRead_fast(String* nmea_message){

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

        for(int i=0; i<NMEA_SENTENCE_COUNT; ++i)
        {
            Serial2.flush(); //waits for transmission to finish
            //while(Serial2.available()<MINMEA_MAX_SENTENCE_LENGTH);

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
            if(Buffer[j].charAt(0)=='$'){
            nmea_message[j]=Buffer[j]; //save the temp buffer
            Serial.print(nmea_message[j]);
            }
            Serial.println();
        }
    }
    else
    {
        Serial.println("WARNING: No data to read from GPS module");  
        return false;
    }
     return true;
}
