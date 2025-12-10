#include "temp.h"
#include <Arduino.h>

// Inizializza il pin del sensore
void TempInit() {
    pinMode(TEMP_PIN, INPUT);
    analogReadResolution(ADC_BITS); //set ADC resolution bits

    Serial.print("Sensore di temperatura: pin ");
    Serial.print(TEMP_PIN);
    Serial.println("inizializzato come input.");
}

// Lettura valore ADC
float readTemp() {

    int raw = analogRead(TEMP_PIN);
    if(raw < 0 || raw > ADC_MAX_VAL) {
        Serial.println("Warning: valore ADC fuori range");
        return raw;
    }
    // Conversione da raw a mV
    // RP2040 ADC 12bit, 3.3V
    float temp = ( (3300/ADC_MAX_VAL)*raw -500)/10 ; 
    return temp;
}