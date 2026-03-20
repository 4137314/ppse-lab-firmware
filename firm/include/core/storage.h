#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool storage_init(void);

/** @name Logging e Scrittura */
void storage_log_append(const char* entry);
void storage_sync(void);

/** @name Lettura Generica */
/**
 * @brief Cerca una riga che inizia con una determinata stringa (tag).
 * @param filename Percorso del file.
 * @param tag Stringa da cercare all'inizio della riga (es. "WXC").
 * @param buffer Dove copiare la riga trovata.
 * @param len Dimensione del buffer.
 * @return true se la riga è stata trovata e letta.
 */
bool storage_read_line_with_tag(const char* filename, const char* tag, char* buffer, size_t len);

/** @name Gestione Parametri (Binary) */
bool storage_save_params(const void* data, size_t size);
bool storage_load_params(void* data, size_t size);

/** @name Stato */
bool storage_is_busy_by_usb(void);

#endif
