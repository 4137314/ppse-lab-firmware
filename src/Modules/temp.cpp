#include "temp.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"


/*
// Inizializza il pin del sensore
void TempInit() {
    pinMode(TEMP_PIN, INPUT);
    // Assicuriamoci che l'ADC sia impostato alla risoluzione corretta (es. 12 bit)
    analogReadResolution(ADC_BITS); 

    Serial.print("Sensore TC1047A: pin ");
    Serial.print(TEMP_PIN);
    Serial.println(" inizializzato.");
}

// Lettura valore ADC e conversione in Celsius
float readTemp() {
    int raw = analogRead(TEMP_PIN);
    
    // Debug del valore grezzo
    Serial.print("ADC Raw: ");
    Serial.println(raw);

    // Controllo range (opzionale ma utile per cavi scollegati)
    if(raw < 0 || raw > ADC_MAX_VAL) {
        Serial.println("Errore: Valore ADC fuori range!");
        return -999.0; // Valore sentinella per indicare errore
    }

    // Conversione da raw a mV
    // RP2040 ADC 12bit, 3.3V
    float temp = ( (3300/ADC_MAX_VAL)*raw -500)/10 ; 
    return temp;
}*/

#include <Arduino.h>

void TempInit() {
  analogReadResolution(12); // dummy
  analogReadTemp(); // dummy
}

float readTemp() {
  uint16_t raw = analogReadTemp();
  float v = raw * 3.3f / 4095.0f;
  return (v - 0.5f) / 0.01f; // TC1047
}
