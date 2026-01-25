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


void setup(){
  //if (!init_All()) exit(1);
  //ledsInit();
  if(!displayInit()){
    while (1) ;
  }
  drawHomeScreen();
  buttonsInit();
  ledsInit();;
  ledsShowInitAnimation();
  TempInit();
  
  //displayInit();
  }


void loop(){
  buttonsUpdate();
  
  /*
  GetDate_and_Time();

  GetPosition_and_Satellites();
  
  delay(500);
  */

}
