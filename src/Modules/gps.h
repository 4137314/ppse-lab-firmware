#ifndef GPS_H
#define GPS_H

//NEEDED for non overwriting of uart1 on GPIO 4 and 5
#define UART0_TX_PIN 16
#define UART0_RX_PIN 17

#define MINMEA_MAX_SENTENCE_LENGTH 82 //NMEA lib max line char count
#define GPS_ACQUIRE_MAX_TRIES 30 // NMEA sentence to read for each gpsRead call

#define GPS_PPS_PIN 19 // Pin for the pulse signal from gps
#define GPS_EN_PIN 18   // Enable for the GPS module (active low)
//UART1 for GPS comm
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5
#define GPSBAUD 9600 //GPS module baud rate
#define LED_ALIVE 20    // Led alive GPIO pin

#ifndef DEBUG
#define DEBUG 0
#endif

#include <Arduino.h>
#include "minmea/minmea.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <math.h>

   struct parsed_nmea{
        struct minmea_sentence_gbs parsed_gbs;
        struct minmea_sentence_rmc parsed_rmc;
        struct minmea_sentence_gga parsed_gga;
        struct minmea_sentence_gsa parsed_gsa;
        struct minmea_sentence_gll parsed_gll;
        struct minmea_sentence_gst parsed_gst;
        struct minmea_sentence_gsv parsed_gsv;
        struct minmea_sentence_vtg parsed_vtg;
        struct minmea_sentence_zda parsed_zda;
    };
    
    //GLOBAL VARIABLE
    extern struct parsed_nmea global_parsed_nmea;
    
    
    bool gpsInit();
    
    bool nmea_gps_parse(String* nmea_message);
    
    bool gpsAcquire(enum minmea_sentence_id sentence_type);
    
    bool GPS_getFix(float &lat, float &lon, float &alt, int &sats, int &fixQ);
    
    bool gpsGetLatLon(float* latitude, float* longitude);
    
    void EmptyGpsBuffer();
    
    bool GPS_poolOnce(int timeoutMs);
    
    void GPS_sendToPC_periodic();
    
    bool GetDate_and_Time();
    
    bool GetPosition_and_Satellites();

    void printGpsStatus();
#endif/*



#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <math.h>
#include "minmea/minmea.h"

// ---------- Hardware / config ----------
#define UART0_TX_PIN 16
#define UART0_RX_PIN 17

#define MINMEA_MAX_SENTENCE_LENGTH 82

#define GPS_PPS_PIN 19
#define GPS_EN_PIN 18

#define UART1_TX_PIN 4
#define UART1_RX_PIN 5
#define GPSBAUD 9600

#define LED_ALIVE 20

#ifndef DEBUG
#define DEBUG 0
#endif

// ---------- Parsed data container ----------
struct parsed_nmea{
    minmea_sentence_gbs parsed_gbs;
    minmea_sentence_rmc parsed_rmc;
    minmea_sentence_gga parsed_gga;
    minmea_sentence_gsa parsed_gsa;
    minmea_sentence_gll parsed_gll;
    minmea_sentence_gst parsed_gst;
    minmea_sentence_gsv parsed_gsv;
    minmea_sentence_vtg parsed_vtg;
    minmea_sentence_zda parsed_zda;
};

// Stato globale (come già fai tu)
extern parsed_nmea global_parsed_nmea;

// ---------- Public API (coerente, non bloccante) ----------
bool gpsInit();

// Da chiamare spesso nel loop(): legge Serial2, accumula righe, parse e aggiorna global_parsed_nmea
void gpsPoll();

// Non leggono Serial2: usano solo lo stato già parsato
bool gpsHasFix(uint32_t freshnessMs = 2000);
bool gpsGetLatLon(float* latitude, float* longitude);
bool GPS_getFix(float &lat, float &lon, float &alt, int &sats, int &fixQ);

void printGpsStatus();
void GPS_sendToPC_periodic();

#endif

*/