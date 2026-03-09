/**
 * @file ui_manager.h
 * @brief Orchestratore dell'interfaccia utente e della navigazione.
 * @details Gestisce la macchina a stati delle View e il dispacciamento degli input.
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "core/messages.h"

/** @brief Identificativi univoci per le schermate di sistema. */
typedef enum {
    VIEW_HOME,
    VIEW_GPS,
    VIEW_METEO,
    VIEW_SETTINGS,
    VIEW_INFO
} ViewId;

/**
 * @brief Inizializza il framework grafico e imposta la View iniziale.
 */
void ui_manager_init();

/**
 * @brief Task principale della UI (da eseguire nel loop del Core 0).
 * @param data L'ultimo pacchetto dati ricevuto dal Core 1.
 */
void ui_manager_update(SystemDataPacket* data);

/**
 * @brief Inoltra l'input dell'utente alla View attiva.
 * @param btn ID del pulsante premuto.
 */
void ui_manager_handle_input(ButtonId btn);

/**
 * @brief Cambia la schermata corrente.
 * @param new_view ID della nuova View da caricare.
 */
void ui_manager_switch_to(ViewId new_view);

#endif /* UI_MANAGER_H */
