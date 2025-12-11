#include "gps.h"
#include "display_ui.h"
#include <TinyGPSPlus.h>
#include <Arduino.h>

// GPS object
TinyGPSPlus gps;




void gpsInit() {
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

    //UART1 per GPS (RX=5, TX=4), Serial2 maps to UART1 in Arduino libs!
    Serial2.setTX(4);
    Serial2.setRX(5);
    Serial2.begin(9600); // enable uart1 on 4 and 5
    Serial.print("UART1 started on Serial2, listening for GPS data...");
    int i=0;
    while( i<1000 && (Serial2.available() == 0) ){ 
        delay(1);
        ++i;
    }
    if (i==1000){
        Serial.println("");
        Serial.println("ERROR: No data from GPS module");
        return;
    }
    Serial.println("OK!");
    return;
}

String* gpsRead() {
    String* str= (String*) malloc(sizeof(String));

    int Byte;
    if( Serial2.available() > 0) //check buffer of serial (max 64B)
    {
        //BLINK led
        digitalWrite(LED_ALIVE, 1);
        digitalWrite(20, 0);
        
        // Void GPS buffer till the next $GPGGA
        Serial2.readStringUntil("$GPGGA");
        Serial2.readStringUntil('\n');

        
        if (str!="")
        {
            Serial.print(str); //
        }
    } 
    else
    {
        Serial.println("WARNING: No data to read from GPS module");  
    }
    

    analogRead(TEMP_PIN);

}
