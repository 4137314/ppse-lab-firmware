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

extern volatile bool up_press;
extern volatile bool down_press;
extern volatile bool left_press;
extern volatile bool right_press;


long long debounce_callback(alarm_id_t id, void *user_data);
void gpio_callback(uint gpio, uint32_t events);

void buttonsInit();
void buttonsUpdate();   // da chiamare nel loop



#endif
