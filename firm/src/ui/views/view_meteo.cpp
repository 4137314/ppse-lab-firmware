#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ui/view_definitions.h"
#include "ui/ui_manager.h"
#include "core/messages.h"       // Risolve 'SystemDataPacket'
#include "drivers/display_ssd1306.h" // Risolve 'get_display_driver' e 'display_set_brightness'
#include "drivers/inputs.h"      // Risolve 'BTN_OK', 'BTN_UP', ecc.

static void meteo_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    canvas->setCursor(30, 0);
    canvas->println(F("ENVIRONMENT"));
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    // Visualizzazione Temperatura Grande
    canvas->setTextSize(2);
    canvas->setCursor(20, 25);
    canvas->printf("%.1f C", pkt->temp_c);
    
    // Icona termometro stilizzata
    canvas->drawRoundRect(5, 20, 10, 30, 5, SSD1306_WHITE);
    canvas->fillCircle(10, 45, 4, SSD1306_WHITE);

    canvas->setTextSize(1);
    canvas->setCursor(0, 55);
    canvas->println(F("Status: Sensor Active"));
}

const view_interface_t view_meteo = {
    .on_enter = NULL,
    .on_update = meteo_render,
    .on_input = NULL,
    .on_exit = NULL
};
