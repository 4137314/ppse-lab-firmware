#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ui/view_definitions.h"
#include "ui/ui_manager.h"
#include "core/messages.h"       // Risolve 'SystemDataPacket'
#include "drivers/display_ssd1306.h" // Risolve 'get_display_driver' e 'display_set_brightness'
#include "drivers/inputs.h"      // Risolve 'BTN_OK', 'BTN_UP', ecc.
#include "drivers/peripherals.h"

static uint8_t brightness = 150;

static void settings_render(const void* data) {
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
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

static void settings_input(uint8_t btn) {
    if (btn == BTN_UP && brightness < 255) {
        brightness = (brightness + 25 > 255) ? 255 : brightness + 25;
        display_set_brightness(brightness);
    } 
    else if (btn == BTN_DOWN && brightness > 0) {
        brightness = (brightness < 25) ? 0 : brightness - 25;
        display_set_brightness(brightness);
    }
    else if (btn == BTN_OK) {
        peripherals_trigger_feedback(FEEDBACK_SUCCESS);
    }
}

const view_interface_t view_settings = {
    .on_enter = NULL,
    .on_update = settings_render,
    .on_input = settings_input,
    .on_exit = NULL
};
