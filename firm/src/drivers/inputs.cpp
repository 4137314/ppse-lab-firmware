#include "drivers/inputs.h"
#include <Arduino.h>
#include "drivers/config_pins.h"

static volatile ButtonId pending_button = BTN_NONE;
static volatile uint32_t last_interrupt_time = 0;
const uint32_t DEBOUNCE_DELAY_MS = 200;

void handle_button_interrupt() {
    uint32_t current_time = millis();
    if (current_time - last_interrupt_time > DEBOUNCE_DELAY_MS) {
        if (digitalRead(BTN_UP_PIN) == LOW)   pending_button = BTN_UP;
        else if (digitalRead(BTN_DOWN_PIN) == LOW) pending_button = BTN_DOWN;
        else if (digitalRead(BTN_OK_PIN) == LOW)   pending_button = BTN_OK;
        else if (digitalRead(BTN_BACK_PIN) == LOW) pending_button = BTN_BACK;
        
        last_interrupt_time = current_time;
    }
}

void inputs_init() {
    pinMode(BTN_UP_PIN, INPUT_PULLUP);
    pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
    pinMode(BTN_OK_PIN, INPUT_PULLUP);
    pinMode(BTN_BACK_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(BTN_UP_PIN),   handle_button_interrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_DOWN_PIN), handle_button_interrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_OK_PIN),   handle_button_interrupt, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_BACK_PIN), handle_button_interrupt, FALLING);
}

void inputs_update() { 
    // Ora gestito totalmente dagli interrupt
}

ButtonId inputs_get_last_press() {
    ButtonId event = pending_button;
    pending_button = BTN_NONE; 
    return event;
}

bool inputs_is_pressed(ButtonId btn) {
    switch (btn) {
        case BTN_UP:   return (digitalRead(BTN_UP_PIN) == LOW);
        case BTN_DOWN: return (digitalRead(BTN_DOWN_PIN) == LOW);
        case BTN_OK:   return (digitalRead(BTN_OK_PIN) == LOW);
        case BTN_BACK: return (digitalRead(BTN_BACK_PIN) == LOW);
        default: return false;
    }
}
