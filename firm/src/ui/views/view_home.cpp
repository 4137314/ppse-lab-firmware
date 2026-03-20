#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

/**
 * @brief Gestisce l'input per la Home.
 * Usiamo BTN_RELEASED per evitare rimbalzi tra le pagine.
 */
static void home_input(button_t btn, button_state_t state) {
    if (state == BTN_RELEASED) {
        if (btn == BTN_OK) {
            ui_manager_navigate_to(VIEW_ID_GPS);
        } else if (btn == BTN_UP || btn == BTN_DOWN) {
            ui_manager_navigate_to(VIEW_ID_METEO);
        }
    }
}

/**
 * @brief Rendering della Dashboard principale.
 */
static void home_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas    = (Adafruit_SSD1306*)get_display_driver();

    // --- HEADER ---
    canvas->setTextSize(1);
    canvas->setCursor(35, 0);
    canvas->print(F("PPSE-LAB"));
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    // --- BLOCCO CENTRALE: STATUS ---
    canvas->setCursor(0, 18);
    canvas->printf("TEMP: %.1f C", pkt->temp_c);

    canvas->setCursor(0, 30);
    canvas->printf("BATT: %.2f V", pkt->battery_v);

    canvas->setCursor(0, 42);
    // Trasformiamo l'uptime in formato leggibile MM:SS
    uint32_t m = pkt->uptime_s / 60;
    uint32_t s = pkt->uptime_s % 60;
    canvas->printf("UPTIME: %02lu:%02lu", m, s);

    // --- BARRA DI NAVIGAZIONE INFERIORE ---
    canvas->drawFastHLine(0, 53, 128, SSD1306_WHITE);
    canvas->setCursor(0, 57);
    canvas->setTextSize(1);
    canvas->print(F("OK:GPS  UP/DN:METEO"));
}

const view_interface_t view_home = {
    .on_enter = NULL, .on_update = home_render, .on_input = home_input, .on_exit = NULL};
