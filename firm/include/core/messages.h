/**
 * @file messages.h
 * @brief Protocolli di comunicazione inter-core (IPC) per RP2040.
 * @details Definisce le strutture dati "thread-safe" scambiate tra il Core di
 * navigazione/volo (Core 1) e il Core di interfaccia/telemetria (Core 0).
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Stati operativi del ricevitore GPS.
 */
typedef enum {
    GPS_STATUS_IDLE = 0, /**< Modulo spento o non inizializzato. */
    GPS_STATUS_NO_FIX,   /**< Modulo attivo ma nessun segnale valido. */
    GPS_STATUS_FIX_2D,   /**< Posizione orizzontale acquisita. */
    GPS_STATUS_FIX_3D    /**< Posizione e altitudine acquisite (Ottimale). */
} GpsStatus_t;

/**
 * @brief Snapshot dello stato globale del sistema.
 * @details Struttura ottimizzata per il passaggio tramite FIFO o memoria condivisa.
 * Rappresenta la telemetria completa del drone.
 */
typedef struct __attribute__((packed)) {
    // --- Navigazione & GPS ---
    double latitude;    /**< Latitudine (uso double per precisione drone). */
    double longitude;   /**< Longitudine. */
    float altitude_m;   /**< Altitudine s.l.m. (fondamentale per drone). */
    float speed_ms;     /**< Velocità in metri al secondo (unità SI). */
    uint8_t satellites; /**< Satelliti visibili. */
    uint8_t gps_status; /**< Stato del fix (cast da GpsStatus_t). */

    // --- Assetto (Impostato per drone) ---
    float pitch; /**< Inclinazione longitudinale (gradi). */
    float roll;  /**< Inclinazione trasversale (gradi). */
    float yaw;   /**< Direzione bussola (gradi). */

    // --- Salute Hardware ---
    float battery_v;   /**< Tensione batteria (Failsafe trigger). */
    float temp_c;      /**< Temperatura interna controller. */
    uint32_t uptime_s; /**< Tempo di volo/attività in secondi. */

    struct {
        uint8_t is_armed : 1;
        uint8_t is_logging : 1;
        uint8_t wifi_active : 1;
        uint8_t error_active : 1;
    } flags;

} SystemDataPacket;  // <--- Cambiato da SystemPacket_t a SystemDataPacket

#endif /* MESSAGES_H */
