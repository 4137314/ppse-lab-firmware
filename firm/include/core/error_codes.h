/**
 * @file error_codes.h
 * @brief Definizioni degli stati di errore e diagnostica di sistema.
 * @details Fornisce un linguaggio comune per tutti i moduli (Core 0 e Core 1)
 * per segnalare anomalie hardware o logiche.
 * * @defgroup ErrorHandling Gestione Errori
 * @{
 */

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Categorie principali di errore.
 * @details Utilizzate per classificare l'origine dell'anomalia.
 */
typedef enum {
    ERR_CAT_NONE    = 0x00, /**< Nessun errore. */
    ERR_CAT_SYS     = 0x01, /**< Errori critici di sistema (Kernel/IPC). */
    ERR_CAT_HW      = 0x02, /**< Errori driver o periferiche fisiche. */
    ERR_CAT_SENSORS = 0x03, /**< Errori acquisizione dati (GPS/IMU). */
    ERR_CAT_POWER   = 0x04, /**< Anomalie elettriche o batteria. */
    ERR_CAT_STORAGE = 0x05  /**< Errori filesystem o SD Card. */
} error_category_t;

/**
 * @brief Codici errore specifici.
 * @details La numerazione è suddivisa per categorie (es. 0x1x per System, 0x2x per HW).
 * Questo design facilita il debug logico sui bus di comunicazione IPC.
 */
typedef enum {
    SUCCESS = 0,            /**< Operazione completata correttamente. */

    // --- Sistema (0x10-0x1F) ---
    ERR_SYS_IPC_TIMEOUT     = 0x10, /**< Il Core partner non risponde. */
    ERR_SYS_WATCHDOG        = 0x11, /**< Reset causato da Watchdog. */
    ERR_SYS_OUT_OF_MEMORY   = 0x12, /**< Fallimento allocazione RAM. */

    // --- Hardware (0x20-0x2F) ---
    ERR_HW_I2C_BUS_LOCK     = 0x20, /**< Bus I2C bloccato (SDA/SCL low). */
    ERR_HW_DISPLAY_LOST     = 0x21, /**< Display SSD1306 non risponde. */
    ERR_HW_UART_OVERRUN     = 0x22, /**< Buffer seriale pieno (Perdita dati). */

    // --- Sensori (0x30-0x3F) ---
    ERR_SENS_GPS_NO_DATA    = 0x30, /**< Nessun byte ricevuto dal modulo GPS. */
    ERR_SENS_IMU_NOT_CAL    = 0x31, /**< Accelerometro non calibrato. */
    ERR_SENS_GPS_BAD_FIX    = 0x32, /**< Segnale GPS degradato (HDOP alto). */

    // --- Power (0x40-0x4F) ---
    ERR_PWR_BATT_LOW        = 0x40, /**< Batteria sotto la soglia di sicurezza. */
    ERR_PWR_BATT_CRITICAL   = 0x41, /**< Tensione critica: atterraggio immediato. */
    ERR_PWR_USB_OVERVOLT    = 0x42, /**< Sovratensione rilevata su porta USB. */

    // --- Storage (0x50-0x5F) ---
    ERR_STG_SD_NOT_FOUND    = 0x50, /**< SD Card non inserita. */
    ERR_STG_WRITE_FAIL      = 0x51, /**< Fallimento scrittura log (Card full/error). */
    ERR_STG_FS_CORRUPT      = 0x52  /**< Filesystem corrotto. */
} error_code_t;

/**
 * @brief Struttura dati per il report di un errore.
 * @details Utilizzata per inviare notifiche di errore attraverso il bus IPC.
 */
typedef struct {
    error_category_t category; /**< Categoria di appartenenza. */
    error_code_t code;         /**< Codice errore specifico. */
    uint32_t timestamp;        /**< Timestamp in millisecondi dall'avvio (millis). */
    bool is_critical;          /**< Se true, richiede intervento immediato (es. Failsafe). */
} error_report_t;

/** @} */ // fine del gruppo ErrorHandling

#endif /* ERROR_CODES_H */
