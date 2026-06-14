/**
 * @file peripherals.cpp
 * @brief Implementazione driver con logica di feedback a stati e boot sequence.
 */

#include "drivers/peripherals.h"

#include <Arduino.h>
#include <FastLED.h>

#include "core/config.h"
#include "core/messages.h"
#include "drivers/config_pins.h"

static CRGB leds[NUM_LEDS];
extern SystemConfig global_cfg;

// --- Stato interno ---
static bool boot_finished       = false;
static uint32_t boot_start_time = 0;

void peripherals_init() {
    pinMode(LED_ALIVE_PIN, OUTPUT);
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_ALIVE_PIN, HIGH);
        delay(100);
        digitalWrite(LED_ALIVE_PIN, LOW);
        delay(100);
    }
    digitalWrite(LED_ALIVE_PIN, HIGH);
    pinMode(BUCK_5V_EN_PIN, OUTPUT);
    digitalWrite(BUCK_5V_EN_PIN, HIGH);
    pinMode(TEMP_SENS_PIN, INPUT);
    boot_start_time = millis();
}

void peripherals_init_leds() {
    FastLED.addLeds<WS2812B, RGB_LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(40);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}

/* --- Controllo Fisico Periferiche --- */
void peripherals_set_leds(bool enabled) {
    if (!enabled) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
    }
    global_cfg.leds_enabled = enabled;
}

void peripherals_set_buzzer(bool enabled) {
    if (!enabled) {
        noTone(BUZZER_PIN);
    }
}

/**
 * @brief Gestore avanzato effetti LED
 */
void peripherals_update_led_fx(LedAnimState state, float val) {
    bool needs_show = true;

    // Pulisci il buffer solo se stiamo cambiando animazione 
    // o se serve per evitare il mix di colori (giallo)
    static LedAnimState last_state = (LedAnimState)-1;
    if (last_state != state) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        last_state = state;
    }

    switch (state) {
        case LED_ANIM_BOOT: {
            static uint8_t pos = 0;
            fadeToBlackBy(leds, NUM_LEDS, 20);
            leds[pos % NUM_LEDS] = CRGB(150, 150, 150);
            pos++;
            break;
        }
        case LED_ANIM_SEARCHING: {
            if ((millis() / 500) % 2 == 0)
                fill_solid(leds, NUM_LEDS, CRGB::Red);
            else
                fill_solid(leds, NUM_LEDS, CRGB::Black);
            break;
        }
        case LED_ANIM_FIX_OK: {
            float brightness_factor = constrain((float)val * 2.0f, 0.2f, 1.0f);
            uint8_t brightness = (uint8_t)(brightness_factor * 255);
            fill_solid(leds, NUM_LEDS, CRGB(0, brightness, 0)); // Solo Verde
            break;
        }
        case LED_ANIM_SIGNAL_LOW: {
            fill_solid(leds, NUM_LEDS, CRGB(150, 50, 0)); // Arancione
            break;
        }
        default:
            needs_show = false;
            break;
    }

    if (needs_show) {
        FastLED.show();
    }
}

// void peripherals_auto_feedback(const SystemDataPacket* data) {
//     if (!global_cfg.leds_enabled) {
//         fill_solid(leds, NUM_LEDS, CRGB::Black);
//         FastLED.show();
//         return;
//     }
//
//     if (!boot_finished) {
//         peripherals_update_led_fx(LED_ANIM_BOOT, 0.0f);
//         if (millis() - boot_start_time > 3000) {
//             peripherals_update_led_fx(LED_ANIM_BOOT_DONE, 0.0f);
//             delay(500);
//             boot_finished = true;
//         }
//     } else {
//         if (data->gps_status < GPS_STATUS_FIX_2D) {
//             peripherals_update_led_fx(LED_ANIM_SEARCHING, 0.0f);
//         } else if (data->battery_v < 3.4f) {
//             peripherals_update_led_fx(LED_ANIM_SIGNAL_LOW, 0.0f);
//         } else {
//             float sat_quality = constrain((float)data->satellites / 12.0f, 0.1f, 1.0f);
//             peripherals_update_led_fx(LED_ANIM_FIX_OK, sat_quality);
//         }
//     }
// }

// void peripherals_auto_feedback(const SystemDataPacket* data) {
//     if (!global_cfg.leds_enabled) {
//         fill_solid(leds, NUM_LEDS, CRGB::Black);
//         FastLED.show();
//         return;
//     }
//
//     if (!boot_finished) {
//         // Mostra solo l'animazione di boot finché il timer non scade
//         peripherals_update_led_fx(LED_ANIM_BOOT, 0.0f);
//
//         if (millis() - boot_start_time > 3000) {
//             // Rimosso: peripherals_update_led_fx(LED_ANIM_BOOT_DONE, 0.0f);
//             // Rimosso: delay(500);
//             boot_finished = true;
//         }
//     } else {
//         // Appena boot_finished diventa true, passa DIRETTAMENTE alla logica GPS
//         if (data->gps_status < GPS_STATUS_FIX_2D) {
//             peripherals_update_led_fx(LED_ANIM_SEARCHING, 0.0f);
//         } else if (data->battery_v < 3.4f) {
//             peripherals_update_led_fx(LED_ANIM_SIGNAL_LOW, 0.0f);
//         } else {
//             float sat_quality = constrain((float)data->satellites / 12.0f, 0.1f, 1.0f);
//             peripherals_update_led_fx(LED_ANIM_FIX_OK, sat_quality);
//         }
//     }
// }
 void peripherals_auto_feedback(const SystemDataPacket* data) {
    // Se i LED sono disabilitati da config, spegni tutto e esci
    if (!global_cfg.leds_enabled) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        return;
    }

    // Gestione fase di avvio
    if (!boot_finished) {
        // Mostra animazione di boot finché il timer (3000ms) non scade
        peripherals_update_led_fx(LED_ANIM_BOOT, 0.0f);
        
        if (millis() - boot_start_time > 3000) {
            boot_finished = true;
        }
    } 
    // Gestione stato operativo post-boot
    else {
        // Controlliamo direttamente il flag booleano gps_status
        // Se false (nessun fix), passiamo allo stato di ricerca
        if (data->gps_status == false) {
            peripherals_update_led_fx(LED_ANIM_SEARCHING, 0.0f);
        } 
        // Se la batteria è bassa, diamo priorità all'avviso di segnale basso
        else if (data->battery_v < 3.4f) {
            peripherals_update_led_fx(LED_ANIM_SIGNAL_LOW, 0.0f);
        } 
        // Se c'è fix e la batteria è ok, mostriamo il verde basato sui satelliti
        else {
            float sat_quality = constrain((float)data->satellites / 12.0f, 0.1f, 1.0f);
            peripherals_update_led_fx(LED_ANIM_FIX_OK, sat_quality);
        }
    }
}

/* --- Feedback Acustico --- */
void peripherals_beep(uint32_t freq, uint32_t duration) {
    tone(BUZZER_PIN, freq, duration);
}

void peripherals_trigger_feedback(FeedbackType type) {
    switch (type) {
        case FEEDBACK_SUCCESS:
            peripherals_beep(1500, 80);
            break;
        case FEEDBACK_ERROR:
            peripherals_beep(400, 300);
            break;
        default:
            break;
    }
}

void peripherals_set_led(uint8_t r, uint8_t g, uint8_t b) {
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
}

float peripherals_get_temperature() {
    uint32_t raw_sum = 0;
    for (uint8_t i = 0; i < 10; i++) {
        raw_sum += analogRead(TEMP_SENS_PIN);
        delayMicroseconds(50);
    }
    return (((raw_sum / 10.0f) * (3.3f / 4095.0f)) - 0.5f) * 100.0f;
}
