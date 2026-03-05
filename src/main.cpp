/**
 * @name Flag di Compilazione e Debug
 * @{ */

/** * @brief Abilita/Disabilita i messaggi di log sulla Serial USB.
 * 0 = Silenzioso, 1 = Verboso. Se non definita esternamente, il default è 1 (Safe).
 */
#ifndef DEBUG
  #define DEBUG 1
#endif

/** * @brief Determina se il buzzer deve emettere un suono alla fine del setup.
 * Utile per feedback "blind" senza guardare il display.
 */
#ifndef BUZZER_INIT_BEEP
  #define BUZZER_INIT_BEEP 1
#endif
/** @} */

/**
 * @file main.cpp
 * @brief Orchestratore di sistema e gestione Multicore.
 * * Questo file implementa la logica principale divisa tra Core 0 (UI/Input)
 * e Core 1 (GPS/Storage). La comunicazione avviene tramite code FIFO
 * sicure per il multicore.
 */
#include "Modules/buttons.h"
#include "Modules/buzzer.h"
#include "Modules/gps.h"
#include "Modules/leds.h"
#include "Modules/display_ui.h"
#include "Modules/temp.h"
#include "Modules/Filesystem.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h" 
#include "pico/multicore.h" //https://picodocs.pinout.xyz/group__pico__multicore.html#details
#include "pico/mutex.h" //https://www.raspberrypi.com/documentation/pico-sdk/high_level.html#group_mutex
#include "pico/util/queue.h"

#define SERIAL_MAX_SETUP_TIME 5000

/* --- Variabili Globali e Sincronizzazione --- */

/** @brief Flag di stato per segnalare che il Core 1 ha completato l'init hardware. */
volatile bool core1_ready = false;
struct parsed_nmea ParsedNMEA = {0};

/** @brief Struttura globale contenente gli ultimi dati GPS validi. */
struct parsed_nmea gpsData;

/** @brief Forza l'allocazione di 8KB di stack per ciascun core (default 4KB). */
bool core1_separate_stack = true; // Enables 8KB stack for each core (instead of 4KB each)

/** @name Code di Comunicazione Inter-Core
 * @{ */
queue_t Qctrl_0_to_1;		/**< Coda per comandi inviati da Core 0 a Core 1. */
queue_t Qctrl_1_to_0;		/**< Coda per feedback inviati da Core 1 a Core 0. */
queue_t Qdata;			/**< Coda per il trasferimento dei pacchetti @ref parsed_nmea. */
/** @} */

/* --- Variabili di Timing e UI Flags --- */
unsigned long lastSensorRead = 0;			/**< Timestamp ultima lettura sensori. */
const unsigned long sensorInterval = 1000;		/**< Intervallo campionamento (1Hz). */
uint32_t lastUiGpsMs = 0;				/**< Ultimo aggiornamento grafico GPS. */
volatile bool gpsDirty = false;				/**< Flag: i dati GPS sono cambiati e richiedono redraw. */

/* --- CORE 1: Inizializzazione Hardware Critico --- */

/**
 * @brief Configurazione iniziale per il Core 1.
 * @details Inizializza il modulo GPS, le code di comunicazione e il 
 * Filesystem (FatFS). Se il setup fallisce, il sistema entra in EXIT_FAILURE.
 */
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

/**
 * @brief Loop principale del Core 1.
 * @details Gestisce in background:
 * 1. Sincronizzazione dei file su SD.
 * 2. Parsing dei dati GPS NMEA.
 * 3. Invio periodico (4Hz) dei dati processati al Core 0 tramite @ref Qdata.
 */
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


/* --- CORE 0: Logica UI e Input Utente --- */

/**
 * @brief Configurazione iniziale per il Core 0.
 * @details Inizializza la comunicazione seriale, i pulsanti, il display OLED,
 * i LED RGB e il sensore di temperatura.
 */
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
  ledsInit();
  ledsShowInitAnimation();
  TempInit();
 
  
  
}

/**
 * @brief Loop principale del Core 0.
 * @details Gestisce l'interazione in tempo reale:
 * 1. Aggiorna lo stato dei pulsanti (@ref buttonsUpdate).
 * 2. Svuota la coda @ref Qdata per ricevere gli ultimi dati GPS dal Core 1.
 * 3. Salva periodicamente l'ultima posizione valida.
 */
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
  bool hashUpdate = false;
  while (queue_try_remove(&Qdata, &pkt)) {
    gpsData = pkt;      // tengo l’ultimo
    updated = true;
  }
  if (hashUpdate) gpsDirty = true;
  static uint32_t lastPrint = 0;
  if (hashUpdate && Serial && millis() - lastPrint >= 1000) {
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
