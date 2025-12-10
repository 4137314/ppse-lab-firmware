#ifndef GPS_H
#define GPS_H

#include <TinyGPS++.h>
#include <Arduino.h>

#define GPS_EN_PIN 18
#define GPS_PPS_PIN 19
#define LED_ALIVE  20

extern TinyGPSPlus gps;

// Forward declarations
bool gpsInit();
void gpsUpdate();
TinyGPSPlus& getGPS();
void drawGPSScreen();

#endif