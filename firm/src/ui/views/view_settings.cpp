#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include "core/config.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"
#include "drivers/peripherals.h"
#include "drivers/display_ssd1306.h" // Per get_display_driver() e display_set_brightness()

// Voci del menu
enum { SETTING_BRIGHTNESS, SETTING_BUZZER, SETTING_LEDS, SETTING_INFO, SETTING_BACK, SETTING_COUNT };
static uint8_t cursor = 0;

static void settings_input(button_t btn, button_state_t state) {
    if (state != BTN_RELEASED) return;

    switch (btn) {
        case BTN_UP:    cursor = (cursor == 0) ? SETTING_COUNT - 1 : cursor - 1; break;
        case BTN_DOWN:  cursor = (cursor + 1) % SETTING_COUNT; break;
        case BTN_OK:
            switch(cursor) {
                case SETTING_BRIGHTNESS: 
                    global_cfg.oled_brightness = (global_cfg.oled_brightness >= 255) ? 50 : global_cfg.oled_brightness + 50;
                    display_set_brightness(global_cfg.oled_brightness);
                    break;
                case SETTING_BUZZER: 
                    global_cfg.buzzer_enabled = !global_cfg.buzzer_enabled;
                    peripherals_set_buzzer(global_cfg.buzzer_enabled); // Esegue comando fisico
                    break;
                case SETTING_LEDS:   
                    global_cfg.leds_enabled = !global_cfg.leds_enabled;
                    peripherals_set_leds(global_cfg.leds_enabled);     // Esegue comando fisico
                    break;
                case SETTING_INFO:   ui_manager_navigate_to(VIEW_ID_INFO); break;
                case SETTING_BACK:   ui_manager_navigate_to(VIEW_ID_HOME); break;
            }
            break;
        default: break;
    }
}

static void settings_render(const void* data) {
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    const char* labels[] = {"Bright.", "Buzzer", "LEDs", "Info", "Back"};

    // Header
    canvas->fillRect(0, 0, 128, 12, SSD1306_WHITE);
    canvas->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    canvas->setCursor(35, 2);
    canvas->print(F("SETTINGS"));

    // Menu
    canvas->setTextColor(SSD1306_WHITE);
    for (uint8_t i = 0; i < SETTING_COUNT; i++) {
        uint8_t y = 16 + (i * 9);
        if (cursor == i) {
            canvas->fillRect(0, y - 1, 128, 9, SSD1306_WHITE);
            canvas->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        } else {
            canvas->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
        }

        canvas->setCursor(2, y);
        canvas->print(labels[i]);

        // Rendering valori dinamici
        if (cursor == i) {
            canvas->setCursor(80, y);
            if (i == SETTING_BRIGHTNESS) canvas->printf("%d", global_cfg.oled_brightness);
            else if (i == SETTING_BUZZER) canvas->print(global_cfg.buzzer_enabled ? F("ON") : F("OFF"));
            else if (i == SETTING_LEDS)   canvas->print(global_cfg.leds_enabled ? F("ON") : F("OFF"));
        }
    }
}

const view_interface_t view_settings = {
    .on_enter = NULL, .on_update = settings_render, .on_input = settings_input, .on_exit = NULL};
