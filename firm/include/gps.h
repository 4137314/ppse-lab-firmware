/**
 * @file gps.h
 * @brief Definizioni, macro e strutture per la gestione della telemetria GPS.
 * * Questo header definisce l'interfaccia per il modulo GPS, incluse le
 * mappature dei pin UART per RP2040, le impostazioni di logging su filesystem e
 * la struttura dati che aggrega i risultati del parsing della libreria minmea.
 */

#ifndef GPS_H
#define GPS_H

#ifndef DEBUG
/** @brief Abilita o disabilita l'output di debug seriale. */
#define DEBUG 1
#endif

/** @name Mappatura Pin UART0
 * Necessaria per evitare sovrapposizioni con UART1 su GPIO 4 e 5.
 * @{ */
#define UART0_TX_PIN 16
#define UART0_RX_PIN 17
/** @} */

/** @name Parametri Protocollo NMEA
 * @{ */
#define MINMEA_MAX_SENTENCE_LENGTH 82 /**< Lunghezza massima di una riga NMEA standard. */
#define GPS_ACQUIRE_MAX_TRIES                                                   \
    30 /**< Tentativi massimi di lettura sentenze per ogni chiamata gpsAcquire. \
        */
/** @} */

/** @name Pinout Hardware GPS
 * @{ */
#define GPS_PPS_PIN  19   /**< Pin per il segnale Pulse Per Second (PPS) dal GPS. */
#define GPS_EN_PIN   18   /**< Pin di abilitazione alimentazione modulo (Attivo Basso). */
#define UART1_TX_PIN 4    /**< Pin TX per comunicazione UART con il modulo. */
#define UART1_RX_PIN 5    /**< Pin RX per comunicazione UART con il modulo. */
#define GPSBAUD      9600 /**< Baud rate predefinito del modulo GPS. */
#define LED_ALIVE    20   /**< GPIO per il LED di segnalazione attività (Heartbeat). */
/** @} */

/** @name Timing e Filesystem
 * @{ */
#define GPS_SYNC_TIMEOUT_MSEC 20              /**< Timeout per la sincronizzazione di data e ora. */
#define GPS_LOG_PATH          "/gps_log.csv"  /**< Percorso del file log rotativo (CSV). */
#define GPS_LAST_PATH         "/gps_last.csv" /**< Percorso del file con l'ultima posizione nota. */
#define GPS_MAX_LINES         20              /**< Numero massimo di righe per il log rotativo. */
#define GPS_WRITE_FREQ                                                     \
    600000 /**< Frequenza minima di scrittura su Flash (ms) per protezione \
              usura. */
/** @} */

/** @name Protocollo Inter-Core
 * @{ */
#define GPS_SYNC_REQ 0xA /**< Comando di richiesta sincronizzazione GPS. */
#define GPS_SYNC_ACK 0xB /**< Risposta di avvenuta sincronizzazione. */
/** @} */

#include <Arduino.h>
#include <hardware/gpio.h>
#include <sys/time.h>
#include <time.h>

#include "filesystem.h"
#include "minmea.h"
#include "pico/stdlib.h"

/**
 * @struct parsed_nmea
 * @brief Container globale per le sentenze NMEA parsate.
 * * Questa struttura aggrega i diversi tipi di messaggi NMEA gestiti dalla
 * libreria @c minmea. Viene aggiornata ogni volta che una nuova stringa valida
 * viene ricevuta dalla seriale.
 */
struct parsed_nmea {
    struct minmea_sentence_gbs parsed_gbs; /**< Errori satellitari. */
    struct minmea_sentence_rmc parsed_rmc; /**< Dati minimi raccomandati (Data, Ora, Posizione). */
    struct minmea_sentence_gga parsed_gga; /**< Fix di precisione e altitudine. */
    struct minmea_sentence_gsa parsed_gsa; /**< DOP e satelliti attivi. */
    struct minmea_sentence_gll parsed_gll; /**< Latitudine e Longitudine. */
    struct minmea_sentence_gst parsed_gst; /**< Statistiche rumore pseudorange. */
    struct minmea_sentence_gsv parsed_gsv; /**< Satelliti in vista. */
    struct minmea_sentence_vtg parsed_vtg; /**< Velocità e rotta reale. */
    struct minmea_sentence_zda parsed_zda; /**< Data e Ora UTC. */
};

extern uint32_t lastUiGpsMs;   /**< Timestamp dell'ultimo aggiornamento UI dei dati GPS. */
extern volatile bool gpsDirty; /**< Flag per indicare dati GPS non ancora processati. */

/* --- Prototipi Funzioni --- */

/** @brief Configura GPIO e UART per il modulo GPS. */
bool gpsInit();

/** @brief Esegue il parsing di una stringa NMEA. */
bool nmea_gps_parse(String* nmea_message, struct parsed_nmea* nmea_ptr);

/** @brief Cerca e acquisisce una specifica sentenza dalla seriale. */
bool gpsAcquire(enum minmea_sentence_id sentence_type, struct parsed_nmea* nmea_ptr);

/** @brief Estrae data e ora dai messaggi RMC. */
bool GetDate_and_Time(struct parsed_nmea* nmea_ptr);

/** @brief Estrae posizione e numero satelliti dai messaggi GGA. */
bool GetPosition_and_Satellites(struct parsed_nmea* nmea_ptr);

/** @brief Sincronizza lo stato globale e gestisce il salvataggio su disco. */
bool GPS_sync(struct parsed_nmea* nmea_ptr, bool force);

/** @brief Sovrascrive il file dell'ultimo fix noto. */
bool save_gps_last(struct parsed_nmea* nmea_ptr);

/** @brief Aggiunge una riga al log rotativo (max 20 righe). */
bool save_gps_log20(struct parsed_nmea* nmea_ptr);

/** @brief Gestisce le richieste di sync provenienti dall'altro core. */
void check_GPS_sync_req(queue_t* recv_queue, queue_t* send_queue, struct parsed_nmea* nmea_ptr);

/* --- Funzioni di Debug --- */
void print_NMEA_rmc(void* frame);
void print_NMEA_gga(void* frame);
void print_NMEA_gst(void* frame);
void print_NMEA_gsv(void* frame);
void print_NMEA_vtg(void* frame);
void print_NMEA_zda(void* frame);

#endif  // GPS_H
