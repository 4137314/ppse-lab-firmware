#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "core/storage.h"
#include "drivers/display_ssd1306.h"
#include "ui/view_definitions.h"

// Struttura locale per i dati provenienti dal PC
static struct {
    char city[24];
    float temp_ext;
    float wind_speed;
    int humidity;
    int weather_code;
    bool valid = false;
} remote_weather;

/**
 * @brief Carica i dati dal file tramite l'interfaccia agnostica di storage.
 */
static void meteo_on_enter() {
    char buffer[128];
    remote_weather.valid = false;

    // Chiediamo allo storage la riga specifica con il tag "WXC"
    if (storage_read_line_with_tag("/weather.csv", "WXC", buffer, sizeof(buffer))) {
        // La View interpreta la riga CSV secondo il protocollo definito con lo script Python
        // Formato: WXC,City,Temp,Wind,Hum,Code,Weekday
        int count = sscanf(buffer, "WXC,%[^,],%f,%f,%d,%d", remote_weather.city,
                           &remote_weather.temp_ext, &remote_weather.wind_speed,
                           &remote_weather.humidity, &remote_weather.weather_code);

        // Se abbiamo letto almeno i parametri fondamentali, consideriamo i dati validi
        if (count >= 5) {
            remote_weather.valid = true;
        }
    }
}

static void meteo_render(const void* data) {
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();

    // 1. INTESTAZIONE: Nome città o errore
    canvas->setTextSize(1);
    canvas->setCursor(0, 0);
    if (remote_weather.valid) {
        canvas->printf("CITY: %s", remote_weather.city);
    } else {
        canvas->print(F("CITY: UNKNOWN"));
    }
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    if (remote_weather.valid) {
        // 2. TEMPERATURA: Caratteri grandi al centro
        canvas->setTextSize(3);
        canvas->setCursor(10, 20);
        canvas->printf("%.1f", remote_weather.temp_ext);
        canvas->setTextSize(1);
        canvas->print(" C");

        // 3. DETTAGLI: Colonna destra per Vento e Umidità
        canvas->setCursor(88, 20);
        canvas->printf("W:%.0f", remote_weather.wind_speed);
        canvas->setCursor(88, 32);
        canvas->printf("H:%d%%", remote_weather.humidity);

        // 4. DESCRIZIONE: Basata sul codice WMO (Open-Meteo)
        canvas->setCursor(10, 44);
        if (remote_weather.weather_code == 0)
            canvas->print(F("Clear Sky"));
        else if (remote_weather.weather_code <= 3)
            canvas->print(F("Partly Cloudy"));
        else if (remote_weather.weather_code >= 60)
            canvas->print(F("Rainy Conditions"));
        else
            canvas->print(F("Check Outside"));

    } else {
        // Messaggio se il file non è presente o è corrotto
        canvas->setCursor(15, 28);
        canvas->println(F("NO SYNC DATA"));
        canvas->setCursor(15, 40);
        canvas->setTextSize(1);
        canvas->println(F("Connect USB for WX"));
    }

    // 5. FOOTER: Linea di separazione e stato
    canvas->drawFastHLine(0, 54, 128, SSD1306_WHITE);
    canvas->setCursor(0, 57);
    canvas->print(F("Sync via Python Script"));
}

const view_interface_t view_meteo = {
    .on_enter = meteo_on_enter, .on_update = meteo_render, .on_input = NULL, .on_exit = NULL};
