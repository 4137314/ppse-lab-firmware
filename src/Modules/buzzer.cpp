#include "buzzer.h"

static uint8_t buzzerPin;

void buzzerInit(uint8_t pin) {
    buzzerPin = pin;
    pinMode(buzzerPin, OUTPUT);
    Serial.println("Buzzer initialized");
    //buzzerBeep();
    //Serial.println("Buzzer beep sent");
}

void buzzerBeep(uint16_t freq, uint16_t duration) {
    tone(buzzerPin, freq);
    delay(duration);
    noTone(buzzerPin);
}