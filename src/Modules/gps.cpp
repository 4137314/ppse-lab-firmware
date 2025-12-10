#include "gps.h"
#include "display_ui.h"
#include <TinyGPS++.h>
#include <Arduino.h>

// GPS object
TinyGPSPlus gps;

// Pin per UART1
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5
#define GPS_EN_PIN 18   // Enable GPS module
#define GPS_PPS_PIN 19  // opzionale
#define LED_ALIVE 20    // LED lampeggia quando riceve dati

bool gpsInit() {
    Serial.println("=== GPS Init ===");

    // LED alive
    pinMode(LED_ALIVE, OUTPUT);
    digitalWrite(LED_ALIVE, LOW);

    // Enable GPS
    pinMode(GPS_EN_PIN, OUTPUT);
    digitalWrite(GPS_EN_PIN, HIGH);  // HIGH = GPS acceso

    // PPS pin
    pinMode(GPS_PPS_PIN, INPUT);

    // Serial1 per GPS (RX=5, TX=4)
    Serial1.begin(9600);
    Serial.println("Serial1 started for GPS");

    // Non possiamo controllare il "Serial1 ready", assume ok
    return true;
}


// Da chiamare nel loop
void gpsUpdate() {
    bool received = false;

    while (Serial1.available()) {
        char c = Serial1.read();
        gps.encode(c);
        received = true;
        digitalWrite(LED_ALIVE, HIGH); // LED lampeggia su ogni char
        Serial.write(c);               // invia NMEA al monitor seriale
    }
    if (received) digitalWrite(LED_ALIVE, LOW);

    // Solo quando la posizione cambia
    if (gps.location.isUpdated()) {
        Serial.print("Lat: "); Serial.println(gps.location.lat(), 6);
        Serial.print("Lng: "); Serial.println(gps.location.lng(), 6);
        Serial.print("Alt: "); Serial.println(gps.altitude.meters());
        Serial.print("Sat: "); Serial.println(gps.satellites.value());
        Serial.println("---");
    }
}

// Accesso dati GPS da altri moduli
TinyGPSPlus& getGPS() {
    return gps;
}
