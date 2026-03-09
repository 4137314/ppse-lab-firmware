/**
 * @file messages.h
 * @brief Definizione dei protocolli di messaggistica inter-core.
 * @details Contiene le strutture dati scambiate tra il Core 1 (Data) e il Core 0 (UI).
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include <Arduino.h>

/** @brief Stati del Fix GPS. */
typedef enum {
    GPS_NO_FIX,
    GPS_FIX_2D,
    GPS_FIX_3D
} GpsFixStatus;

/**
 * @brief Pacchetto dati globale di sistema.
 * @details Questa struct viene inviata dal Core 1 al Core 0 tramite FIFO.
 * Contiene l'istantanea completa di tutti i sensori e stati.
 */
typedef struct {
    // Telemetria GPS
    float        latitude;      /**< Latitudine in gradi decimali. */
    float        longitude;     /**< Longitudine in gradi decimali. */
    float        speed_kmh;     /**< Velocità in km/h. */
    uint8_t      satellites;    /**< Numero di satelliti agganciati. */
    GpsFixStatus gps_status;    /**< Stato del fix corrente. */
    
    // Dati Ambientali
    float        temperature;   /**< Temperatura dal sensore analogico. */
    
    // Stato Sistema
    uint32_t     uptime_ms;     /**< Millisecondi dall'avvio. */
    float        battery_v;     /**< Voltaggio batteria (se presente). */
    bool         sd_logging;    /**< True se il log su SD è attivo. */
} SystemDataPacket;

#endif /* MESSAGES_H */
