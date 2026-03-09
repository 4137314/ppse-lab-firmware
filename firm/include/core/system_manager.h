/**
 * @file system_manager.h
 * @brief Gestore della sincronizzazione e comunicazione multicore.
 */

#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include "messages.h"

/**
 * @brief Inizializza le code di sistema e le primitive multicore.
 */
void sys_manager_init();

/**
 * @brief Invia un pacchetto dati alla coda (chiamata dal Core 1).
 * @param packet Puntatore al pacchetto da inviare.
 * @return true se l'invio ha avuto successo.
 */
bool sys_manager_send_data(SystemDataPacket* packet);

/**
 * @brief Riceve l'ultimo pacchetto dati disponibile (chiamata dal Core 0).
 * @param buffer Puntatore dove copiare il pacchetto ricevuto.
 * @return true se è stato ricevuto un nuovo pacchetto.
 */
bool sys_manager_receive_data(SystemDataPacket* buffer);

#endif /* SYSTEM_MANAGER_H */
