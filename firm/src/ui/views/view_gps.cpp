#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "core/storage.h"  
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

// Variabile locale per tracciare se l'utente ha appena salvato (per feedback visivo)
static uint32_t saved_feedback_ms = 0;

static void gps_on_input(button_t btn, button_state_t state) {
    // Se premo OK e ho il rilascio, triggero il salvataggio sulla SD
    if (btn == BTN_OK && state == BTN_RELEASED) {
        // Il flag di feedback farà apparire una scritta di conferma a schermo per 1.5 secondi
        saved_feedback_ms = millis();
        
        Serial.println("[UI GPS] Richiesta salvataggio manuale posizione...");
        // Qui quando re-integrerai lo storage si chiamerà:
        // storage_save_last_gps(current_lat, current_lon);
    }
}

static void gps_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas    = (Adafruit_SSD1306*)get_display_driver();

    if (!canvas) return;

    // --- INTESTAZIONE (Sposta su 0) ---
    canvas->setCursor(25, 0);
    canvas->setTextSize(1);
    canvas->println(F("GPS TELEMETRY"));
    canvas->drawFastHLine(0, 9, 128, SSD1306_WHITE);

    // --- STATO FIX E SATELLITI (Sposta su 12) ---
    canvas->setCursor(0, 12);
    canvas->printf("FIX: %s", pkt->gps_status ? "YES" : "NO");
    canvas->setCursor(75, 12);
    canvas->printf("SATS: %02d", pkt->satellites);

    // --- COORDINATE (Sposta su 24 e 34) ---
    canvas->setCursor(0, 24);
    canvas->print(F("LAT: "));
    canvas->printf("%.6f", pkt->latitude);

    canvas->setCursor(0, 34);
    canvas->print(F("LON: "));
    canvas->printf("%.6f", pkt->longitude);

    // --- VELOCITÀ (Sposta su 44) ---
    canvas->setCursor(0, 44);
    float speed_kmh = pkt->speed_ms * 3.6f;
    canvas->printf("SPD: %.1f km/h", speed_kmh);

    // --- FOOTER DINAMICO (Sposta la linea su 54 e il testo su 56) ---
    canvas->drawFastHLine(0, 54, 128, SSD1306_WHITE);
    
    // Il testo ora parte da 56. 56 + 8 (altezza font) = 64. Perfetto.
    if (saved_feedback_ms > 0 && (millis() - saved_feedback_ms < 1500)) {
        canvas->setCursor(35, 56);
        canvas->print(F("* SAVED! *"));
    } else {
        if (pkt->gps_status) {
            canvas->setCursor(75, 56);
            canvas->print(F("[OK] SAVE")); 
        }
    }
}

const view_interface_t view_gps = {
    .on_enter = NULL, .on_update = gps_render, .on_input = gps_on_input, .on_exit = NULL};
