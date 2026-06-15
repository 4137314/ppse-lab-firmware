/**
 * @file messages.h
 * @brief Definizione delle strutture dati per il protocollo di comunicazione.
 * @details Questo file contiene i pacchetti di telemetria e stato del sistema.
 * Le strutture sono ottimizzate per essere trasmesse via bus seriale o IPC.
 * @defgroup ProtocolDef Protocollo Dati
 * @{
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdbool.h>
#include <stdint.h>
#include "error_codes.h" 

/**
 * @brief Enumerazione degli stati operativi del modulo GPS.
 */
typedef enum {
    GPS_STATUS_IDLE = 0,    /**< Modulo in attesa o spento. */
    GPS_STATUS_NO_FIX,      /**< Modulo attivo, ma nessun fix ottenuto. */
    GPS_STATUS_FIX_2D,      /**< Fix 2D ottenuto (lat/lon). */
    GPS_STATUS_FIX_3D       /**< Fix 3D ottenuto (lat/lon/alt). */
} GpsStatus_t;

/**
 * @brief Pacchetto contenente le informazioni meteo correnti.
 */
typedef struct {
    char city[24];          /**< Nome della città (max 23 caratteri + terminatore). */
    float temp_ext;         /**< Temperatura esterna in °C. */
    float wind_speed;       /**< Velocità del vento in m/s o km/h. */
    int humidity;           /**< Umidità relativa in %. */
    int weather_code;       /**< Codice meteo (es. OpenMeteo). */
    bool valid;             /**< Flag di validità dei dati meteo. */
} WeatherDataPacket;

/**
 * @brief Pacchetto principale di telemetria di sistema.
 * @details Struttura 'packed' per garantire un layout di memoria fisso, 
 * essenziale per la serializzazione tra sistemi eterogenei.
 */
typedef struct __attribute__((packed)) {
    // --- Navigazione & GPS ---
    double latitude;        /**< Latitudine (gradi decimali). */
    double longitude;       /**< Longitudine (gradi decimali). */
    float altitude_m;       /**< Altitudine in metri. */
    float speed_ms;         /**< Velocità rispetto al suolo in m/s. */
    uint8_t satellites;     /**< Numero di satelliti utilizzati nel fix. */
    uint8_t gps_status;     /**< Stato del fix (GpsStatus_t). */

    // --- Assetto ---
    float pitch;            /**< Angolo di beccheggio (gradi). */
    float roll;             /**< Angolo di rollio (gradi). */
    float yaw;              /**< Angolo di imbardata (gradi). */

    // --- Salute Hardware ---
    float battery_v;        /**< Tensione batteria in Volt. */
    float temp_c;           /**< Temperatura interna in °C. */
    uint32_t uptime_s;      /**< Tempo di accensione in secondi. */

    // --- Dati Meteo ---
    WeatherDataPacket weather; /**< Dati meteo associati alla posizione. */

    // --- Diagnostica ---
    error_report_t last_error; /**< Ultimo report di errore registrato. */

    /**
     * @brief Flag di stato operativo del sistema (bit-field).
     */
    struct {
        uint8_t is_armed : 1;      /**< Sistema armato. */
        uint8_t is_logging : 1;    /**< Logging su storage attivo. */
        uint8_t wifi_active : 1;   /**< Modulo Wi-Fi attivo. */
        uint8_t error_active : 1;  /**< Stato di errore critico persistente. */
    } flags;

} SystemDataPacket;

/** @} */ // fine del gruppo ProtocolDef

#endif // MESSAGES_H
