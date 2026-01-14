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
#include <time.h>       
#include <sys/time.h>   // Needed for settimeofday
#include "minmea/minmea.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

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

    void EmptyGpsBuffer();

    bool GetDate_and_Time();

    bool GetPosition_and_Satellites();

#endif
