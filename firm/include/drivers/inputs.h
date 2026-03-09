/**
 * @file inputs.h
 * @brief Driver per la gestione degli input fisici (Pulsanti).
 * @details Gestisce il debounce e la lettura dei GPIO definiti in config_pins.h.
 * Fornisce un'interfaccia asincrona alla UI senza conoscere lo stato dei menu.
 */

#ifndef INPUTS_H
#define INPUTS_H

#include <Arduino.h>
#include "config_pins.h"

/**
 * @brief Identificativi logici dei pulsanti.
 * @details Astrae il numero del pin fisico in un'identità funzionale per la UI.
 */
typedef enum {
    BTN_NONE,   /**< Nessun evento registrato. */
    BTN_UP,     /**< Pulsante Navigazione SU. */
    BTN_DOWN,   /**< Pulsante Navigazione GIÙ. */
    BTN_OK,     /**< Pulsante Selezione/Conferma. */
    BTN_BACK    /**< Pulsante Indietro/Annulla. */
} ButtonId;

/**
 * @brief Inizializza i pin GPIO per i pulsanti.
 * @details Configura i pin come INPUT_PULLUP e prepara gli interrupt o 
 * la logica di polling per il debounce.
 */
void inputs_init();

/**
 * @brief Recupera l'ultimo pulsante premuto.
 * @details Implementa una logica "consumabile": una volta chiamato, l'evento 
 * viene restituito e il registro interno resettato a BTN_NONE.
 * @return ButtonId L'identificativo del pulsante premuto dall'ultima lettura.
 */
ButtonId inputs_get_last_press();

/**
 * @brief Task di aggiornamento per il debounce (se non si usano interrupt).
 * @note Da chiamare ciclicamente se l'implementazione non è basata su hardware timer.
 */
void inputs_update();

#endif /* INPUTS_H */
