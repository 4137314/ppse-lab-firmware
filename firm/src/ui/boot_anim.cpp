/**
 * @file boot_anim.cpp
 * @brief Animazione di boot hacker-style con rendering asincrono dei LED.
 */

#include "ui/boot_anim.h"
#include "drivers/display_ssd1306.h"
#include <Adafruit_SSD1306.h>
#include <FastLED.h> // Incluso per FastLED.delay()

void run_hacker_boot_sequence(void (*update_callback)()) {
    Adafruit_SSD1306* canvas = (Adafruit_SSD1306*)get_display_driver();
    if (!canvas) return;

    const char* modules[] = {"CORE", "RADIO", "GPS", "SENSORS", "UI"};
    int num_modules = 5;

    for (int i = 0; i <= num_modules; i++) {
        canvas->clearDisplay();
        canvas->setTextColor(SSD1306_WHITE);
        
        // --- Render UI ---
        canvas->setCursor(0, 0);
        canvas->print("INIT SYSTEM: ");
        canvas->println(i == num_modules ? "READY" : "LOADING");
        canvas->drawLine(0, 10, 128, 10, SSD1306_WHITE);
        
        canvas->setCursor(0, 15);
        for (int j = 0; j < i; j++) {
            canvas->printf("> %s_READY\n", modules[j]);
        }
        
        if (i < num_modules) {
            canvas->print("> ");
            canvas->print(modules[i]);
            canvas->print((millis() / 300) % 2 == 0 ? "_" : "");
        }

        canvas->drawRect(0, 56, 128, 8, SSD1306_WHITE);
        canvas->fillRect(2, 58, (i * 124 / num_modules), 4, SSD1306_WHITE);
        
        canvas->display();

        // --- Loop di attesa fluido ---
        // Invece di un delay bloccante, aggiorniamo i LED continuamente
        uint32_t wait_duration = 600 + (rand() % 400);
        uint32_t start_wait = millis();
        
        while (millis() - start_wait < wait_duration) {
            if (update_callback) {
                update_callback(); // Aggiorna i LED velocemente
            }
            FastLED.delay(10); // Gestisce il timing FastLED per animazioni fluide
        }
    }
    
    // Pausa finale di conferma prima di passare alla UI principale
    delay(500);
}
