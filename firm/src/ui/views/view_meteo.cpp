#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

static void meteo_render(const void* data) {
    const SystemDataPacket* sys = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    if (!canvas || !sys) return;

    // Pulisce il buffer prima di ogni frame per evitare artefatti
    canvas->clearDisplay();
    canvas->setTextColor(SSD1306_WHITE);

    if (sys->weather.valid) {
        // --- Header ---
        canvas->setTextSize(1);
        canvas->setCursor(0, 0);
        canvas->printf("CITY: %s", sys->weather.city);
        canvas->drawFastHLine(0, 9, 128, SSD1306_WHITE);

        // --- Temperatura (Grande, sinistra) ---
        canvas->setTextSize(2);
        canvas->setCursor(5, 18);
        canvas->printf("%.1f C", sys->weather.temp_ext);

        // --- Dati Vento e Umidità (Colonna destra, allineata) ---
        canvas->setTextSize(1);
        canvas->setCursor(80, 15);
        canvas->printf("W:%02.0f", sys->weather.wind_speed); // %02.0f evita che la scritta si sposti
        canvas->setCursor(80, 25);
        canvas->printf("H:%02d%%", sys->weather.humidity);   // %02d forza 2 cifre per l'umidità

        // --- Descrizione Meteo ---
        canvas->setCursor(5, 42);
        if (sys->weather.weather_code == 0)      canvas->print(F("Clear Sky"));
        else if (sys->weather.weather_code <= 3) canvas->print(F("Partly Cloudy"));
        else if (sys->weather.weather_code >= 60) canvas->print(F("Rainy"));
        else                                     canvas->print(F("Condition check"));
    } else {
        // --- Stato Errore ---
        canvas->setTextSize(1);
        canvas->setCursor(0, 0);
        canvas->print(F("CITY: UNKNOWN"));
        canvas->drawFastHLine(0, 9, 128, SSD1306_WHITE);
        canvas->setCursor(20, 25);
        canvas->println(F("NO SYNC DATA"));
        canvas->setCursor(10, 38);
        canvas->println(F("Run make sync-meteo"));
    }

    // --- Footer fisso ---
    canvas->drawFastHLine(0, 54, 128, SSD1306_WHITE);
    canvas->setCursor(0, 56);
    canvas->print(F("Sync via Serial/PC"));
    
    // IMPORTANTE: invia il buffer al display
    canvas->display();
}

static void meteo_on_input(button_t btn, button_state_t state) {
    if (state == BTN_RELEASED && btn == BTN_BACK) {
        ui_manager_navigate_to(VIEW_ID_HOME);
    }
}

const view_interface_t view_meteo = {
    .on_enter = NULL, 
    .on_update = meteo_render, 
    .on_input = meteo_on_input, 
    .on_exit = NULL
};
