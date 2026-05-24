#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    GPS_STATUS_IDLE = 0,
    GPS_STATUS_NO_FIX,
    GPS_STATUS_FIX_2D,
    GPS_STATUS_FIX_3D
} GpsStatus_t;

// --- Struttura Meteo Integrata ---
typedef struct {
    char city[24];
    float temp_ext;
    float wind_speed;
    int humidity;
    int weather_code;
    bool valid;
} WeatherDataPacket;

typedef struct __attribute__((packed)) {
    // --- Navigazione & GPS ---
    double latitude;
    double longitude;
    float altitude_m;
    float speed_ms;
    uint8_t satellites;
    uint8_t gps_status;

    // --- Assetto ---
    float pitch;
    float roll;
    float yaw;

    // --- Salute Hardware ---
    float battery_v;
    float temp_c;
    uint32_t uptime_s;

    // --- Dati Meteo ---
    WeatherDataPacket weather;

    struct {
        uint8_t is_armed : 1;
        uint8_t is_logging : 1;
        uint8_t wifi_active : 1;
        uint8_t error_active : 1;
    } flags;

} SystemDataPacket;

#endif
