#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

// Pulsanti pin (puoi anche includere da main con extern)
#define SW_UP     22
#define SW_DOWN   23
#define SW_RIGHT  24
#define SW_LEFT   25

// Forward declarations delle funzioni che userai nel main
bool buttonsInit();
void buttonsUpdate();   // da chiamare nel loop
bool isButtonPressed(uint8_t buttonPin); // lettura diretta, opzionale

extern int menuIndex;
extern bool menuOpen;
extern bool inSubmenu;

#endif