/**
 * @file storage.h
 * @brief Interfaccia per la persistenza dati su SD e memoria Flash.
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

/**
 * @brief Inizializza il filesystem (SD Card / LittleFS).
 */
bool storage_init();

/**
 * @brief Scrive una riga di log CSV sulla SD.
 * @param entry Stringa formattata da salvare.
 */
void storage_log_append(const char* entry);

/**
 * @brief Salva una struttura di configurazione (Settings).
 * @param data Puntatore ai dati.
 * @param size Dimensione dei dati.
 */
void storage_save_config(void* data, size_t size);

/**
 * @brief Carica una struttura di configurazione (Settings).
 */
bool storage_load_config(void* data, size_t size);

#endif /* STORAGE_H */
