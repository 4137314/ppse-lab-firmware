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

  String NMEA_message[NMEA_SENTENCE_COUNT];
  for(int i=0; i<NMEA_SENTENCE_COUNT; ++i)
  {
    if(!gpsAcquire(NMEA_message)){
      #if DEBUG == 1
      Serial.println("gpsAcquire has returned FALSE");
      #endif
    }
    if(!minmea_gps_parse(NMEA_message)){
      #if DEBUG == 1
      Serial.println("gps parse has returned FALSE");
      #endif
    }
    
  }
  delay(500);
}
