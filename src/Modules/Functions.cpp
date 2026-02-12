#include "Functions.h"

bool init_All(){
    bool status=true;

//////////////// Serial over USB //////////////////
    Serial.begin(9600);

    int i=0;
    while (!Serial && i<SERIAL_MAX_SETUP_TIME){
        delay(1);
        ++i;
    }

    if (i==1000)
    {
        Serial.println("ERROR: Serial over USB setup timed out");
        exit(EXIT_FAILURE);
    }

    Serial.println("<<<< START of hardware initialization >>>>");
    Serial.println("Serial is up!");

//////////////// TEMP SENSOR /////////////////////    
    Serial.print("Initializing temperture sensor...");
    TempInit();

//////////////// OnBoard LEDs ////////////////////
    ledsInit();

//////////////// BUTTONS /////////////////////////  
    buttonsInit();

///////////////// BUZZER /////////////////////////
    //buzzerInit(BUZZER);

//////////////// OLED DISPLAY ////////////////////
    status = status && displayInit();

//////////////// GPS MODULE //////////////////////
    status = status && gpsInit();

    Serial.println("<<<< END of hardware initialization >>>>");
    return status;
}



bool request_weather_data(){

}
