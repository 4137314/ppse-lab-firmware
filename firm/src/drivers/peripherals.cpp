/**
 * @file peripherals.cpp
 */

#include <Arduino.h>            
#include "drivers/config_pins.h"  // <--- DEVE STARE QUI IN ALTO
#include <stdint.h>             
#include "drivers/peripherals.h"
#define FASTLED_INTERNAL          // Opzionale: riduce i warning di FastLED
#include <FastLED.h>

// Ora NUM_LEDS è noto e il compilatore non darà errore
static CRGB leds[NUM_LEDS];     

void peripherals_init() {
    pinMode(BUZZER_PIN, OUTPUT);
    noTone(BUZZER_PIN);

    // Nota: Assicurati che RGB_LED_PIN sia definito in config_pins.h
    FastLED.addLeds<WS2812B, RGB_LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(40); 
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();

    pinMode(TEMP_SENS_PIN, INPUT);
}

// ... il resto del file rimane invariato ...

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
            peripherals_beep(1500, 80); 
            break;
            
        case FEEDBACK_ERROR:
            peripherals_set_led(255, 0, 0);   // Rosso
            peripherals_beep(400, 300);
            break;
            
        case FEEDBACK_NEUTRAL:
            peripherals_set_led(0, 0, 255);   // Blu
            peripherals_beep(1000, 40);
            break;
            
        case FEEDBACK_OFF:
            peripherals_set_led(0, 0, 0);
            noTone(BUZZER_PIN);
            break;
    }
}

/* --- Sensore di Temperatura con Filtro --- */

/**
 * @brief Rinomino in 'get' per matchare la chiamata in telemetry.cpp
 */
float peripherals_get_temperature() {
    // Media mobile semplice su 10 letture per stabilizzare il drone
    uint32_t raw_sum = 0;
    const uint8_t samples = 10;
    
    for(uint8_t i = 0; i < samples; i++) {
        raw_sum += analogRead(TEMP_SENS_PIN);
        delayMicroseconds(50); 
    }
    
    float raw_avg = (float)raw_sum / samples;
    
    // Conversione Voltaggio (Riferimento 3.3V, ADC 12-bit su RP2040)
    // Formula: V = (raw / 4095) * 3.3
    float voltage = raw_avg * (3.3f / 4095.0f);
    
    // Formula per MCP9700: (Vout - 500mV) / 10mV/°C
    float tempC = (voltage - 0.5f) * 100.0f;
    
    return tempC;
}
