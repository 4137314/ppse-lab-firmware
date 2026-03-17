/**
 * @file display_ssd1306.cpp
 * @brief Implementazione driver OLED via I2C per RP2040.
 */

#include "drivers/display_ssd1306.h"
#include "drivers/config_pins.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// L'oggetto display è statico: visibile solo in questo file.
// Usiamo -1 per il reset se non gestito da un pin GPIO dedicato.
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool display_hw_init() {
    /** * Configurazione I2C0 su RP2040. 
     * Pin da schema: GPIO 12 (SDA), GPIO 13 (SCL).
     */
    Wire.setSDA(I2C0_SDA_PIN); 
    Wire.setSCL(I2C0_SCL_PIN);
    Wire.setClock(400000); // 400kHz per un refresh fluido della UI
    Wire.begin();

    // Avvio del controller
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        return false;
    }

    // Configurazione iniziale standard
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setRotation(0); // 0 o 2 (180°) a seconda di come è montato sulla board
    display.display();
    
    return true;
}

bool display_is_alive() {
    Wire.beginTransmission(SCREEN_ADDRESS);
    return (Wire.endTransmission() == 0);
}

void display_clear() {
    display.clearDisplay();
}

void display_show() {
    display.display();
}

void display_set_power(bool on) {
    if (on) {
        display.ssd1306_command(SSD1306_DISPLAYON);
    } else {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
    }
}

void display_set_brightness(uint8_t level) {
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(level);
}

void display_draw_pixel(int16_t x, int16_t y, uint16_t color) {
    display.drawPixel(x, y, color);
}

void* get_display_driver() {
    return (void*)&display;
}
