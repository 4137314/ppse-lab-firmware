/**
 * @file display_ssd1306.cpp
 * @brief Implementazione driver OLED via I2C.
 */

#include "drivers/display_ssd1306.h"

// Istanza globale del display (statica per nasconderla ad altri file)
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool display_hw_init() {
    // Inizializza I2C sui pin corretti definiti in config_pins.h
    Wire.setSDA(DISPLAY_SDA_PIN);
    Wire.setSCL(DISPLAY_SCL_PIN);
    Wire.begin();

    // Avvio del controller SSD1306
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        return false;
    }

    display.clearDisplay();
    display.display();
    return true;
}

void display_clear() {
    display.clearDisplay();
}

void display_show() {
    display.display();
}

void display_set_brightness(uint8_t level) {
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(level);
}

Adafruit_SSD1306* get_display_driver() {
    return &display;
}
