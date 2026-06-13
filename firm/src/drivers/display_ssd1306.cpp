/**
 * @file display_ssd1306.cpp
 * @brief Implementazione driver OLED via I2C0 nativo per PCB PPSE Lab.
 */

#include "drivers/display_ssd1306.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "drivers/config_pins.h"

// Istanziamento statico del driver
static Adafruit_SSD1306 display_instance(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
static bool is_initialized = false;

bool display_hw_init() {
    // Configurazione hardware pin I2C0
    Wire.setSDA(I2C0_SDA_PIN);
    Wire.setSCL(I2C0_SCL_PIN);
    Wire.begin();
    Wire.setClock(400000); // Passiamo a 400kHz per una UI più fluida (OLED SSD1306 supporta fino a 400kHz)

    // Verifica presenza hardware
    Wire.beginTransmission(SCREEN_ADDRESS);
    if (Wire.endTransmission() != 0) {
        Serial.println("[OLED] CRITICO: Hardware non rilevato!");
        return false;
    }

    if (!display_instance.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        return false;
    }

    display_instance.clearDisplay();
    display_instance.display();
    is_initialized = true;
    return true;
}

bool display_is_alive() {
    if (!is_initialized) return false;
    Wire.beginTransmission(SCREEN_ADDRESS);
    return (Wire.endTransmission() == 0);
}

void display_clear() {
    if (is_initialized) display_instance.clearDisplay();
}

void display_show() {
    if (is_initialized) display_instance.display();
}

void display_set_power(bool on) {
    if (!is_initialized) return;
    display_instance.ssd1306_command(on ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
}

void display_set_brightness(uint8_t level) {
    if (!is_initialized) return;
    display_instance.ssd1306_command(SSD1306_SETCONTRAST);
    display_instance.ssd1306_command(level);
}

void display_draw_pixel(int16_t x, int16_t y, uint16_t color) {
    if (is_initialized) display_instance.drawPixel(x, y, color);
}

void* get_display_driver() {
    // Restituiamo il puntatore all'istanza. 
    // Il cast avverrà nel file che chiama questa funzione.
    return (void*)&display_instance; 
}
