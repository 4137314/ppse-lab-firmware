/**
 * @file peripherals.cpp
 * @brief Implementazione dei driver per le periferiche di feedback (LED, Buzzer) e sensori analogici.
 */

#include "drivers/config_pins.h"  // Fornisce la mappatura reale dei pin post-refactoring

#include "drivers/peripherals.h"

#include <Arduino.h>
#include <stdint.h>

#include "core/config.h" // <--- Assicurati che questo sia il file che contiene la definizione di ConfigData

// Commentato per eliminare il warning "FASTLED_INTERNAL redefined" poiché già iniettato globalmente
// #define FASTLED_INTERNAL          

#include <FastLED.h>

// Array dei LED allocato dinamicamente tramite la macro globale NUM_LEDS di config_pins.h
static CRGB leds[NUM_LEDS];

extern SystemConfig global_cfg;

/**
 * @brief Inizializzazione elettrica pura e immediata della PCB.
 * @details Questa funzione deve contenere esclusivamente operazioni GPIO dirette senza
 * chiamate a librerie complesse, per garantire l'accensione istantanea dei bus.
 */
void peripherals_init() {
    // 1. CONFIGURAZIONE IMMEDIATA DEL PIN DEL LED ALIVE
    pinMode(LED_ALIVE_PIN, OUTPUT);
    
    // Test visivo istantaneo all'accensione (3 lampeggi veloci)
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_ALIVE_PIN, HIGH);
        delay(100);
        digitalWrite(LED_ALIVE_PIN, LOW);
        delay(100);
    }
    // Lo lasciamo acceso fisso per indicare che il setup iniziale è partito
    digitalWrite(LED_ALIVE_PIN, HIGH);

    // ==========================================
    // ATTIVAZIONE ALIMENTAZIONE PRINCIPALE PCB
    // ==========================================
    // Forza alto il GPIO 11 per alimentare lo schermo e il resto della scheda
    pinMode(BUCK_5V_EN_PIN, OUTPUT);
    digitalWrite(BUCK_5V_EN_PIN, HIGH);

    // Configurazione del pin analogico per il sensore di temperatura
    pinMode(TEMP_SENS_PIN, INPUT);
}

/**
 * @brief Inizializzazione dei moduli logici dei LED RGB (FastLED).
 * @details Viene chiamata nel setup principale solo dopo che i clock e i timer 
 * di sistema dell'RP2040 si sono stabilizzati, prevenendo deadlock inter-core.
 */
void peripherals_init_leds() {
    // Inizializzazione della striscia/LED RGB WS2812B tramite FastLED
    FastLED.addLeds<WS2812B, RGB_LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(40);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}

/* --- Feedback Visivo --- */
void peripherals_set_led(uint8_t r, uint8_t g, uint8_t b) {
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
}

void peripherals_update_led_fx(LedAnimState state) {
    // Definizione colori standard
    const CRGB color_ok    = CRGB(0, 100, 50);    // Verde Menta
    const CRGB color_warn  = CRGB(150, 50, 0);    // Ambra
    const CRGB color_error = CRGB(150, 0, 0);     // Rosso

    switch (state) {
        case LED_ANIM_SEARCHING:
            // "Respiro" lento di tutti i LED (pulsazione totale)
            fill_solid(leds, NUM_LEDS, CRGB(0, 0, 50).nscale8(beatsin8(30, 50, 255)));
            break;

        case LED_ANIM_FIX_OK:
            // Effetto fisso, sobrio
            fill_solid(leds, NUM_LEDS, color_ok);
            break;

        case LED_ANIM_SIGNAL_LOW:
            // Barra di segnale: accende solo i primi 3 LED in ambra
            fill_solid(leds, NUM_LEDS, CRGB::Black);
            for(int i=0; i<3; i++) leds[i] = color_warn;
            break;

        case LED_ANIM_BOOT:
            // Animazione "Knight Rider" sobria (bianco caldo)
            static uint8_t pos = 0;
            fadeToBlackBy(leds, NUM_LEDS, 20);
            leds[pos % NUM_LEDS] = CRGB(100, 80, 50);
            pos++;
            break;
    }
    FastLED.show();
}

void peripherals_set_leds(bool enabled) {
    if (!enabled) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
    }
}

void peripherals_set_buzzer(bool enabled) {
    if (!enabled) {
        noTone(BUZZER_PIN);
    }
}

void peripherals_auto_feedback(const SystemDataPacket* data) {
    // BLOCCO LOGICO: se l'utente ha disabilitato i LED da menu, non fare nulla
    if (!global_cfg.leds_enabled) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        return;
    }

    // Altrimenti procedi con l'automatismo
    if (data->gps_status == false) {
        peripherals_update_led_fx(LED_ANIM_SEARCHING); 
    } else if (data->battery_v < 3.4f) {
        peripherals_update_led_fx(LED_ANIM_SIGNAL_LOW); 
    } else {
        peripherals_update_led_fx(LED_ANIM_FIX_OK);
    }
}

/* --- Feedback Acustico --- */
void peripherals_beep(uint32_t freq, uint32_t duration) {
    tone(BUZZER_PIN, freq, duration);
}

/* --- Logica di Feedback Combinata --- */
void peripherals_trigger_feedback(FeedbackType type) {
    switch (type) {
        case FEEDBACK_SUCCESS:
            peripherals_set_led(0, 255, 0);  // Verde
            peripherals_beep(1500, 80);
            break;

        case FEEDBACK_ERROR:
            peripherals_set_led(255, 0, 0);  // Rosso
            peripherals_beep(400, 300);
            break;

        case FEEDBACK_NEUTRAL:
            peripherals_set_led(0, 0, 255);  // Blu
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
 * @brief Restituisce la temperatura letta dal sensore analogico MCP9700.
 * @details Applica una media mobile semplice su 10 campioni per stabilizzare le variazioni spurie.
 */
float peripherals_get_temperature() {
    uint32_t raw_sum      = 0;
    const uint8_t samples = 10;

    for (uint8_t i = 0; i < samples; i++) {
        raw_sum += analogRead(TEMP_SENS_PIN);
        delayMicroseconds(50);
    }

    float raw_avg = (float)raw_sum / samples;

    // Conversione Voltaggio (Riferimento 3.3V, ADC a 12-bit nativo su RP2040)
    // Formula: V = (raw / 4095) * 3.3
    float voltage = raw_avg * (3.3f / 4095.0f);

    // Formula per integrato lineare MCP9700: (Vout - 500mV Offset) / coefficiente termico 10mV/°C
    float tempC = (voltage - 0.5f) * 100.0f;

    return tempC;
}
