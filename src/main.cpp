#ifndef DEBUG
  #define DEBUG 0
#endif


#include "Modules/Functions.h"
#include "pico/stdlib.h" //Standard library for Pico
#include "hardware/gpio.h" // REFERENCE: https://www.raspberrypi.com/documentation/pico-sdk/hardware.html


void setup(){
  if (!init_All()) exit(1);
}


void loop(){

  GetDate_and_Time();

  GetPosition_and_Satellites();
  
  delay(500);
}
