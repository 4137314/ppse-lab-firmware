/**
 * @file ui_manager.h
 * @brief Orchestratore agnostico dell'interfaccia utente.
 * @details Gestisce la macchina a stati (FSM) delle View e il routing degli eventi.
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

/** * @brief Identificativi univoci per le schermate di sistema. 
 */
typedef enum {
    VIEW_ID_HOME = 0,
    VIEW_ID_GPS,
    VIEW_ID_METEO,
    VIEW_ID_SETTINGS,
    VIEW_ID_INFO,
    VIEW_ID_COUNT
} view_id_t;

/**
 * @brief Inizializza il sistema UI.
 * @details Prepara l'hardware del display e imposta la prima view.
 */
void ui_manager_init(void);

/**
 * @brief Task di aggiornamento (da chiamare nel loop del Core 0).
 * @param system_data Puntatore ai dati ricevuti dal Core 1.
 */
void ui_manager_update(const void* system_data);

/**
 * @brief Dispaccia un evento di input alla view attiva.
 * @param input_id Identificativo dell'input (es. da driver inputs.h).
 */
void ui_manager_dispatch_input(uint8_t input_id);

/**
 * @brief Cambia la view attiva.
 * @param new_view_id L'ID della nuova schermata.
 */
void ui_manager_navigate_to(view_id_t new_view_id);

/**
 * @brief Restituisce la view correntemente visualizzata.
 */
view_id_t ui_manager_get_current_view(void);

#endif /* UI_MANAGER_H */
