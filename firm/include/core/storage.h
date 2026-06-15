/**
 * @file storage.h
 * @brief Interfaccia per la gestione della persistenza dati su file system (LittleFS).
 * @details Gestisce il salvataggio dei parametri di configurazione, il logging
 * delle telemetrie e l'archiviazione dei fix GPS.
 * @defgroup StorageManager Gestione Memoria
 * @{
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Inizializza il file system LittleFS.
 * @return true se il mount ha avuto successo, false altrimenti.
 */
bool storage_init(void);

/**
 * @brief Aggiunge una riga di log al file delle telemetrie.
 * @param entry Stringa di testo da accodare (es. riga CSV).
 */
void storage_log_append(const char* entry);

/**
 * @brief Sincronizza i dati in sospeso con il supporto fisico.
 */
void storage_sync(void);

/**
 * @brief Cerca una riga contenente uno specifico tag in un file.
 * @param filename Percorso del file.
 * @param tag Tag da cercare (es. "FIX").
 * @param buffer Buffer di destinazione.
 * @param len Lunghezza massima del buffer.
 * @return true se trovato, false altrimenti.
 */
bool storage_read_line_with_tag(const char* filename, const char* tag, char* buffer, size_t len);

/**
 * @brief Salva i parametri di configurazione in formato binario.
 * @param data Puntatore ai dati da salvare.
 * @param size Dimensione della struttura dati.
 * @return true se scrittura avvenuta con successo.
 */
bool storage_save_params(const void* data, size_t size);

/**
 * @brief Carica i parametri di configurazione dalla flash.
 * @param data Puntatore alla struct di destinazione.
 * @param size Dimensione prevista.
 * @return true se lettura avvenuta con successo.
 */
bool storage_load_params(void* data, size_t size);

/**
 * @brief Verifica se lo storage è impegnato dal protocollo USB (MTP/MSC).
 * @return true se il dispositivo è montato su PC.
 */
bool storage_is_busy_by_usb(void);

/**
 * @brief Logga una coordinata GPS nel file di log dedicato.
 * @param lat Latitudine.
 * @param lon Longitudine.
 */
void storage_log_gps_fix(float lat, float lon);

/**
 * @brief Recupera l'ultimo fix GPS salvato in memoria.
 * @param lat Puntatore dove scrivere la latitudine.
 * @param lon Puntatore dove scrivere la longitudine.
 * @return true se il fix è stato trovato e parsato.
 */
bool storage_read_last_gps(float* lat, float* lon);

/**
 * @brief Esegue un dump del file GPS sulla seriale per debug.
 */
void debug_dump_gps_log(void);

/**
 * @brief Formatta l'intera partizione Flash LittleFS.
 * @warning Questa operazione è distruttiva: cancella tutti i file salvati.
 * @return true se la formattazione e il ri-mount hanno avuto successo.
 */
bool storage_format_all(void);

/** @} */ // fine del gruppo StorageManager

#endif // STORAGE_H
