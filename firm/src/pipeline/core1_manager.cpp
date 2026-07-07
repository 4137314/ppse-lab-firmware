#include "pipeline/core1_manager.h"
#include <Arduino.h>
#include "drivers/sensors_i2c.h"
#include "core/messages.h"
#include "core/telemetry.h"
#include "core/system_manager.h" // Necessario per sys_manager_send_data
#include "drivers/config_pins.h"

void core1_setup() {
    Serial.println("CORE 1: Avvio alimentazione GPS...");
    
    pinMode(SAM_EN_PIN, OUTPUT);
    digitalWrite(SAM_EN_PIN, LOW); // PMOS: LOW = ON
    
    sensors_i2c_init();
    telemetry_init(); 
    
    Serial.println("CORE 1: Init completato.");
}

void core1_loop() {
    telemetry_update();
    
    delay(100); // Riduce il carico sul bus I2C/SPI
}
