/**
 * @file telemetry.h
 * @brief Gestore agnostico dell'acquisizione e del parsing dei dati di volo.
 * @details Centralizza i dati provenienti da GPS, sensori e stato sistema
 * trasformandoli in un frame di telemetria normalizzato.
 * Repository: https://github.com/4137314/ppse-lab-firmware
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Inizializza i motori di parsing della telemetria.
 * @details Configura i buffer circolari e resetta gli stati dei parser (NMEA/UBX).
 */
void telemetry_init(void);

/**
 * @brief Task principale di acquisizione (Core 1).
 * @details Esegue il polling delle interfacce radio, processa i byte in arrivo
 * e aggiorna lo stato interno del sistema.
 */
void telemetry_update(void);

/**
 * @brief Copia l'ultimo frame di dati validi nel buffer fornito.
 * @details Utilizza una copia atomica per evitare che il Core 0 legga dati
 * parzialmente aggiornati dal Core 1.
 * @param dest_buffer Puntatore alla struttura di destinazione.
 * @param size Dimensione della struttura di destinazione.
 * @return true se il frame è stato copiato ed è valido.
 */
bool telemetry_get_frame(void* dest_buffer, size_t size);

/**
 * @brief Restituisce il tempo trascorso dall'ultimo fix GPS valido.
 * @return uint32_t Millisecondi dall'ultimo aggiornamento posizione.
 */
uint32_t telemetry_get_last_fix_age(void);

/**
 * @brief Verifica l'integrità del flusso dati.
 * @return true se i dati sono aggiornati e coerenti.
 */
bool telemetry_is_healthy(void);

#endif /* TELEMETRY_H */
