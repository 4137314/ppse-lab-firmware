#include "ui/views/view_settings.h"
#include "drivers/display_ssd1306.h"
#include "drivers/peripherals.h"

static uint8_t brightness = 150;

void view_settings_render(SystemDataPacket* data) {
    Adafruit_SSD1306* canvas = get_display_driver();
    
    canvas->setCursor(0, 0);
    canvas->println(F("SETTINGS"));
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    canvas->setCursor(0, 25);
    canvas->printf("> Brightness: %d\n", brightness);
    canvas->println(F("  LED Test: [OK]"));
    canvas->println(F("  System Reset"));
    
    canvas->setCursor(0, 55);
    canvas->println(F("UP/DN to adjust"));
}

void view_settings_handle_input(ButtonId btn) {
    if (btn == BTN_UP && brightness < 255) {
        brightness += 25;
        display_set_brightness(brightness);
    } 
    else if (btn == BTN_DOWN && brightness > 0) {
        brightness -= 25;
        display_set_brightness(brightness);
    }
    else if (btn == BTN_OK) {
        peripherals_trigger_feedback(FEEDBACK_SUCCESS);
    }
}
