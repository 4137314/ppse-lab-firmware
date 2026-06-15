/**
 * @file ui_manager.h
 * @brief Orchestratore agnostico dell'interfaccia utente.
 * @details Gestisce la macchina a stati (FSM) delle View, il routing degli 
 * eventi di input e l'aggiornamento grafico del display.
 * @defgroup UIManager Gestione UI
 * @{
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Identificativi univoci per le schermate (view) del sistema.
 */
typedef enum {
    VIEW_ID_HOME = 0,   /**< Schermata principale (Dashboard). */
    VIEW_ID_GPS,        /**< Schermata dati di navigazione. */
    VIEW_ID_METEO,      /**< Schermata dati meteorologici. */
    VIEW_ID_SETTINGS,   /**< Schermata configurazione parametri. */
    VIEW_ID_INFO,       /**< Schermata informazioni di sistema. */
    VIEW_ID_COUNT       /**< Helper per la dimensione dello stack UI. */
} view_id_t;

/**
 * @brief Inizializza il sistema UI.
 * @details Configura il driver del display, inizializza le strutture grafiche
 * e imposta VIEW_ID_HOME come schermata iniziale.
 */
void ui_manager_init(void);

/**
 * @brief Task di aggiornamento periodico dell'interfaccia.
 * @details Da richiamare nel loop principale del Core 0. Gestisce il render 
 * dei widget basandosi sui dati di telemetria.
 * @param system_data Puntatore al pacchetto dati (SystemDataPacket) ricevuto dal Core 1.
 */
void ui_manager_update(const void* system_data);

/**
 * @brief Dispaccia un evento di input alla schermata attiva.
 * @details Instrada l'evento (es. pressione pulsante) verso la view 
 * correntemente visualizzata.
 * @param input_id Identificativo dell'input (basato su ButtonId in inputs.h).
 */
void ui_manager_dispatch_input(uint8_t input_id);

/**
 * @brief Cambia la schermata attiva (Navigazione).
 * @param new_view_id L'ID della nuova schermata da caricare.
 */
void ui_manager_navigate_to(view_id_t new_view_id);

/**
 * @brief Restituisce la schermata correntemente visualizzata.
 * @return L'ID della vista attiva.
 */
view_id_t ui_manager_get_current_view(void);

/** @} */ // fine del gruppo UIManager

#endif /* UI_MANAGER_H */
