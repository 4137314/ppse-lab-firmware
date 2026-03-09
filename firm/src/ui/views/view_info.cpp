#include "ui/views/view_info.h"

void view_info_render(SystemDataPacket* data) {
    Adafruit_SSD1306* canvas = get_display_driver();
    
    canvas->setCursor(0, 0);
    canvas->println(F("SYSTEM INFO"));
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    canvas->setCursor(0, 20);
    canvas->printf("Firmware: v2.0-Alpha\n");
    canvas->printf("Uptime: %lu ms\n", data->uptime_ms);
    canvas->printf("Core 1 Status: OK\n");
    
    // Barra di stato SD
    canvas->setCursor(0, 50);
    canvas->print(F("Storage: "));
    canvas->println(data->sd_logging ? F("LOGGING...") : F("READY"));
}

void view_info_handle_input(ButtonId btn) {
    // Ritorna alla home è già gestito dal manager (BTN_BACK)
}
