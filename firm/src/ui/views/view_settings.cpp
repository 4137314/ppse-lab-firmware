#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <LittleFS.h>
#include "core/config.h"
#include "core/storage.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"
#include "drivers/peripherals.h"
#include "drivers/display_ssd1306.h"

enum { SETTING_BRIGHTNESS, SETTING_BUZZER, SETTING_LEDS, SETTING_ERASE, SETTING_INFO, SETTING_COUNT };
static uint8_t cursor = 0;
static bool erase_confirm = false;
static uint32_t confirm_timer = 0; // Timer per nascondere "OK"

static void settings_input(button_t btn, button_state_t state) {
    if (state != BTN_RELEASED) return;

    switch (btn) {
        case BTN_UP:   
            cursor = (cursor == 0) ? SETTING_COUNT - 1 : cursor - 1; 
            break;
        case BTN_DOWN: 
            cursor = (cursor + 1) % SETTING_COUNT; 
            break;
        case BTN_BACK: 
            ui_manager_navigate_to(VIEW_ID_HOME); 
            break;
        case BTN_OK:
            #ifndef DEFAULT_BUZZER_OFF
                peripherals_play_click(); 
            #endif

            switch(cursor) {
                case SETTING_BRIGHTNESS: 
                    global_cfg.oled_brightness = (global_cfg.oled_brightness >= 250) ? 50 : ((global_cfg.oled_brightness / 50) + 1) * 50;
                    display_set_brightness(global_cfg.oled_brightness);
                    break;
                case SETTING_BUZZER: 
                    global_cfg.buzzer_enabled = !global_cfg.buzzer_enabled;
                    peripherals_set_buzzer(global_cfg.buzzer_enabled);
                    break;
                case SETTING_LEDS:   
                    global_cfg.leds_enabled = !global_cfg.leds_enabled;
                    peripherals_set_leds(global_cfg.leds_enabled);
                    break;
                case SETTING_ERASE:
                    LittleFS.end();
                    LittleFS.format();
                    LittleFS.begin();
                    erase_confirm = true;
                    confirm_timer = millis(); // Avvia il timer di 2 secondi
                    break;
                case SETTING_INFO:   
                    ui_manager_navigate_to(VIEW_ID_INFO); 
                    break;
            }
            break;
        default: break;
    }
}

static void settings_render(const void* data) {
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    const char* labels[] = {"Bright", "Buzzer", "LEDs", "Erase Flash", "Info"};

    // Reset automatico della scritta OK dopo 2 secondi
    if (erase_confirm && (millis() - confirm_timer > 2000)) {
        erase_confirm = false;
    }

    canvas->clearDisplay();
    canvas->fillRect(0, 0, 128, 12, SSD1306_WHITE);
    canvas->setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    canvas->setCursor(35, 2);
    canvas->print(F("SETTINGS"));

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

        if (cursor == i) {
            canvas->setCursor(80, y);
            if (i == SETTING_BRIGHTNESS) canvas->printf("%d", global_cfg.oled_brightness);
            else if (i == SETTING_BUZZER) canvas->print(global_cfg.buzzer_enabled ? F("ON") : F("OFF"));
            else if (i == SETTING_LEDS)   canvas->print(global_cfg.leds_enabled ? F("ON") : F("OFF"));
            else if (i == SETTING_ERASE && erase_confirm) canvas->print(F("OK"));
        }
    }
    canvas->display();
}

const view_interface_t view_settings = {
    .on_enter = NULL, 
    .on_update = settings_render, 
    .on_input = settings_input, 
    .on_exit = NULL
};
