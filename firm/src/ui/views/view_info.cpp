#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ui/view_definitions.h"
#include "ui/ui_manager.h"
#include "core/messages.h"       // Risolve 'SystemDataPacket'
#include "drivers/display_ssd1306.h" // Risolve 'get_display_driver' e 'display_set_brightness'
#include "drivers/inputs.h"      // Risolve 'BTN_OK', 'BTN_UP', ecc.

static void info_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    canvas->setCursor(0, 0);
    canvas->println(F("SYSTEM INFO"));
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    canvas->setCursor(0, 20);
    canvas->printf("Firmware: v2.0-Alpha\n");
    canvas->printf("Uptime: %lu s\n", pkt->uptime_s);
    canvas->printf("Core 1 Status: OK\n");
    
    // Stato dei Flag (Bitfields dalla struct)
    canvas->setCursor(0, 50);
    canvas->print(F("Storage: "));
    canvas->println(pkt->flags.is_logging ? F("LOGGING...") : F("READY"));
}

const view_interface_t view_info = {
    .on_enter = NULL,
    .on_update = info_render,
    .on_input = NULL,
    .on_exit = NULL
};
