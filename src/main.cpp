#ifndef DEBUG
  #define DEBUG 0
#endif


#include "Modules/Functions.h"
#include "pico/stdlib.h" //Standard library for Pico



void setup(){
  littleFS_Setup();
  if (!init_All()) exit(1);
}


void loop(){

  GPS_sync();

  buttonsUpdate();

  delay(500);
}
