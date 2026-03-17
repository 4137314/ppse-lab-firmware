/**
 * @file inputs.h
 * @brief Driver agnostico per la gestione degli input fisici (Pulsanti).
 * @details Gestisce il debounce e la lettura dei segnali digitali.
 * Astrae completamente l'hardware per consentire la portabilità tra diversi framework.
 * Repository: https://github.com/4137314/ppse-lab-firmware
 */

#ifndef INPUTS_H
#define INPUTS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Identificativi logici dei pulsanti.
 * @details Astrae il pin fisico in un'identità funzionale per la UI o il volo.
 */
typedef enum {
    BTN_NONE = 0,   /**< Nessun evento registrato. */
    BTN_UP,         /**< Pulsante Navigazione SU / Incremento. */
    BTN_DOWN,       /**< Pulsante Navigazione GIÙ / Decremento. */
    BTN_OK,         /**< Pulsante Selezione / Conferma / Arm motori. */
    BTN_BACK,       /**< Pulsante Indietro / Annulla / Disarm. */
    BTN_COUNT       /**< Helper per la dimensione degli array di stato. */
} ButtonId;

/**
 * @brief Inizializza i GPIO per i pulsanti.
 * @details Configura i pull-up e lo stato iniziale dei registri di debounce.
 * L'implementazione specifica (Arduino o SDK) risiede nel file .cpp.
 */
void inputs_init(void);

/**
 * @brief Recupera l'ultimo evento di pressione (Logica a consumo).
 * @details Restituisce il pulsante premuto e pulisce il buffer interno.
 * @return ButtonId L'identificativo del pulsante rilevato.
 */
ButtonId inputs_get_last_press(void);

/**
 * @brief Task di aggiornamento periodico.
 * @details Gestisce il debounce software campionando i GPIO. 
 * Deve essere chiamato con una frequenza costante (es. ogni 10-20ms).
 */
void inputs_update(void);

/**
 * @brief Verifica lo stato attuale (istantaneo) di un pulsante.
 * @param btn L'ID del pulsante da controllare.
 * @return true se il pulsante è fisicamente premuto in questo istante.
 */
bool inputs_is_pressed(ButtonId btn);

#endif /* INPUTS_H */
