/**
 * @file telemetry.h
 * @brief Gestione telemetria con accesso atomico per architetture multi-core.
 * @details Fornisce un'interfaccia sicura per la lettura e scrittura dei dati di
 * sistema, garantendo l'integrità dei dati tramite meccanismi di accesso 
 * sincronizzato (mutex o lock atomici).
 * @defgroup Telemetry Telemetria
 * @{
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "core/messages.h"

/**
 * @brief Inizializza i buffer di telemetria e i meccanismi di sincronizzazione.
 */
void telemetry_init(void);

/**
 * @brief Aggiorna il buffer di telemetria interno con i dati di sistema correnti.
 * @details Dovrebbe essere chiamata periodicamente dai task di sistema per 
 * mantenere i dati aggiornati.
 */
void telemetry_update(void);

/**
 * @brief Copia il frame di telemetria corrente nella destinazione fornita.
 * @details Utilizza un meccanismo di accesso sicuro per evitare la lettura di 
 * dati parzialmente aggiornati (data tearing) su architetture multi-core.
 * @param dest Puntatore alla struttura SystemDataPacket di destinazione.
 * @return true se il frame è stato copiato correttamente.
 */
bool telemetry_get_frame(SystemDataPacket* dest);

/**
 * @brief Verifica lo stato di salute dei dati di telemetria.
 * @return true se il sistema è in grado di fornire telemetria valida.
 */
bool telemetry_is_healthy(void);

/**
 * @brief Controlla i dati GPS e registra il log se il fix è valido.
 * @param lat Latitudine rilevata.
 * @param lon Longitudine rilevata.
 */
static void check_and_log_gps(float lat, float lon);

/** @} */ // fine del gruppo Telemetry

#endif // TELEMETRY_H
