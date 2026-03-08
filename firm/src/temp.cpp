#include "temp.h"

#include <Arduino.h>

#include "hardware/adc.h"
#include "pico/stdlib.h"

void TempInit() {
    // Imposta la risoluzione ADC a 12 bit (standard per RP2040)
    analogReadResolution(12);

    // Esegue una lettura a vuoto per stabilizzare l'ADC
    analogReadTemp();
}

float readTemp() {
    // Legge il valore grezzo dall'ADC (0 - 4095)
    uint16_t raw = analogReadTemp();

    // Conversione in tensione (Volt)
    float v = raw * 3.3f / 4095.0f;

    /**
     * Formula di conversione per TC1047:
     * Vout = 500mV a 0°C, con un coefficiente di 10mV/°C.
     * Temp = (Vout - 0.5) / 0.01
     */
    return (v - 0.5f) / 0.01f;
}
