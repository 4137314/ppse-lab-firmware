#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <FastLED.h>        //https://github.com/FastLED/FastLED/blob/master/cookbook/getting-started/concepts.md

#define LED_PIN     13           // Data pin number
#define NUM_LEDS    8         // Number of LEDs
#define LED_TYPE    WS2812B     // Your LED strip type
#define COLOR_ORDER GRB         // Color channel order

void ledsInit();
void ledsShowInitAnimation();
void ledsNavigationAnimation();
void ledsToggleAnimation();
void ledsInactivityAnimation();
#endif
