// leds.h
#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Pin e numero LED
#define LED_PIN 13
#define LED_COUNT 8

extern Adafruit_NeoPixel strip;

bool ledsInit();
void ledsShowInitAnimation();

#endif
