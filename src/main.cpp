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


void setup(){
  if (!init_All()) exit(1);
}


void loop(){

  GetDate_and_Time();

  GetPosition_and_Satellites();
  
  delay(500);
}
