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
    // 1. Aggiorna lo stato interno del modulo telemetria
    telemetry_update(); 
    
    // 2. Prepara il pacchetto dati locale
    SystemDataPacket frame;
    telemetry_get_frame(&frame); // Recupera i dati aggiornati dalla telemetria
    
    // 3. Arricchisce il frame con dati dai sensori I2C
    frame.uptime_s  = millis() / 1000;
    frame.temp_c    = sensors_read_temperature_c();
    frame.battery_v = sensors_read_battery_v();
    
    // 4. Invia il pacchetto al bus (Thread-safe verso Core 0)
    sys_manager_send_data(&frame);
    
    // Debug: segnale di vita
    static uint32_t last_log = 0;
    if (millis() - last_log > 5000) {
        Serial.println("CORE 1: Telemetry Bus Push Alive...");
        last_log = millis();
    }
    
    delay(50);
}
