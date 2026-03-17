/**
 * @file ui_manager.cpp
 * @brief Gestore centrale dell'interfaccia utente.
 * @details Include direttamente le views per risolvere i problemi di linking
 * riscontrati durante il build su RP2040.
 */

#include "ui/ui_manager.h"
#include "ui/view_definitions.h"
#include "core/messages.h"
#include "drivers/inputs.h"
#include "drivers/display_ssd1306.h" 
#include <Arduino.h>

// --- FIX LINKER: Inclusione diretta delle View ---
// Includendo i .cpp, le variabili 'view_home', 'view_gps' ecc. 
// diventano visibili localmente al manager risolvendo gli 'undefined reference'.
#include "views/view_home.cpp"
#include "views/view_gps.cpp"
#include "views/view_meteo.cpp"
#include "views/view_settings.cpp"
#include "views/view_info.cpp"

// Puntatore alla view attiva e stato attuale
static const view_interface_t* current_view = &view_home;
static view_id_t current_view_id = VIEW_ID_HOME;

void ui_manager_init() {
    if (current_view && current_view->on_enter) {
        current_view->on_enter();
    }
}

/**
 * @brief Aggiorna la UI corrente.
 * @param system_data Puntatore ai dati di telemetria (castato internamente nelle view).
 */
void ui_manager_update(const void* system_data) {
    display_clear();

    if (current_view && current_view->on_update) {
        // Passiamo i dati alla view attiva (es. Home o GPS)
        current_view->on_update(system_data);
    }

    display_show();
}

/**
 * @brief Smista l'input ricevuto alla view attiva.
 * @param btn_raw ID del pulsante premuto.
 */
void ui_manager_dispatch_input(uint8_t btn_raw) {
    ButtonId btn = (ButtonId)btn_raw;
    
    if (btn == BTN_NONE || !current_view) return;

    // Logica globale: il tasto BACK (SWLEFT) riporta sempre alla Home
    if (btn == BTN_BACK && current_view_id != VIEW_ID_HOME) {
        ui_manager_navigate_to(VIEW_ID_HOME);
        return;
    }

    // Se la view ha una logica di input specifica, la eseguiamo
    if (current_view->on_input) {
        current_view->on_input(btn_raw);
    }
}

/**
 * @brief Gestisce la transizione tra diverse schermate.
 * @param new_view_id ID della destinazione.
 */
void ui_manager_navigate_to(view_id_t new_view_id) {
    // Esci dalla view corrente (pulizia memoria o timer)
    if (current_view && current_view->on_exit) {
        current_view->on_exit();
    }

    current_view_id = new_view_id;

    // Switch della logica di visualizzazione
    switch (new_view_id) {
        case VIEW_ID_HOME:     current_view = &view_home;     break;
        case VIEW_ID_GPS:      current_view = &view_gps;      break;
        case VIEW_ID_METEO:    current_view = &view_meteo;    break;
        case VIEW_ID_SETTINGS: current_view = &view_settings; break;
        case VIEW_ID_INFO:     current_view = &view_info;     break;
        default:               current_view = &view_home;     break;
    }

    // Inizializza la nuova view
    if (current_view && current_view->on_enter) {
        current_view->on_enter();
    }
}

view_id_t ui_manager_get_current_view(void) {
    return current_view_id;
}
