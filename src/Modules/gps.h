#ifndef GPS_H
#define GPS_H

//NEEDED for non overwriting of uart1 on GPIO 4 and 5
#define UART0_TX_PIN 16
#define UART0_RX_PIN 17

#define MINMEA_MAX_SENTENCE_LENGTH 82 //NMEA lib max line char count
#define NMEA_SENTENCE_COUNT 5 // NMEA sentence to read for each gpsRead call

#define GPS_EN_PIN 18   // Enable for the GPS module (active low)
//UART1 for GPS comm
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5
#define GPSBAUD 9600 //GPS module baud rate
#define LED_ALIVE 20    // Led alive GPIO pin

#include <Arduino.h>
#include "minmea/minmea.h"

    bool gpsInit();

    bool gpsRead(String* nmea_message);

    bool minmea_gps_parse(String* nmea_message);

#endif
