#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

static void gps_on_input(button_t btn, button_state_t state) {
    if (state != BTN_RELEASED) return;

    // Gestione ritorno alla HOME
    if (btn == BTN_BACK) {
        ui_manager_navigate_to(VIEW_ID_HOME);
    }
}

static void gps_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas    = (Adafruit_SSD1306*)get_display_driver();

    if (!canvas) return;

    // --- INTESTAZIONE ---
    canvas->setCursor(25, 0);
    canvas->setTextSize(1);
    canvas->println(F("GPS TELEMETRY"));
    canvas->drawFastHLine(0, 9, 128, SSD1306_WHITE);

    // --- STATO FIX E SATELLITI ---
    canvas->setCursor(0, 12);
    canvas->printf("FIX: %s", pkt->gps_status ? "YES" : "NO");
    canvas->setCursor(75, 12);
    canvas->printf("SATS: %02d", pkt->satellites);

    // --- COORDINATE ---
    canvas->setCursor(0, 24);
    canvas->print(F("LAT: "));
    canvas->printf("%.6f", pkt->latitude);

    canvas->setCursor(0, 34);
    canvas->print(F("LON: "));
    canvas->printf("%.6f", pkt->longitude);

    // --- VELOCITÀ ---
    canvas->setCursor(0, 44);
    float speed_kmh = pkt->speed_ms * 3.6f;
    canvas->printf("SPD: %.1f km/h", speed_kmh);

    // --- FOOTER ---
    canvas->drawFastHLine(0, 54, 128, SSD1306_WHITE);
}

const view_interface_t view_gps = {
    .on_enter = NULL, 
    .on_update = gps_render, 
    .on_input = gps_on_input, 
    .on_exit = NULL
};
