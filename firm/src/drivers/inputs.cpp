/**
 * @file inputs.cpp
 * @brief Implementazione della gestione input con debounce software.
 */

#include "drivers/inputs.h"

#include <Arduino.h>  // Necessario per pinMode, digitalRead, millis

#include "drivers/config_pins.h"  // Necessario per BTN_UP_PIN, ecc.

// Variabile statica per memorizzare l'ultimo tasto non ancora letto
static ButtonId last_button_event  = BTN_NONE;
static uint32_t last_debounce_time = 0;
const uint32_t DEBOUNCE_DELAY_MS   = 50;

void inputs_init() {
    // Configurazione pin con pull-up interno (attivi bassi)
    pinMode(BTN_UP_PIN, INPUT_PULLUP);
    pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
    pinMode(BTN_OK_PIN, INPUT_PULLUP);
    pinMode(BTN_BACK_PIN, INPUT_PULLUP);
}

void inputs_update() {
    // Semplice polling logic con debounce
    if ((millis() - last_debounce_time) < DEBOUNCE_DELAY_MS)
        return;

    // Controlla i pulsanti (LOW significa premuto causa INPUT_PULLUP)
    if (digitalRead(BTN_UP_PIN) == LOW) {
        last_button_event  = BTN_UP;
        last_debounce_time = millis();
    } else if (digitalRead(BTN_DOWN_PIN) == LOW) {
        last_button_event  = BTN_DOWN;
        last_debounce_time = millis();
    } else if (digitalRead(BTN_OK_PIN) == LOW) {
        last_button_event  = BTN_OK;
        last_debounce_time = millis();
    } else if (digitalRead(BTN_BACK_PIN) == LOW) {
        last_button_event  = BTN_BACK;
        last_debounce_time = millis();
    }
}

ButtonId inputs_get_last_press() {
    ButtonId event    = last_button_event;
    last_button_event = BTN_NONE;  // "Consuma" l'evento
    return event;
}

/**
 * @brief Implementazione della funzione mancante richiesta dall'header
 */
bool inputs_is_pressed(ButtonId btn) {
    switch (btn) {
        case BTN_UP:
            return (digitalRead(BTN_UP_PIN) == LOW);
        case BTN_DOWN:
            return (digitalRead(BTN_DOWN_PIN) == LOW);
        case BTN_OK:
            return (digitalRead(BTN_OK_PIN) == LOW);
        case BTN_BACK:
            return (digitalRead(BTN_BACK_PIN) == LOW);
        default:
            return false;
    }
}
