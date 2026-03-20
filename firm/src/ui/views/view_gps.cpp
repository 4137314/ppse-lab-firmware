#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "core/storage.h"  // Per salvare le coordinate per il Python
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

static void gps_on_input(button_t btn, button_state_t state) {
    // Se premo OK e ho un fix valido, salvo la posizione per lo script Python
    if (btn == BTN_OK && state == BTN_RELEASED) {
        // Recuperiamo i dati (usando il sistema di messaggistica o puntatore)
        // Qui ipotizziamo di voler triggerare un salvataggio manuale
        // storage_save_last_gps(lat, lon);
    }
}

static void gps_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas    = (Adafruit_SSD1306*)get_display_driver();

    // --- INTESTAZIONE ---
    canvas->setCursor(25, 0);
    canvas->setTextSize(1);
    canvas->println(F("GPS TELEMETRY"));
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    // --- STATO FIX E SATELLITI ---
    canvas->setCursor(0, 15);
    canvas->printf("FIX: %s", pkt->gps_status ? "YES" : "NO");
    canvas->setCursor(75, 15);
    canvas->printf("SATS: %02d", pkt->satellites);

    // --- COORDINATE (Più grandi per leggibilità) ---
    canvas->setTextSize(1);
    canvas->setCursor(0, 28);
    canvas->print(F("LAT: "));
    canvas->printf("%.6f", pkt->latitude);

    canvas->setCursor(0, 38);
    canvas->print(F("LON: "));
    canvas->printf("%.6f", pkt->longitude);

    // --- VELOCITÀ ---
    canvas->setCursor(0, 50);
    // Convertiamo m/s in km/h per comodità (facoltativo)
    float speed_kmh = pkt->speed_ms * 3.6f;
    canvas->printf("SPD: %.1f km/h", speed_kmh);

    // --- FOOTER DINAMICO ---
    if (pkt->gps_status) {
        canvas->setCursor(85, 50);
        canvas->print(F("[OK] SAV"));  // Suggerimento: premi OK per salvare
    }
}

const view_interface_t view_gps = {
    .on_enter = NULL, .on_update = gps_render, .on_input = gps_on_input, .on_exit = NULL};
