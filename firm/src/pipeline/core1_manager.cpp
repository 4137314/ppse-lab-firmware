#include "pipeline/core1_manager.h"
#include <Arduino.h>
#include "drivers/sensors_i2c.h"
#include "core/messages.h"
#include "core/telemetry.h"
#include "drivers/config_pins.h" // Necessario per SAM_EN_PIN

extern volatile SystemDataPacket real_system_data;

void core1_setup() {
    Serial.println("CORE 1: Avvio alimentazione GPS...");
    
    // ACCENSIONE CORRETTA: Logica PMOS (LOW = ON)
    pinMode(SAM_EN_PIN, OUTPUT);
    digitalWrite(SAM_EN_PIN, LOW); 
    
    sensors_i2c_init();
    telemetry_init(); 
    
    Serial.println("CORE 1: Init completato.");
}

void core1_loop() {
    telemetry_update(); 
    
    real_system_data.uptime_s = millis() / 1000;
    real_system_data.temp_c   = sensors_read_temperature_c();
    real_system_data.battery_v = sensors_read_battery_v();
    
    // Debug: segnale di vita nel loop
    static uint32_t last_log = 0;
    if (millis() - last_log > 5000) {
        Serial.println("CORE 1: Telemetry Loop Alive...");
        last_log = millis();
    }
    
    delay(50);
}
