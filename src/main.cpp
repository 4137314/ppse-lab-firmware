// Se la macro non viene iniettata dal Makefile/Compiler, 
// allora definiscila come 0 (Safe default)
#ifndef DEBUG
  #define DEBUG 0
#endif

// Se vuoi automatizzare il comportamento del Beep nel codice
#ifndef BUZZER_INIT_BEEP
  #define BUZZER_INIT_BEEP 1
#endif

#include "Modules/Functions.h"
#include "pico/stdlib.h" 

// Variabili per timing non bloccante
unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 1000; // Leggi i sensori ogni secondo


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
