#include "ui/views/view_gps.h"

void view_gps_render(SystemDataPacket* data) {
    Adafruit_SSD1306* canvas = get_display_driver();
    canvas->setCursor(0, 0);
    canvas->println(F("GPS TELEMETRY"));
    canvas->drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    canvas->printf("Lat: %.6f\n", data->latitude);
    canvas->printf("Lon: %.6f\n", data->longitude);
    canvas->printf("Sats: %d  Fix: %d\n", data->satellites, data->gps_status);
    canvas->printf("Spd: %.1f km/h\n", data->speed_kmh);
}

void view_gps_handle_input(ButtonId btn) {
    // Qui potresti aggiungere lo start/stop del log su SD
}
