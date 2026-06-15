/**
 * @file core0_manager.h
 * @brief Punto di ingresso e ciclo di controllo principale per il Core 0.
 * @details Il Core 0 gestisce la logica di alto livello, la UI, la gestione 
 * dei file (LittleFS) e la comunicazione seriale.
 * @defgroup Core0 Master Control
 * @{
 */

#pragma once

/**
 * @brief Setup iniziale del Core 0.
 * @details Inizializza i driver fondamentali (Display, Storage, Periferiche) 
 * e prepara lo stack di sistema prima dell'avvio del ciclo principale.
 */
void core0_setup(void);

/**
 * @brief Ciclo principale di esecuzione del Core 0.
 * @details Esegue la logica di polling della UI, l'aggiornamento della 
 * telemetria e la gestione dei task di sistema non bloccanti.
 */
void core0_loop(void);

/** @} */ // fine del gruppo Core0
