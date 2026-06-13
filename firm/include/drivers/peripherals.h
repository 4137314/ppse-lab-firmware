/**
 * @file peripherals.h
 * @brief Driver agnostico unificato per feedback (LED, Buzzer) e sensori.
 */

#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <stdbool.h>
#include <stdint.h>
#include "core/messages.h"

/**
 * @brief Stati logici di feedback per l'utente o il sistema.
 */
typedef enum {
    FEEDBACK_SUCCESS, 
    FEEDBACK_ERROR,   
    FEEDBACK_NEUTRAL, 
    FEEDBACK_OFF      
} FeedbackType;

/**
 * @brief Stati per animazione LED (FastLED).
 */
typedef enum {
    LED_ANIM_NONE,
    LED_ANIM_BOOT,
    LED_ANIM_SEARCHING,
    LED_ANIM_FIX_OK,
    LED_ANIM_SIGNAL_LOW
} LedAnimState;

/* --- Inizializzazione --- */
void peripherals_init(void);
void peripherals_init_leds(void);

/* --- Controllo Fisico Periferiche (Nuovi prototipi) --- */
/**
 * @brief Forza lo stato ON/OFF dei LED.
 * @param enabled true per abilitare, false per spegnere completamente.
 */
void peripherals_set_leds(bool enabled);

/**
 * @brief Forza lo stato ON/OFF del Buzzer.
 * @param enabled true per abilitare, false per disabilitare.
 */
void peripherals_set_buzzer(bool enabled);

/* --- Feedback Visivo e Acustico --- */
void peripherals_update_led_fx(LedAnimState state);
void peripherals_auto_feedback(const SystemDataPacket* data);
void peripherals_trigger_feedback(FeedbackType type);
void peripherals_set_led(uint8_t r, uint8_t g, uint8_t b);
void peripherals_beep(uint32_t freq_hz, uint32_t duration_ms);

/* --- Acquisizione Dati --- */
float peripherals_get_temperature(void);
float peripherals_get_battery_voltage(void);

#endif /* PERIPHERALS_H */
