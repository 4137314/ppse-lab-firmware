//RELAZIONE 1 15-30pg, descrivere circuiti piú che 
// componenti. Anno mese giorno come data
#define DEBUG 1 // OFF=0 ON=1

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

  gpsRead(NMEA_message);

  minmea_gps_parse(NMEA_message);

  delay(5000);
}
