/**
 * @file display_ssd1306.cpp
 * @brief Implementazione driver OLED via I2C0 nativo per PCB PPSE Lab.
 */

#include "drivers/display_ssd1306.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "drivers/config_pins.h"

// Usiamo un'allocazione statica sicura legata all'istanza standard "Wire" (I2C0)
// Evita crash di puntatori nulli o disallineamenti di memoria nell'heap dell'RP2040
static Adafruit_SSD1306 display_instance(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
static bool is_initialized = false;

bool display_hw_init() {
    // 1. Configurazione hardware dei pin REALI della PCB su I2C0
    Wire.setSDA(I2C0_SDA_PIN); // GPIO 0
    Wire.setSCL(I2C0_SCL_PIN); // GPIO 1
    
    // 2. Avvio del bus
    Wire.begin();
    Wire.setClock(100000); // 100kHz per debug stabile e tolleranza sui transitori
    delay(100);

    Serial.println("[OLED DRIVER] Avvio scansione di controllo su I2C0...");
    byte error;
    bool device_found = false;
    
    Wire.beginTransmission(SCREEN_ADDRESS);
    error = Wire.endTransmission();

    if (error == 0) {
        Serial.printf("[OLED DRIVER] Hardware rilevato all'indirizzo standard 0x%02X\n", SCREEN_ADDRESS);
        device_found = true;
    } else {
        Serial.println("[OLED DRIVER] ERRORE: Nessun dispositivo risponde a 0x3C. Provo 0x3D...");
        Wire.beginTransmission(0x3D);
        if (Wire.endTransmission() == 0) {
            Serial.println("[OLED DRIVER] Hardware trovato all'indirizzo alternativo 0x3D!");
            device_found = true;
        }
    }

    if (!device_found) {
        Serial.println("[OLED DRIVER] CRITICO: Schermo fisicamente assente o non alimentato!");
        return false;
    }

    // 3. Inizializzazione fisica dei registri interni del controller SSD1306
    delay(100); // Diamo tempo alla logica dello schermo di stabilizzarsi dopo l'accensione del BUCK
    
    if (!display_instance.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("[OLED DRIVER] ERRORE CRITICO: display.begin() ha rifiutato i comandi di inizializzazione.");
        return false;
    }

    Serial.println("[OLED DRIVER] Inizializzato con successo in RAM!");
    
    display_instance.clearDisplay();
    display_instance.setTextSize(1);
    display_instance.setTextColor(SSD1306_WHITE);
    display_instance.setRotation(0);  
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
    if (on) {
        display_instance.ssd1306_command(SSD1306_DISPLAYON);
    } else {
        display_instance.ssd1306_command(SSD1306_DISPLAYOFF);
    }
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
    // Restituisce il puntatore sicuro alla struttura statica allocata
    return (void*)&display_instance; 
}
