/**
 * @file telemetry.cpp
 * @brief Gestione acquisizione sensori e parsing GPS.
 */

#include "core/telemetry.h"
#include "drivers/peripherals.h"
#include <minmea.h>

// Pacchetto di lavoro interno al Core 1
static SystemDataPacket local_frame;

void telemetry_init() {
    memset(&local_frame, 0, sizeof(SystemDataPacket));
    // Qui inizializzeremo la Serial1 per il modulo GPS
    Serial1.begin(9600); // Baudrate standard GPS
}

void telemetry_update() {
    // 1. Leggi Temperatura dai Driver
    local_frame.temperature = peripherals_read_temperature();
    
    // 2. Leggi Uptime
    local_frame.uptime_ms = millis();

    // 3. Logica GPS (Placeholder per il parser minmea)
    while (Serial1.available()) {
        char c = Serial1.read();
        // Qui integreremo minmea_parse(c) nel prossimo step
    }

    // 4. Invia il pacchetto aggiornato al System Manager
    sys_manager_send_data(&local_frame);
}

SystemDataPacket telemetry_get_current_frame() {
    return local_frame;
}
