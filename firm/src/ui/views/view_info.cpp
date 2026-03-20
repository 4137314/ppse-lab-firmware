#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

/**
 * @brief Gestione input per la schermata Info.
 */
static void info_input(button_t btn, button_state_t state) {
    if (state == BTN_RELEASED) {
        // Qualsiasi tasto o OK riporta alla Home o alla Dashboard
        if (btn == BTN_OK || btn == BTN_DOWN) {
            ui_manager_navigate_to(VIEW_ID_HOME);
        }
    }
}

/**
 * @brief Rendering delle informazioni di sistema e diagnostica.
 */
static void info_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas    = (Adafruit_SSD1306*)get_display_driver();

    // --- HEADER ---
    canvas->setTextSize(1);
    canvas->setCursor(30, 0);
    canvas->print(F("SYSTEM DIAG"));
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    // --- INFO FIRMWARE & HARDWARE ---
    canvas->setCursor(0, 15);
    canvas->print(F("FW: v2.1-Agno"));
    canvas->setCursor(85, 15);
    canvas->print(F("RP2040"));

    // --- STATO REAL-TIME ---
    canvas->setCursor(0, 28);
    canvas->printf("Uptime: %02lu:%02lu:%02lu", (pkt->uptime_s / 3600), (pkt->uptime_s % 3600) / 60,
                   (pkt->uptime_s % 60));

    // --- STATO STORAGE & LOGGING ---
    canvas->setCursor(0, 40);
    canvas->print(F("Storage: "));
    if (pkt->flags.is_logging) {
        canvas->setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Testo invertito per "Logging"
        canvas->print(F(" RECORDING "));
        canvas->setTextColor(SSD1306_WHITE);
    } else {
        canvas->print(F("READY / IDLE"));
    }

    // --- DIAGNOSTICA BUS ---
    canvas->setCursor(0, 50);
    canvas->printf("I2C: OK  SPI: OK");

    // --- FOOTER ---
    canvas->drawFastHLine(0, 60, 128, SSD1306_WHITE);
    canvas->setCursor(35, 62);  // Molto in basso
    canvas->setTextSize(1);
    // Nota: setTextSize(1) con 64px di altezza lascia poco spazio,
    // quindi usiamo l'ultima riga disponibile
    canvas->setCursor(30, 56);
    canvas->print(F("[OK] BACK"));
}

const view_interface_t view_info = {
    .on_enter = NULL, .on_update = info_render, .on_input = info_input, .on_exit = NULL};
