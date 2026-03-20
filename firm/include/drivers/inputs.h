/**
 * @file inputs.h
 * @brief Driver agnostico per la gestione degli input fisici (Pulsanti).
 * @details Gestisce il debounce e la lettura dei segnali digitali.
 */

#ifndef INPUTS_H
#define INPUTS_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Identificativi logici dei pulsanti.
 */
typedef enum {
    BTN_NONE = 0, /**< Nessun evento registrato. */
    BTN_UP,       /**< Pulsante Navigazione SU / Incremento. */
    BTN_DOWN,     /**< Pulsante Navigazione GIÙ / Decremento. */
    BTN_OK,       /**< Pulsante Selezione / Conferma / Arm motori. */
    BTN_BACK,     /**< Pulsante Indietro / Annulla / Disarm. */
    BTN_COUNT     /**< Helper per la dimensione degli array di stato. */
} ButtonId;

/** * @brief Alias per compatibilità con il framework UI
 */
typedef ButtonId button_t;

/**
 * @brief Stati possibili di un evento pulsante.
 */
typedef enum {
    BTN_PRESSED, /**< Tasto appena premuto. */
    BTN_RELEASED /**< Tasto rilasciato (trigger standard per UI). */
} button_state_t;

// --- API del Driver ---

/**
 * @brief Inizializza i GPIO per i pulsanti.
 */
void inputs_init(void);

/**
 * @brief Recupera l'ultimo evento di pressione (Logica a consumo).
 * @return ButtonId L'identificativo del pulsante rilevato.
 */
ButtonId inputs_get_last_press(void);

/**
 * @brief Task di aggiornamento periodico (Debounce).
 * Deve essere chiamato regolarmente (es. ogni 20ms).
 */
void inputs_update(void);

/**
 * @brief Verifica lo stato attuale (istantaneo) di un pulsante.
 */
bool inputs_is_pressed(ButtonId btn);

#endif /* INPUTS_H */
