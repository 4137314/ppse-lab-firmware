// Se la macro non viene iniettata dal Makefile/Compiler, 
// allora definiscila come 0 (Safe default)
#ifndef DEBUG
  #define DEBUG 1
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
struct parsed_nmea ParsedNMEA = {0};
struct parsed_nmea gpsData;
// Variabili per timing non bloccante
unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 1000; // Leggi i sensori ogni secondo

bool core1_separate_stack = true; // Enables 8KB stack for each core (instead of 4KB each)
queue_t Qctrl_0_to_1, Qctrl_1_to_0;
queue_t Qdata;



uint32_t lastUiGpsMs = 0;
volatile bool gpsDirty = false;

void setup1(){ //CORE1 setup
  if (!gpsInit()) exit(EXIT_FAILURE);
  Serial.println("<<<< START of hardware initialization >>>>");
  queue_init(&Qctrl_1_to_0, sizeof(uint8_t), 2);
  queue_init(&Qctrl_0_to_1, sizeof(uint8_t), 2);
  queue_init(&Qdata, sizeof(ParsedNMEA), 6); // buffer per 15 secondi a 4Hz
  Serial.println("Initializing the Filesystem");
  
 if (!FatFS_Setup()) {
  Serial.println("ERROR: Filesystem setup failed");
  exit(EXIT_FAILURE);
}  


Serial.println("Initializing the USB Flash Translation Layer");
if (!FatFSUSB_Setup()) exit(EXIT_FAILURE);


  core1_ready = true;
}



void setup(){ //CORE 0 setup
  Serial.begin(9600);
  core1_ready = false;
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


  buttonsInit();
  if(!displayInit()){
    while (1) delay(10);
  }
  Serial.println("Display initialized successfully");
  drawHomeScreen();
  ledsInit();;
  ledsShowInitAnimation();
  TempInit();
 
  
  
}



void loop1(){ //CORE 1 main
  bool ret_code = true;
 
 
 static bool said = false;
  if (driveConnected) {
    if (!said && Serial) { Serial.println("Drive connected: pausing file writes"); said = true; }
    delay(10);
    return;
  } else {
  said = false;
  }

  if (!sync_files()) exit(EXIT_FAILURE);

  check_GPS_sync_req(&Qctrl_0_to_1, &Qctrl_1_to_0, &ParsedNMEA); // check if core 1 has requested a gps sync
  
  if (!GPS_sync(&ParsedNMEA, false)) exit(EXIT_FAILURE); // Auto sync after tot time
  //Serial.println("Core 1: GPS sync done");
  //Serial.printf("rmc valid: %d\n", ParsedNMEA.parsed_rmc.valid);
  //Serial.printf("rmc date: %02d/%02d/%02d\n", ParsedNMEA.parsed_rmc.date.day, ParsedNMEA.parsed_rmc.date.month, ParsedNMEA.parsed_rmc.date.year);
  //Serial.printf("rmc time: %02d:%02d:%02d\n", ParsedNMEA.parsed_rmc.time.hours, ParsedNMEA.parsed_rmc.time.minutes, ParsedNMEA.parsed_rmc.time.seconds);
  //Serial.printf("GPS: fix=%d q=%u sat=%u lat=%.6f lon=%.6f\n",
  //ParsedNMEA.parsed_gga.fix_quality > 0, ParsedNMEA.parsed_gga.fix_quality, ParsedNMEA.parsed_gga.satellites_tracked,
  //minmea_tocoord(&ParsedNMEA.parsed_gga.latitude), minmea_tocoord(&ParsedNMEA.parsed_gga.longitude));
  //Serial.println("gga time: " + String(ParsedNMEA.parsed_gga.time.hours) + ":" + String(ParsedNMEA.parsed_gga.time.minutes) + ":" + String(ParsedNMEA.parsed_gga.time.seconds));
  static uint32_t lastSendMs = 0;
  
  if (millis() - lastSendMs >= 250) { // 4 Hz verso UI
    lastSendMs = millis();


  queue_try_add(&Qdata, &ParsedNMEA);
}

  delay(1000);
}



void loop(){ // CORE 0 main
  if(!core1_ready) {
    Serial.println("Waiting for core 1 to be ready...");
    while (!core1_ready) delay(1);
  }
  //static uint8_t req = GPS_SYNC_REQ;
  buttonsUpdate();
  //GPS_poolOnce(0);
  //printGpsStatus();
  //GPSScreen_Tick();
  //GPS_sendToPC_periodic();
  struct parsed_nmea pkt;
  bool updated = false;
  while (queue_try_remove(&Qdata, &pkt)) {
    gpsData = pkt;      // tengo l’ultimo
    updated = true;
  }
  if (updated) gpsDirty = true;
  static uint32_t lastPrint = 0;
  if (updated && Serial && millis() - lastPrint >= 1000) {
    lastPrint = millis();
   // Serial.printf("core 0: received new GPS data from core 1\n");
   // Serial.printf("GPS: fix=%d q=%u sat=%u lat=%.6f lon=%.6f\n",
  //gpsData.parsed_gga.fix_quality > 0, gpsData.parsed_gga.fix_quality, gpsData.parsed_gga.satellites_tracked, minmea_tocoord(&gpsData.parsed_gga.latitude), minmea_tocoord(&gpsData.parsed_gga.longitude));
    //  Serial.printf("GPS: time=%02d:%02d:%02d\n",
  //gpsData.parsed_gga.time.hours, gpsData.parsed_gga.time.minutes, gpsData.parsed_gga.time.seconds);
  }
    // qui aggiorni la schermata GPS usando lastParsedNMEA
    // es: showGps(lastGps.fix_valid, lastGps.lat, lastGps.lon, lastGps.sats, lastGps.age_ms);
   
  static uint32_t lastUpdate = 0;
  if (pkt.parsed_rmc.valid == 1 && (lastUpdate == 0 || millis() - lastUpdate >= 1000000)) { // aggiorna schermata GPS ogni tot tempo, o se è la prima volta
    lastUpdate = millis();
    save_gps_last(&pkt);
    gpsDirty = false;
  }

  
  /*
  GetDate_and_Time();

  GetPosition_and_Satellites();
  
  delay(5);
  */
 
  


  //if required GPS sync with menu entry, must call:
  // If queue is full, we just skip (prevents hanging)
  // if (queue_try_add(&queue_CTRL, &req));
}
