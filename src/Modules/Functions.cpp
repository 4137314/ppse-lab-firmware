#include "Functions.h"

bool init_All(){
    bool status;

//////////////// Serial over USB //////////////////
    Serial.begin(9600);

    Serial.println("<<<< START of hardware initialization >>>>");
    // Wait for USB serial to be up
    //while (!Serial) delay(10); 
    Serial.println("OK!");

//////////////// TEMP SENSOR /////////////////////    
    Serial.print("Initializing temperture sensor...");
    TempInit();

//////////////// OnBoard LEDs ////////////////////
    //ledsInit();

//////////////// BUTTONS /////////////////////////  
    //buttonsInit();

///////////////// BUZZER /////////////////////////
    //buzzerInit(BUZZER);

//////////////// OLED DISPLAY ////////////////////
    //displayInit();

//////////////// GPS MODULE //////////////////////
    status = gpsInit();

    Serial.println("<<<< END of hardware initialization >>>>");
    return status;
}