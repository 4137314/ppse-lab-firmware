#include "Functions.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPSPlus.h>
#include <Adafruit_NeoPixel.h>
#include <stdint.h>

#include "buzzer.h"
#include "buttons.h"
#include "display_ui.h"
#include "gps_module.h"
#include "leds.h"
#include "temp.h"
#include "init.h"

initAll(){
    bool status = true;
    Serial.println("<<<< Start init hardware >>>>");
//////////////// Serial over USB //////////////////
    print("Starting serial interface over USB...");
    Serial.begin(9600);
    while (!Serial) delay(10); // attende che il monitor seriale si connetta
    println("OK!");

//////////////// TEMP SENSOR /////////////////////    
    print("Initializing temperture sensor...");
    TempInit();

   


  // Pin output
  pinMode(GPS_EN_PIN, OUTPUT);
  pinMode(TEMP_PIN, INPUT); // sensore
  pinMode(LED_ALIVE, OUTPUT);
  Serial.println(F("Pin configured"));

  // GPS
  gpsInit();

  // SENSORS 
  sensorsInit();

  // LEDS
  ledsInit();

  // OLED
  displayInit();

  buttonsInit();

  // Buzzer
  buzzerInit(BUZZER);

  Serial.println(F("=> Init completed"));
  return status;

}