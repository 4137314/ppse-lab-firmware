#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

// Rimuoviamo la define statica manuale.
// FW_VERSION viene passato dal Makefile tramite il flag -DFW_VERSION=\"...\"
#ifndef FW_VERSION
#define FW_VERSION "v0.0.0-unknown"
#endif

static void info_input(button_t btn, button_state_t state) {
    if (state == BTN_RELEASED) {
        // Navigazione: il tasto BACK riporta alla vista SETTINGS
        if (btn == BTN_BACK) {
            ui_manager_navigate_to(VIEW_ID_SETTINGS);
        }
    }
}

static void info_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();

    canvas->clearDisplay();
    canvas->setTextSize(1);
    canvas->setTextColor(SSD1306_WHITE);

    // 0: Header
    canvas->setCursor(0, 0);
    canvas->print(F("INFO SISTEMA"));
    canvas->drawFastHLine(0, 9, 128, SSD1306_WHITE);

    // 1: FW Version (Inniettata a compilation time)
    canvas->setCursor(0, 16);
    canvas->print(F("VERSIONE: "));
    canvas->print(FW_VERSION);

    // 2: Uptime (Reattivo: il valore viene estratto dal pacchetto pkt)
    canvas->setCursor(0, 32);
    canvas->print(F("UPTIME: "));
    canvas->printf("%02lu:%02lu:%02lu", 
        (pkt->uptime_s / 3600), 
        (pkt->uptime_s % 3600) / 60, 
        (pkt->uptime_s % 60));

    // 3: Navigazione (Footer)
    canvas->setCursor(0, 56);
    canvas->print(F("< SETTINGS"));

    canvas->display();
}

const view_interface_t view_info = {
    .on_enter = NULL, 
    .on_update = info_render, 
    .on_input = info_input, 
    .on_exit = NULL
};
