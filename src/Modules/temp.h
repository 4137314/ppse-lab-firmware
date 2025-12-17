#ifndef TEMP_H
#define TEMP_H

#include <Arduino.h>

#define TEMP_PIN 26 //GPIO temperature sensor
#define ADC_BITS 12
#define ADC_MAX_VAL 4095 // =2^(ADC_BITS) -1

    // Inizializza il pin del sensore e controlla se è valido
    void TempInit();

    // Lettura in gradi Celsius (LM35)
    float readTemp();

#endif
