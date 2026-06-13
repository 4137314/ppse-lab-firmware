/**
 * @file ui_manager.cpp
 */

#include "ui/ui_manager.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/view_definitions.h"

static const view_interface_t* current_view = &view_home;
static view_id_t current_view_id = VIEW_ID_HOME;

// --- Inizializzazione UI ---
void ui_manager_init(void) {
    if (current_view && current_view->on_enter) {
        current_view->on_enter();
    }
}

// --- LOGICA OVERLAY ERRORE ---
static void render_error_overlay(const SystemDataPacket* data, Adafruit_SSD1306* canvas) {
    // Se non c'è un errore attivo, non facciamo nulla
    if (!data->flags.error_active) return;

    // Disegna un rettangolo nero per oscurare il contenuto sottostante
    canvas->fillRect(0, 0, 128, 64, SSD1306_BLACK);
    canvas->setTextColor(SSD1306_WHITE);
    
    canvas->setCursor(0, 0);
    canvas->print("CRITICAL ERROR");
    canvas->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    canvas->setCursor(0, 20);
    canvas->printf("CAT: 0x%02X", data->last_error.category);
    canvas->setCursor(0, 30);
    canvas->printf("CODE: 0x%02X", data->last_error.code);
    
    if (data->last_error.is_critical) {
        canvas->setCursor(0, 50);
        canvas->print("SYSTEM HALTED");
    }
}

void ui_manager_update(const void* system_data) {
    const SystemDataPacket* data = (const SystemDataPacket*)system_data;
    
    display_clear();
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    
    if (canvas) {
        canvas->setTextColor(SSD1306_WHITE);
        canvas->setTextSize(1);
    }

    // 1. Logica Prioritaria: Se c'è un errore, mostriamo l'overlay e blocchiamo la view
    if (data->flags.error_active) {
        render_error_overlay(data, canvas);
    } 
    // 2. Altrimenti, rendiamo la view normale
    else if (current_view && current_view->on_update) {
        current_view->on_update(data);
    }
    
    display_show();
}

/**
 * @brief Smista l'input alla view attiva.
 */
void ui_manager_dispatch_input(uint8_t btn_raw) {
    button_t btn = (button_t)btn_raw;

    if (btn == BTN_NONE || !current_view)
        return;

    if (btn == BTN_BACK && current_view_id != VIEW_ID_HOME) {
        ui_manager_navigate_to(VIEW_ID_HOME);
        return;
    }

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
