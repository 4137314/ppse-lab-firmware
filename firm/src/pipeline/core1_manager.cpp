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
    
    SystemDataPacket frame;
    telemetry_get_frame(&frame);
    
    // CAMBIAMENTO: Non inviare più tutto il pacchetto frame.
    // Invia solo le coordinate GPS usando la funzione chirurgica.
    sys_manager_update_gps(frame.latitude, frame.longitude, frame.gps_status, frame.satellites);
    // Debug
    static uint32_t last_log = 0;
    if (millis() - last_log > 5000) {
        Serial.println("CORE 1: GPS Update Sent...");
        last_log = millis();
    }
    
    delay(50);
}
