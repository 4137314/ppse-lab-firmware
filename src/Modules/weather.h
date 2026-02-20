#ifndef WEATHER_H
#define WEATHER_H
#pragma once
#include <Arduino.h>

struct WeatherState {
    bool curValid  = false;
    bool fcValid   = false;
    char city[24] = "NA";

    float curTempC = NAN;
    float curWindKmh = NAN;
    int curHumidity = -1;
    int curWcode = -1;

    //forecast 7 giorni 24 ore
    float tempC[7][24];
    uint8_t wcode[7][24];

    uint8_t weekday0 = 0; //oggi
    uint8_t recvMask = 0; //bit i=1 se ricevuto giorno i

    uint32_t lastUpdateMs = 0; //epoch last update time
};

extern WeatherState wx;

void Weather_Init();
void Weather_HandleLine(const String& line);

// Ui
const char* Weather_WeekdayName(uint8_t weekday);
const char* Weather_CodeToShortText(int code);



#endif
