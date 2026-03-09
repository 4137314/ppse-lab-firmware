#include "ui/views/view_home.h"
#include "ui/ui_manager.h"

void view_home_render(SystemDataPacket* data) {
    Adafruit_SSD1306* canvas = get_display_driver();
    canvas->setTextSize(1);
    canvas->setTextColor(SSD1306_WHITE);
    canvas->setCursor(0, 0);
    canvas->println(F("--- MAIN DASHBOARD ---"));
    canvas->printf("Temp: %.1f C\n", data->temperature);
    canvas->printf("Uptime: %lu s\n", data->uptime_ms / 1000);
    canvas->println(F("\n[OK] per Menu GPS"));
}

void view_home_handle_input(ButtonId btn) {
    if (btn == BTN_OK) {
        ui_manager_switch_to(VIEW_GPS);
    }
}
