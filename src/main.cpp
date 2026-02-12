#ifndef DEBUG
  #define DEBUG 0
#endif

#include "Modules/gps.h"
#include "Modules/buzzer.h"
#include "Modules/buttons.h"
#include "Modules/display_ui.h"
#include "Modules/leds.h"
#include "Modules/temp.h"
#include "Modules/Functions.h"
#include "pico/stdlib.h" //Standard library for Pico
#include "hardware/gpio.h" // REFERENCE: https://www.raspberrypi.com/documentation/pico-sdk/hardware.html
#include "Modules/pc_link.h"
#include "Modules/weather.h"


static uint32_t lastWx = 0;
const uint32_t WX_PERIOD_MS = 30UL * 60UL * 1000UL; // 30 minutes

void setup(){
  //if (!init_All()) exit(1);
  //ledsInit();
  //if(!displayInit()){
  //  while (1) ;
  //}

  Serial.begin(115200);
  delay(1000); //wait for serial to initialize
  Serial.println("=== System Init ===");

  //PC_Link_Init();
  //Weather_Init();
  displayInit();
  drawHomeScreen();
  buttonsInit();
  ledsInit();;
  ledsShowInitAnimation();
  Serial.println("Boot complete.");
 
  if(!gpsInit()){
    Serial.println("GPS initialization failed.");
  } else {
    Serial.println("GPS initialized successfully.");
  }
}

                 
                 
void loop(){
  //PC_Link_Update();
  buttonsUpdate();
  GPS_poolOnce(250);
  printGpsStatus();
  //GPSScreen_Tick();
  //GPS_sendToPC_periodic();

  
  
  
  /*
  GetDate_and_Time();

  GetPosition_and_Satellites();
  
  delay(500);
  */

  }


