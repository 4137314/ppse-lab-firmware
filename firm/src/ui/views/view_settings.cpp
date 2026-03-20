#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include "core/messages.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "drivers/peripherals.h"
#include "ui/ui_manager.h"
#include "ui/view_definitions.h"

// Enumerazione per gestire le voci del menu
enum { SETTING_BRIGHTNESS, SETTING_LED_TEST, SETTING_BACK, SETTING_COUNT };

static uint8_t cursor     = 0;
static uint8_t brightness = 150;

/**
 * @brief Gestione input con navigazione a cursore.
 */
static void settings_input(button_t btn, button_state_t state) {
    if (state != BTN_RELEASED)
        return;

    switch (btn) {
        case BTN_UP:
            cursor = (cursor == 0) ? SETTING_COUNT - 1 : cursor - 1;
            break;

        case BTN_DOWN:
            cursor = (cursor + 1) % SETTING_COUNT;
            break;

        case BTN_OK:
            if (cursor == SETTING_BRIGHTNESS) {
                // Cicla la luminosità: 50 -> 150 -> 255 -> 50
                brightness = (brightness < 100) ? 150 : (brightness < 200 ? 255 : 50);
                display_set_brightness(brightness);
                peripherals_trigger_feedback(FEEDBACK_SUCCESS);
            } else if (cursor == SETTING_LED_TEST) {
                // CORRETTO: Usiamo FEEDBACK_ERROR invece di FEEDBACK_ALERT
                peripherals_trigger_feedback(FEEDBACK_ERROR);
            } else if (cursor == SETTING_BACK) {
                ui_manager_navigate_to(VIEW_ID_HOME);
            }
            break;

        default:
            break;
    }
}

/**
 * @brief Rendering del menu Settings con cursore dinamico.
 */
static void settings_render(const void* data) {
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();

    // --- HEADER ---
    canvas->setTextSize(1);
    canvas->setTextColor(SSD1306_WHITE);
    canvas->setCursor(40, 0);
    canvas->print(F("SETTINGS"));
    canvas->drawFastHLine(0, 10, 128, SSD1306_WHITE);

    // --- LISTA OPZIONI ---
    const char* labels[] = {"Brightness", "LED Test", "Back to Home"};

    for (uint8_t i = 0; i < SETTING_COUNT; i++) {
        uint8_t y = 20 + (i * 12);
        canvas->setCursor(10, y);

        // Disegna il cursore (freccia) sulla voce selezionata
        if (cursor == i) {
            canvas->print(F("> "));
        } else {
            canvas->print(F("  "));
        }

        canvas->print(labels[i]);

        // Mostra il valore attuale solo per la luminosità
        if (i == SETTING_BRIGHTNESS) {
            canvas->print(F(": "));
            canvas->print(brightness);
        }
    }

    // --- FOOTER ISTRUZIONI ---
    canvas->drawFastHLine(0, 54, 128, SSD1306_WHITE);
    canvas->setCursor(10, 57);
    canvas->print(F("UP/DN: Move  OK: Sel"));
}

const view_interface_t view_settings = {
    .on_enter = NULL, .on_update = settings_render, .on_input = settings_input, .on_exit = NULL};
