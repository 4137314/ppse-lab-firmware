#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h" // REFERENCE: https://www.raspberrypi.com/documentation/pico-sdk/hardware.html
#include "display_ui.h"
#include "leds.h"
#include "gps.h"



// Pulsanti pin (puoi anche includere da main con extern)
#define SW_UP     22
#define SW_DOWN   23
#define SW_RIGHT  24
#define SW_LEFT   25


// Global variables
extern int menuIndex;
extern bool menuOpen;
extern bool inSubmenu;
extern bool ambientDisplay;

extern volatile bool up_press;
extern volatile bool down_press;
extern volatile bool left_press;
extern volatile bool right_press;



long long debounce_callback(alarm_id_t id, void *user_data);
void buttons_callback(uint gpio, uint32_t events);


extern uint32_t tLeft  ;
extern uint32_t tRight ;
extern const uint32_t COMBO_MS; // finestra per considerare "insieme"

extern uint32_t lastActivity;
extern const uint32_t DISPLAY_TIMEOUT_MS ; // 15 secondi

// led navigation animation
extern int ledsNumber;
enum KeyEvent : uint8_t { KEY_NONE, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };



void buttonsInit();
void buttonsUpdate();   // da chiamare nel loop
void onLeftRightCombo();
bool checkLeftRightCombo(KeyEvent ev);

#endif
