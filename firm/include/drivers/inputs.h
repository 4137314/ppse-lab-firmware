/**
 * @file inputs.h
 * @brief Driver agnostico per la gestione degli input fisici (Pulsanti).
 * @details Gestisce il debounce hardware e fornisce una mappatura logica 
 * degli eventi di pressione per l'interfaccia utente.
 * @defgroup InputDriver Gestione Input
 * @{
 */

#ifndef INPUTS_H
#define INPUTS_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Identificativi logici dei pulsanti del sistema.
 */
typedef enum {
    BTN_NONE = 0, /**< Nessun evento registrato. */
    BTN_UP,       /**< Pulsante Navigazione SU / Incremento. */
    BTN_DOWN,     /**< Pulsante Navigazione GIÙ / Decremento. */
    BTN_OK,       /**< Pulsante Selezione / Conferma / Arm motori. */
    BTN_BACK,     /**< Pulsante Indietro / Annulla / Disarm. */
    BTN_COUNT     /**< Helper per la dimensione degli array di stato. */
} ButtonId;

/** * @brief Alias per compatibilità con il framework UI.
 */
typedef ButtonId button_t;

/**
 * @brief Stati possibili di un evento pulsante.
 */
typedef enum {
    BTN_PRESSED,  /**< Tasto appena premuto (Leading edge). */
    BTN_RELEASED  /**< Tasto rilasciato (Trailing edge - trigger standard UI). */
} button_state_t;

/**
 * @brief Inizializza i GPIO e le strutture di debounce per i pulsanti.
 */
void inputs_init(void);

/**
 * @brief Recupera l'ultimo evento di pressione (Logica a consumo).
 * @details Questa funzione consuma l'evento: dopo la lettura, il buffer 
 * interno viene resettato a BTN_NONE.
 * @return ButtonId L'identificativo dell'ultimo pulsante rilevato.
 */
ButtonId inputs_get_last_press(void);

/**
 * @brief Task di aggiornamento periodico (Debounce).
 * @details Implementa la logica di campionamento temporale per eliminare 
 * i rimbalzi elettromeccanici. Deve essere richiamato con un timer 
 * costante (es. 20ms).
 */
void inputs_update(void);

/**
 * @brief Verifica lo stato fisico attuale (istantaneo) di un pulsante.
 * @param btn Identificativo del pulsante da interrogare.
 * @return true se il pulsante è attualmente mantenuto premuto.
 */
bool inputs_is_pressed(ButtonId btn);

/** @} */ // fine del gruppo InputDriver

#endif /* INPUTS_H */
