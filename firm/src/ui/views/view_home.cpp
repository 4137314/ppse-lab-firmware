#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ui/view_definitions.h"
#include "ui/ui_manager.h"
#include "core/messages.h"       // Risolve 'SystemDataPacket'
#include "drivers/display_ssd1306.h" // Risolve 'get_display_driver' e 'display_set_brightness'
#include "drivers/inputs.h"      // Risolve 'BTN_OK', 'BTN_UP', ecc.

// Cambiamo la firma per matchare l'interfaccia (usa const void* data)
static void home_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    canvas->setTextSize(1);
    canvas->setTextColor(SSD1306_WHITE);
    canvas->setCursor(0, 0);
    canvas->println(F("--- MAIN DASHBOARD ---"));
    // Nota: verifica se il campo è .temp_c o .temperature in messages.h
    canvas->printf("Temp: %.1f C\n", pkt->temp_c); 
    canvas->printf("Uptime: %lu s\n", pkt->uptime_s);
    canvas->println(F("\n[OK] per Menu GPS"));
}

static void home_input(uint8_t btn) {
    if (btn == BTN_OK) {
        // Usa ui_manager_navigate_to e l'ID corretto con _ID_
        ui_manager_navigate_to(VIEW_ID_GPS); 
    }
}

// Questa è l'istanza che il manager vede tramite 'extern'
const view_interface_t view_home = {
    .on_enter = NULL,
    .on_update = home_render,
    .on_input = home_input,
    .on_exit = NULL
};
