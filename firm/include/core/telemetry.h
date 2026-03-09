/**
 * @file telemetry.h
 * @brief Gestione dell'acquisizione dati e parsing telemetria.
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "messages.h"

/**
 * @brief Inizializza il modulo telemetria e il parser GPS.
 */
void telemetry_init();

/**
 * @brief Task ciclico di acquisizione (da eseguire sul Core 1).
 * @details Legge i dati dalla seriale, esegue il parsing e aggiorna il pacchetto locale.
 */
void telemetry_update();

/**
 * @brief Restituisce l'ultimo pacchetto dati generato.
 */
SystemDataPacket telemetry_get_current_frame();

#endif /* TELEMETRY_H */
