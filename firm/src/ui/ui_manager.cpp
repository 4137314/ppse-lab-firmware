/**
 * @file ui_manager.cpp
 */

#include "ui/ui_manager.h"

#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/view_definitions.h"

// Inclusione delle implementazioni delle viste
// Nota: Assicurati che questi .cpp includano "drivers/inputs.h" in cima!
#include "views/view_gps.cpp"
#include "views/view_home.cpp"
#include "views/view_info.cpp"
#include "views/view_meteo.cpp"
#include "views/view_settings.cpp"

static const view_interface_t* current_view = &view_home;
static view_id_t current_view_id            = VIEW_ID_HOME;

void ui_manager_init() {
    if (current_view && current_view->on_enter) {
        current_view->on_enter();
    }
}

void ui_manager_update(const void* system_data) {
    display_clear();
    if (current_view && current_view->on_update) {
        current_view->on_update(system_data);
    }
    display_show();
}

/**
 * @brief Smista l'input alla view attiva.
 * @param btn_raw ID del pulsante (proveniente dal driver hardware).
 */
void ui_manager_dispatch_input(uint8_t btn_raw) {
    // Cast dell'ID grezzo al tipo enum button_t
    button_t btn = (button_t)btn_raw;

    if (btn == BTN_NONE || !current_view)
        return;

    // Logica Globale: Pressione prolungata o tasto specifico per tornare Home
    // Se il tuo hardware non ha BTN_BACK, puoi usare una combinazione o un timeout
    if (btn == BTN_BACK && current_view_id != VIEW_ID_HOME) {
        ui_manager_navigate_to(VIEW_ID_HOME);
        return;
    }

    // DISPATCH ALLA VIEW: Passiamo sia il tasto che lo stato.
    // Se il driver hardware non passa ancora lo stato, forziamo BTN_RELEASED
    // per attivare le logiche on_input delle viste.
    if (current_view->on_input) {
        current_view->on_input(btn, BTN_RELEASED);
    }
}

void ui_manager_navigate_to(view_id_t new_view_id) {
    if (current_view && current_view->on_exit) {
        current_view->on_exit();
    }

    current_view_id = new_view_id;

    switch (new_view_id) {
        case VIEW_ID_HOME:
            current_view = &view_home;
            break;
        case VIEW_ID_GPS:
            current_view = &view_gps;
            break;
        case VIEW_ID_METEO:
            current_view = &view_meteo;
            break;
        case VIEW_ID_SETTINGS:
            current_view = &view_settings;
            break;
        case VIEW_ID_INFO:
            current_view = &view_info;
            break;
        default:
            current_view = &view_home;
            break;
    }

    if (current_view && current_view->on_enter) {
        current_view->on_enter();
    }
}

view_id_t ui_manager_get_current_view(void) {
    return current_view_id;
}
