//RELAZIONE 1 15-30pg, descrivere circuiti piú che 
// componenti. Anno mese giorno come data

#include "Modules/gps.h"
#include "Modules/buzzer.h"
#include "Modules/buttons.h"
#include "Modules/display_ui.h"
#include "Modules/leds.h"
#include "Modules/temp.h"
#include "Modules/Functions.h"


void setup(){
  
  bool status = init_All();

}


void loop(){
  
String NMEA_message[NMEA_SENTENCE_COUNT];

  gpsRead(NMEA_message);

  delay(5000);
}
