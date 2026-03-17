/**
 * @file storage.h
 * @brief Interfaccia agnostica per la persistenza dati (SD, Flash, LittleFS).
 * @details Gestisce il logging della telemetria e il salvataggio dei parametri
 * di configurazione del sistema in modo asincrono.
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Inizializza i supporti di memoria disponibili.
 * @details Tenta l'aggancio della SD Card (SPI) e del filesystem interno (LittleFS).
 * @return true se almeno un sistema di storage è pronto.
 */
bool storage_init(void);

/** @name Logging Telemetria (Black Box)
 * @{ */

/**
 * @brief Scrive un record di log nel file corrente.
 * @details Ottimizzato per stringhe CSV o dati binari di telemetria.
 * @param entry Puntatore alla stringa o buffer da salvare.
 */
void storage_log_append(const char* entry);

/**
 * @brief Forza la scrittura dei dati dal buffer al supporto fisico (Flush).
 * @note Da chiamare periodicamente o dopo eventi critici.
 */
void storage_sync(void);

/** @} */

/** @name Gestione Parametri e Configurazione
 * @{ */

/**
 * @brief Salva la struttura dei parametri nella memoria non volatile (NVM).
 * @param data Puntatore alla struttura di configurazione.
 * @param size Dimensione della struttura.
 * @return true se il salvataggio è andato a buon fine.
 */
bool storage_save_params(const void* data, size_t size);

/**
 * @brief Carica i parametri dalla memoria all'avvio.
 * @param data Puntatore alla struttura di destinazione.
 * @param size Dimensione attesa.
 * @return true se i dati sono stati letti e validati correttamente.
 */
bool storage_load_params(void* data, size_t size);

/** @} */

/** @name Diagnostica
 * @{ */

/** @brief Verifica se la SD Card è fisicamente inserita e leggibile. */
bool storage_is_sd_available(void);

/** @brief Restituisce lo spazio libero nel filesystem in KB. */
uint32_t storage_get_free_space(void);

/** @} */

#endif /* STORAGE_H */
