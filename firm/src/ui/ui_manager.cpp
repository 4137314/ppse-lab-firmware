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

// Stato per la gestione dell'overlay errore
static bool error_acknowledged = false;
static uint8_t last_error_code = 0; 

// --- Inizializzazione UI ---
void ui_manager_init(void) {
    if (current_view && current_view->on_enter) {
        current_view->on_enter();
    }
}

// --- LOGICA OVERLAY ERRORE ---
static void render_error_overlay(const SystemDataPacket* data, Adafruit_SSD1306* canvas) {
    // Disegna un rettangolo nero per oscurare parzialmente o totalmente il contenuto
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
    } else {
        canvas->setCursor(0, 50);
        canvas->print("Premere tasto...");
    }
}

void ui_manager_update(const void* system_data) {
    const SystemDataPacket* data = (const SystemDataPacket*)system_data;
    
    // 1. Gestione logica stato errore (nessuna modifica qui)
    if (data->last_error.code != last_error_code) {
        error_acknowledged = false;
        last_error_code = data->last_error.code;
    }

    // 2. Prepara il canvas
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    if (!canvas) return;

    // 3. Pulisci una sola volta
    canvas->clearDisplay(); 

    // 4. Disegna la vista sottostante
    if (current_view && current_view->on_update) {
        current_view->on_update(data);
    }

    // 5. Se c'è l'errore, disegnalo SOPRA senza pulire di nuovo il buffer
    if (data->flags.error_active && !error_acknowledged) {
        render_error_overlay(data, canvas);
    } 
    
    // 6. Invia tutto al display una sola volta per ciclo
    canvas->display(); 
}

/**
 * @brief Smista l'input alla view attiva e gestisce la chiusura del popup errore.
 */
void ui_manager_dispatch_input(uint8_t btn_raw) {
    button_t btn = (button_t)btn_raw;

    if (btn == BTN_NONE || !current_view)
        return;

    // Logica di chiusura errore: se c'è un overlay attivo, il tasto lo chiude
    // In questo modo non propaghiamo l'input alla vista finché l'errore è visibile
    if (last_error_code != 0 && !error_acknowledged) {
        error_acknowledged = true;
        return; // Consumiamo il tasto per chiudere il popup
    }

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
        case VIEW_ID_HOME:      current_view = &view_home;      break;
        case VIEW_ID_GPS:       current_view = &view_gps;       break;
        case VIEW_ID_METEO:     current_view = &view_meteo;     break;
        case VIEW_ID_SETTINGS:  current_view = &view_settings;  break;
        case VIEW_ID_INFO:      current_view = &view_info;      break;
        default:                current_view = &view_home;      break;
    }

    if (current_view && current_view->on_enter) {
        current_view->on_enter();
    }
}

view_id_t ui_manager_get_current_view(void) {
    return current_view_id;
}
