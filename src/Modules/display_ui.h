#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H


#define DISPLAY_SDA_I2C_PIN 0
#define DISPLAY_SCL_I2C_PIN 1
#define SCREEN_WIDTH 128 // OLED display width in pixels
#define SCREEN_HEIGHT 64 // OLED display height in pixels
#define OLED_RESET -1 // reset pin to unset (not available)
#define SCREEN_ADDRESS 0x3C //screen address for Adafruit library

#include "buttons.h"  // Per SW_UP, SW_DOWN, etc.
#include "temp.h"   // Per lettura temperatura
#include "gps.h"
#include  "leds.h"

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h> // use uart as comm protocol


    // Menu
    extern const char* menuItems[];
    extern const int menuLength;

    // Timeout schermo
    extern unsigned long lastActivity;
    extern const unsigned long timeout;

    // Funzioni principali
    bool displayInit();
    void drawHomeScreen();
    void drawMenu(int index);
    void drawSelected(const char* title);

    void drawSettingsScreen();
    void drawSensorsScreen();
    void drawSystemScreen();
    void drawInfoScreen();
    void drawGPSScreen();

    // Aggiorna timer attività e gestisce timeout
    void updateDisplayTimeout();

#endif
