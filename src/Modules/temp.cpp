#include "temp.h"

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

    // CONVERSIONE CORRETTA
    // Usiamo .0 per forzare i calcoli float ed evitare la divisione intera
    // Vout (mV) = (raw / ADC_MAX_VAL) * 3300.0
    float vout = (static_cast<float>(raw) * 3300.0) / static_cast<float>(ADC_MAX_VAL);
    
    // Formula dal datasheet: Temp = (Vout - 500mV) / 10mV/C
    float tempCelsius = (vout - 500.0) / 10.0;

    return tempCelsius;
}
