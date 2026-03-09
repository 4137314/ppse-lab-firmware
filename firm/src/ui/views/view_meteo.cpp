#include "ui/views/view_meteo.h"
#include "drivers/peripherals.h"

void view_meteo_render(SystemDataPacket* data) {
    Adafruit_SSD1306* canvas = get_display_driver();
    
    canvas->setCursor(30, 0);
    canvas->println(F("ENVIRONMENT"));
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    // Visualizzazione Temperatura Grande
    canvas->setTextSize(2);
    canvas->setCursor(20, 25);
    canvas->printf("%.1f C", data->temperature);
    
    // Icona termometro stilizzata (rettangolo + cerchio)
    canvas->drawRoundRect(5, 20, 10, 30, 5, SSD1306_WHITE);
    canvas->fillCircle(10, 45, 4, SSD1306_WHITE);

    canvas->setTextSize(1);
    canvas->setCursor(0, 55);
    canvas->println(F("Status: Sensor Active"));
}

void view_meteo_handle_input(ButtonId btn) {
    // Implementazione futura: switch tra Celsius e Fahrenheit
}
