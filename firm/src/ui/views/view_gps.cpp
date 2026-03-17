#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ui/view_definitions.h"
#include "ui/ui_manager.h"
#include "core/messages.h"       // Risolve 'SystemDataPacket'
#include "drivers/display_ssd1306.h" // Risolve 'get_display_driver' e 'display_set_brightness'
#include "drivers/inputs.h"      // Risolve 'BTN_OK', 'BTN_UP', ecc.

static void gps_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    canvas->setCursor(0, 0);
    canvas->println(F("GPS TELEMETRY"));
    canvas->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    canvas->printf("Lat: %.6f\n", pkt->latitude);
    canvas->printf("Lon: %.6f\n", pkt->longitude);
    canvas->printf("Sats: %d  Fix: %d\n", pkt->satellites, pkt->gps_status);
    // Nota: assicurati che il campo sia .speed_ms o .speed_kmh
    canvas->printf("Spd: %.1f m/s\n", pkt->speed_ms);
}

const view_interface_t view_gps = {
    .on_enter = NULL,
    .on_update = gps_render,
    .on_input = NULL, // Aggiungi logica se serve
    .on_exit = NULL
};
