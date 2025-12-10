#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

// BUZZER
#define BUZZER 21

void buzzerInit(uint8_t pin);
void buzzerBeep(uint16_t freq = 1200, uint16_t duration = 60);

#endif