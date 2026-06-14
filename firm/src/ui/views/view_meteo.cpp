#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "ui/ui_manager.h"        // Fondamentale per ui_manager_navigate_to
#include "ui/view_definitions.h"  // Fondamentale per VIEW_ID_HOME

// 1. Definiamo le funzioni prima di usarle nella struct
static void meteo_render(const void* data) {
    const SystemDataPacket* sys = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    if (!canvas || !sys) return;

    canvas->setTextSize(1);
    canvas->setCursor(0, 0);

    if (sys->weather.valid) {
        canvas->printf("CITY: %s", sys->weather.city);
        canvas->drawFastHLine(0, 9, 128, SSD1306_WHITE);

        canvas->setTextSize(3);
        canvas->setCursor(5, 18);
        canvas->printf("%.1f", sys->weather.temp_ext);
        canvas->setTextSize(1);
        canvas->print(" C");

        canvas->setCursor(85, 18);
        canvas->printf("W:%.0f", sys->weather.wind_speed);
        canvas->setCursor(85, 30);
        canvas->printf("H:%d%%", sys->weather.humidity);

        canvas->setCursor(5, 42);
        if (sys->weather.weather_code == 0) canvas->print(F("Clear Sky"));
        else if (sys->weather.weather_code <= 3) canvas->print(F("Partly Cloudy"));
        else if (sys->weather.weather_code >= 60) canvas->print(F("Rainy"));
        else canvas->print(F("Check Outside"));
    } else {
        canvas->print(F("CITY: UNKNOWN"));
        canvas->drawFastHLine(0, 9, 128, SSD1306_WHITE);
        canvas->setCursor(20, 25);
        canvas->println(F("NO SYNC DATA"));
        canvas->setCursor(10, 38);
        canvas->println(F("Run make sync-meteo"));
    }

    canvas->drawFastHLine(0, 54, 128, SSD1306_WHITE);
    canvas->setCursor(0, 56);
    canvas->print(F("Sync via Serial/PC"));
}

static void meteo_on_input(button_t btn, button_state_t state) {
    if (state == BTN_RELEASED && btn == BTN_BACK) {
        ui_manager_navigate_to(VIEW_ID_HOME);
    }
}

// 2. Dichiarazione UNICA della struct (ora che le funzioni sopra esistono)
const view_interface_t view_meteo = {
    .on_enter = NULL, 
    .on_update = meteo_render, 
    .on_input = meteo_on_input, 
    .on_exit = NULL
};
