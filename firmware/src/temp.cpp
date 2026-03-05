/**
 * @file temp.cpp
 * @brief Implementazione della lettura della temperatura analogica.
 * * Gestisce l'interfaccia con il sensore di temperatura (tipicamente un
 * TC1047A) attraverso il convertitore Analogico-Digitale (ADC) dell'RP2040.
 * Include le formule di conversione dai valori grezzi (raw) ai gradi Celsius.
 */

#include "temp.h"

#include <Arduino.h>

#include "hardware/adc.h"
#include "pico/stdlib.h"

/**
 * @brief Inizializza l'hardware dell'ADC per la lettura termica.
 * * Configura la risoluzione dell'ADC a 12 bit (0-4095) per garantire
 * la massima precisione consentita dall'hardware RP2040.
 */
void TempInit() {
  // Imposta la risoluzione ADC a 12 bit (standard per RP2040)
  analogReadResolution(12);

  // Esegue una lettura a vuoto per stabilizzare l'ADC
  analogReadTemp();
}

/**
 * @brief Legge il sensore e calcola la temperatura in Celsius.
 * * @details La conversione segue le specifiche del sensore TC1047:
 * 1. Acquisisce il valore grezzo dall'ADC (12 bit).
 * 2. Converte il valore in tensione ($V_{out}$) considerando il riferimento
 * a 3.3V.
 * 3. Applica la formula: $T = \frac{V_{out} - 0.5V}{10mV/°C}$.
 * * @return float La temperatura misurata in gradi Celsius (°C).
 */
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
