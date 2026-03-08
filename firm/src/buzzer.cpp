#include "buzzer.h"

static uint8_t buzzerPin;

void buzzerInit(uint8_t pin) {
    buzzerPin = pin;
    pinMode(buzzerPin, OUTPUT);

#if DEBUG
    Serial.println("Buzzer initialized");
#endif

// Se la macro è definita a 1, fa il beep, altrimenti tace
#if BUZZER_INIT_BEEP == 1
    buzzerBeep();
#if DEBUG
    Serial.println("Buzzer init beep sent");
#endif
#endif
}

void buzzerBeep(uint16_t freq, uint16_t duration) {
    tone(buzzerPin, freq);
    delay(duration);
    noTone(buzzerPin);
}
