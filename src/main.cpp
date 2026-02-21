// Se la macro non viene iniettata dal Makefile/Compiler, 
// allora definiscila come 0 (Safe default)
#ifndef DEBUG
  #define DEBUG 0
#endif

// Se vuoi automatizzare il comportamento del Beep nel codice
#ifndef BUZZER_INIT_BEEP
  #define BUZZER_INIT_BEEP 1
#endif

#include "hardware/gpio.h"
#include "Modules/Functions.h"
#include "pico/stdlib.h" 
#include "pico/multicore.h" //https://picodocs.pinout.xyz/group__pico__multicore.html#details
#include "pico/mutex.h" //https://www.raspberrypi.com/documentation/pico-sdk/high_level.html#group_mutex
#include "pico/util/queue.h"


volatile bool core1_ready = false;

// Variabili per timing non bloccante
unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 1000; // Leggi i sensori ogni secondo

bool core1_separate_stack = true; // Enables 8KB stack for each core (instead of 4KB each)
queue_t Qctrl_0_to_1, Qctrl_1_to_0;
queue_t Qdata;

void setup1(){ //CORE0 setup
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
  queue_init(&Qctrl_1_to_0, sizeof(uint8_t), 2);
   queue_init(&Qctrl_0_to_1, sizeof(uint8_t), 2);
  Serial.println("Initializing the Filesystem");
  if (!FatFS_Setup()) {
    Serial.println("ERROR: Filesystem setup failed");
    exit(EXIT_FAILURE);
  } 
    

  Serial.println("Initializing the USB Flash Translation Layer");
  if (!FatFSUSB_Setup()) exit(EXIT_FAILURE);

  if (!gpsInit()) exit(EXIT_FAILURE);

}



void setup(){ //CORE 1 setup



  buttonsInit();
  if(!displayInit()){
    while (1) delay(10);
  }
  drawHomeScreen();
  ledsInit();;
  ledsShowInitAnimation();
  TempInit();
  core1_ready = true;

 
}



void loop1(){ //CORE 0 main
  if (!core1_ready) { delay(1); return; }
  static struct parsed_nmea ParsedNMEA = {0};
  bool ret_code = true;

 if(driveConnected){
  delay(10); //if flash mounted and file is not written loop
  Serial.println("Drive connected, waiting for file write to finish...");
  return;
} 
  
  if(!sync_files()) exit(EXIT_FAILURE); // sync FileSystem registry

  check_GPS_sync_req(&Qctrl_1_to_0, &Qctrl_0_to_1, &ParsedNMEA); // check if core 1 has requested a gps sync

  if (GPS_sync(&ParsedNMEA, false)) exit(EXIT_FAILURE); // Auto sync after tot time
  
  delay(10);
}



void loop(){ // CORE 1 main
  //static uint8_t req = GPS_SYNC_REQ;
  buttonsUpdate();
  //GPS_poolOnce(0);
  //printGpsStatus();
  //GPSScreen_Tick();
  //GPS_sendToPC_periodic();

  
  
  
  /*
  GetDate_and_Time();

  GetPosition_and_Satellites();
  
  delay(500);
  */

  


  //if required GPS sync with menu entry, must call:
  // If queue is full, we just skip (prevents hanging)
  // if (queue_try_add(&queue_CTRL, &req));
}
