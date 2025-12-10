#include "Functions.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
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
    bool status

//////////////// Serial over USB //////////////////
    print("Starting serial interface over USB...");
    Serial.begin(9600);
    while (!Serial) delay(10); // attende che il monitor seriale si connetta
    println("OK!");

//////////////// TEMP SENSOR /////////////////////    
    print("Initializing temperture sensor...");
    status = TempInit();
    if (!status) {
        println("ERROR: could not initialize temp sensor");
        return status;
    }
    println("OK!");

    Serial.println(F("=> Start init hardware"));

}