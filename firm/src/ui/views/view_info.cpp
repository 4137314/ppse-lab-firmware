#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

static void info_input(button_t btn, button_state_t state) {
    if (state == BTN_RELEASED) {
        ui_manager_navigate_to(VIEW_ID_HOME);
    }
}

static void info_render(const void* data) {
    const SystemDataPacket* pkt = (const SystemDataPacket*)data;
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();

    canvas->clearDisplay();
    canvas->setTextSize(1);
    canvas->setTextColor(SSD1306_WHITE);

    // 0: Header
    canvas->setCursor(0, 0);
    canvas->print(F("DIAGNOSTICA"));
    canvas->drawFastHLine(0, 9, 128, SSD1306_WHITE);

    // 1: FW Version (Y=12)
    canvas->setCursor(0, 12);
    canvas->print(F("FW: v2.1-Agno"));

    // 2: Uptime (Y=24)
    canvas->setCursor(0, 24);
    canvas->printf("UP: %02lu:%02lu:%02lu", 
        (pkt->uptime_s / 3600), 
        (pkt->uptime_s % 3600) / 60, 
        (pkt->uptime_s % 60));

    // 3: Logging Status (Y=36)
    canvas->setCursor(0, 36);
    canvas->print(F("LOG: "));
    canvas->print(pkt->flags.is_logging ? F("RECORDING") : F("READY"));

    // 4: Bus Status (Y=48)
    canvas->setCursor(0, 48);
    canvas->print(F("BUS: I2C/SPI OK"));

    // NIENTE PIU' FOOTER A Y=62 (troppo vicino al bordo)
    // Usiamo l'ultima riga disponibile per il comando di uscita
    canvas->setCursor(0, 56);
    canvas->print(F("OK -> HOME"));

    canvas->display();
}

const view_interface_t view_info = {
    .on_enter = NULL, .on_update = info_render, .on_input = info_input, .on_exit = NULL
};
