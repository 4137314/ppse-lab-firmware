/**
 * @file ui_manager.cpp
 * @brief Implementazione della logica di navigazione e gestione View.
 */

#include "ui/ui_manager.h"
#include "ui/views/view_home.h"
#include "ui/views/view_gps.h"
#include "ui/views/view_meteo.h"
#include "ui/views/view_settings.h"
#include "ui/views/view_info.h"

static ViewId current_view = VIEW_HOME;

void ui_manager_init() {
    current_view = VIEW_HOME;
}

void ui_manager_update(SystemDataPacket* data) {
    display_clear();

    // Dispatcher del rendering basato sulla View attiva
    switch (current_view) {
        case VIEW_HOME:     view_home_render(data);     break;
        case VIEW_GPS:      view_gps_render(data);      break;
        case VIEW_METEO:    view_meteo_render(data);    break;
        case VIEW_SETTINGS: view_settings_render(data); break;
        case VIEW_INFO:     view_info_render(data);     break;
    }

    display_show();
}

void ui_manager_handle_input(ButtonId btn) {
    if (btn == BTN_NONE) return;

    // Se premiamo BACK in qualsiasi schermata (tranne Home), torniamo alla Home
    if (btn == BTN_BACK && current_view != VIEW_HOME) {
        ui_manager_switch_to(VIEW_HOME);
        return;
    }

    // Dispatcher dell'input alla View specifica
    switch (current_view) {
        case VIEW_HOME:     view_home_handle_input(btn);     break;
        case VIEW_GPS:      view_gps_handle_input(btn);      break;
        case VIEW_METEO:    view_meteo_handle_input(btn);    break;
        case VIEW_SETTINGS: view_settings_handle_input(btn); break;
        case VIEW_INFO:     view_info_handle_input(btn);     break;
    }
}

void ui_manager_switch_to(ViewId new_view) {
    current_view = new_view;
}
