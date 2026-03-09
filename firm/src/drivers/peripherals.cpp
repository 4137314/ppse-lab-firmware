/**
 * @file peripherals.cpp
 * @brief Gestione unificata di LED RGB, Buzzer e Sensore Temperatura.
 */

#include "drivers/peripherals.h"
#include <FastLED.h>

// --- Configurazione Interna Striscia LED ---
#define NUM_LEDS 8
CRGB leds[NUM_LEDS];

void peripherals_init() {
    // 1. Inizializzazione Buzzer
    pinMode(BUZZER_PIN, OUTPUT);
    noTone(BUZZER_PIN);

    // 2. Inizializzazione FastLED (WS2812B)
    FastLED.addLeds<WS2812B, LED_STRIP_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(40); // Luminosità iniziale di sicurezza
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();

    // 3. Inizializzazione ADC per Temperatura
    analogReadResolution(ADC_RESOLUTION); 
}

/* --- Feedback Visivo --- */

void peripherals_set_led(uint8_t r, uint8_t g, uint8_t b) {
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
}

/* --- Feedback Acustico --- */

void peripherals_beep(uint32_t freq, uint32_t duration) {
    tone(BUZZER_PIN, freq, duration);
}

/* --- Logica di Feedback Combinata --- */

void peripherals_trigger_feedback(FeedbackType type) {
    switch (type) {
        case FEEDBACK_SUCCESS:
            peripherals_set_led(0, 255, 0);   // Verde
            peripherals_beep(1500, 80);       // Beep corto e acuto
            break;
            
        case FEEDBACK_ERROR:
            peripherals_set_led(255, 0, 0);   // Rosso
            peripherals_beep(400, 300);       // Beep lungo e grave
            break;
            
        case FEEDBACK_NEUTRAL:
            peripherals_set_led(0, 0, 255);   // Blu
            peripherals_beep(1000, 40);       // "Click" sonoro
            break;
            
        case FEEDBACK_OFF:
            peripherals_set_led(0, 0, 0);
            noTone(BUZZER_PIN);
            break;
    }
}

/* --- Sensore di Temperatura --- */

float peripherals_read_temperature() {
    // Lettura grezza dall'ADC (0 - 4095 per 12-bit)
    int raw = analogRead(TEMP_ADC_PIN);
    
    // Conversione in Voltaggio (Riferimento 3.3V)
    float voltage = raw * (3.3f / (float)ADC_MAX_VALUE);
    
    /** * Formula per sensori lineari tipo TMP36:
     * Temp = (Voltage - 0.5V offset) * 100
     */
    float tempC = (voltage - 0.5f) * 100.0f;
    
    return tempC;
}
