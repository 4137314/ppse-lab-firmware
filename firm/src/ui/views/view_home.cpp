#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

// Menu ottimizzato
enum { HOME_DASH, HOME_NAV, HOME_WEATHER, HOME_CONFIG, HOME_COUNT };
static uint8_t menu_cursor = 0;
static const char* labels[] = {"DASHBOARD", "NAVIGATION", "WEATHER", "CONFIGURATION"};

static void home_input(button_t btn, button_state_t state) {
    if (state != BTN_RELEASED) return;
    if (btn == BTN_UP)    menu_cursor = (menu_cursor == 0) ? HOME_COUNT - 1 : menu_cursor - 1;
    if (btn == BTN_DOWN)  menu_cursor = (menu_cursor + 1) % HOME_COUNT;
    if (btn == BTN_OK) {
        view_id_t targets[] = {VIEW_ID_GPS, VIEW_ID_GPS, VIEW_ID_METEO, VIEW_ID_SETTINGS}; // GPS e Dash puntano alla stessa vista
        ui_manager_navigate_to(targets[menu_cursor]);
    }
}

static void home_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    
    canvas->clearDisplay();
    canvas->setTextSize(1);
    canvas->setTextColor(SSD1306_WHITE);

    // Titolo discreto in alto a sinistra
    canvas->setCursor(0, 0);
    canvas->print(F("SYSTEM::ROOT"));
    canvas->drawFastHLine(0, 8, 128, SSD1306_WHITE);

    // Menu Centrale
    for (uint8_t i = 0; i < HOME_COUNT; i++) {
        uint8_t y = 14 + (i * 10);
        if (menu_cursor == i) {
            canvas->fillRect(0, y, 128, 9, SSD1306_WHITE);
            canvas->setTextColor(SSD1306_BLACK);
            canvas->setCursor(2, y);
            canvas->print(labels[i]);
            canvas->setTextColor(SSD1306_WHITE);
        } else {
            canvas->setCursor(2, y);
            canvas->print(labels[i]);
        }
    }

    // Footer Retro: Dati Vitali (Senza etichette superflue)
    canvas->drawFastHLine(0, 54, 128, SSD1306_WHITE);
    canvas->setCursor(0, 56);
    // Formato compatto: Temp | Volt | Status
    canvas->printf("%dC|%.1fV|%s", (int)pkt->temp_c, pkt->battery_v, pkt->gps_status ? "OK" : "NO_FIX");

    canvas->display();
}

const view_interface_t view_home = {
    .on_enter = NULL, .on_update = home_render, .on_input = home_input, .on_exit = NULL
};
