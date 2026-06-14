#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include "core/config.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"
#include "drivers/peripherals.h"
#include "drivers/display_ssd1306.h"

// Voci del menu (rimosso SETTING_BACK)
enum { SETTING_BRIGHTNESS, SETTING_BUZZER, SETTING_LEDS, SETTING_INFO, SETTING_COUNT };
static uint8_t cursor = 0;

// static void settings_input(button_t btn, button_state_t state) {
//     if (state != BTN_RELEASED) return;
//
//     switch (btn) {
//         case BTN_UP:   
//             cursor = (cursor == 0) ? SETTING_COUNT - 1 : cursor - 1; 
//             break;
//         case BTN_DOWN: 
//             cursor = (cursor + 1) % SETTING_COUNT; 
//             break;
//         case BTN_BACK: 
//             ui_manager_navigate_to(VIEW_ID_HOME); 
//             break;
//         case BTN_OK:
//             // Feedback sonoro universale all'OK
//             peripherals_play_click(); 
//
//             switch(cursor) {
//                 case SETTING_BRIGHTNESS: 
//                     // Logica robusta: forziamo il valore al prossimo step di 50
//                     // Se siamo a 250 o più, resettiamo a 50
//                     if (global_cfg.oled_brightness >= 250) {
//                         global_cfg.oled_brightness = 50;
//                     } else {
//                         // Calcolo matematico per arrotondare al multiplo di 50 superiore
//                         global_cfg.oled_brightness = ((global_cfg.oled_brightness / 50) + 1) * 50;
//                     }
//
//                     // Limite massimo di sicurezza per SSD1306
//                     if (global_cfg.oled_brightness > 255) global_cfg.oled_brightness = 255;
//
//                     display_set_brightness(global_cfg.oled_brightness);
//                     break;
//
//                 case SETTING_BUZZER: 
//                     global_cfg.buzzer_enabled = !global_cfg.buzzer_enabled;
//                     peripherals_set_buzzer(global_cfg.buzzer_enabled);
//                     break;
//
//                 case SETTING_LEDS:   
//                     global_cfg.leds_enabled = !global_cfg.leds_enabled;
//                     peripherals_set_leds(global_cfg.leds_enabled);
//                     break;
//
//                 case SETTING_INFO:   
//                     ui_manager_navigate_to(VIEW_ID_INFO); 
//                     break;
//             }
//             break;
//         default: break;
//     }
// }

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
            // Feedback sonoro condizionato:
            // Se siamo in debug (DEFAULT_BUZZER_OFF), non suoniamo mai.
            // Altrimenti, lasciamo che sia il driver a decidere in base alla config.
            #ifndef DEFAULT_BUZZER_OFF
                peripherals_play_click(); 
            #endif

            switch(cursor) {
                case SETTING_BRIGHTNESS: 
                    if (global_cfg.oled_brightness >= 250) {
                        global_cfg.oled_brightness = 50;
                    } else {
                        global_cfg.oled_brightness = ((global_cfg.oled_brightness / 50) + 1) * 50;
                    }
                    if (global_cfg.oled_brightness > 255) global_cfg.oled_brightness = 255;
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
    const char* labels[] = {"Bright", "Buzzer", "LEDs", "Info"}; // Rimosso "Back"

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

        if (cursor == i) {
            canvas->setCursor(80, y);
            if (i == SETTING_BRIGHTNESS) canvas->printf("%d", global_cfg.oled_brightness);
            else if (i == SETTING_BUZZER) canvas->print(global_cfg.buzzer_enabled ? F("ON") : F("OFF"));
            else if (i == SETTING_LEDS)   canvas->print(global_cfg.leds_enabled ? F("ON") : F("OFF"));
        }
    }
}

const view_interface_t view_settings = {
    .on_enter = NULL, 
    .on_update = settings_render, 
    .on_input = settings_input, 
    .on_exit = NULL
};
