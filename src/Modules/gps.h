#ifndef GPS_H
#define GPS_H

//NEEDED for non overwriting of uart1 on GPIO 4 and 5
#define UART0_TX_PIN 16
#define UART0_RX_PIN 17

#define MINMEA_MAX_SENTENCE_LENGTH 82 //NMEA lib max line char count
#define NMEA_SENTENCE_COUNT 10 // NMEA sentence to read for each gpsRead call

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

    bool gpsAcquire(String* nmea_message);

    bool minmea_gps_parse(String* nmea_message);

#endif
