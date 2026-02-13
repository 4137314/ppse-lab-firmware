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
#include "pico/multicore.h" //https://picodocs.pinout.xyz/group__pico__multicore.html#details
#include "pico/mutex.h" //https://www.raspberrypi.com/documentation/pico-sdk/high_level.html#group_mutex
#include "pico/util/queue.h"

// Variabili per timing non bloccante
unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 1000; // Leggi i sensori ogni secondo

bool core1_separate_stack = true; // Enables 8KB stack for each core (instead of 4KB each)
queue_t Qctrl_0_to_1, Qctrl_1_to_0;
queue_t Qdata;

void setup(){ //CORE0 setup
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
  Serial.println("Initializing the Filesystem");
  if (!FatFS_Setup()) exit(EXIT_FAILURE);

  Serial.println("Initializing the USB Flash Translation Layer");
  if (!FatFSUSB_Setup()) exit(EXIT_FAILURE);

  if (!gpsInit()) exit(EXIT_FAILURE);
}



void setup1(){ //CORE 1 setup
  if(!displayInit()){
    while (1) delay(10);
  }
  drawHomeScreen();
  buttonsInit();
  ledsInit();;
  ledsShowInitAnimation();
  TempInit();
  
  queue_init(&Qctrl_1_to_0, sizeof(uint8_t), 2);
  queue_init(&Qctrl_0_to_1, sizeof(uint8_t), 2);
}



void loop(){ //CORE 0 main
  struct parsed_nmea ParsedNMEA = {0};
  bool ret_code = true;

  while(driveConnected && !updated) delay(10); //if flash mounted and file is not written loop
  if(sync_files()) exit(EXIT_FAILURE); // sync FileSystem registry

  check_GPS_sync_req(&Qctrl_1_to_0, &Qctrl_0_to_1, &ParsedNMEA); // check if core 1 has requested a gps sync

  if (GPS_sync(&ParsedNMEA, false)) exit(EXIT_FAILURE); // Auto sync after tot time
  
  delay(10);
}



void loop1(){ // CORE 1 main
  static uint8_t req = GPS_SYNC_REQ;
  buttonsUpdate();
  //displayInit();

  //if required GPS sync with menu entry, must call:
  // If queue is full, we just skip (prevents hanging)
  // if (queue_try_add(&queue_CTRL, &req));
}
