#include "core/system_manager.h"
#include "core/messages.h"
#include <Arduino.h>
#include <pico/sync.h>

static SystemDataPacket shared_data;
static spin_lock_t* data_lock = spin_lock_init(spin_lock_claim_unused(true));
static volatile bool _core1_ready = false;

void sys_manager_init() {
    uint32_t save = spin_lock_blocking(data_lock);
    memset(&shared_data, 0, sizeof(SystemDataPacket));
    spin_unlock(data_lock, save);
    _core1_ready = false;
}

bool sys_manager_send_data(SystemDataPacket* packet) {
    uint32_t save = spin_lock_blocking(data_lock);
    shared_data = *packet;
    spin_unlock(data_lock, save);
    return true;
}

bool sys_manager_receive_data(SystemDataPacket* buffer) {
    uint32_t save = spin_lock_blocking(data_lock);
    *buffer = shared_data;
    spin_unlock(data_lock, save);
    return true;
}

void sys_manager_report_error(error_category_t cat, error_code_t code, bool critical) {
    // 1. Log immediato sulla seriale (Push)
    Serial.printf("\n[!!!] SISTEMA: ERRORE RILEVATO!\n");
    Serial.printf("Categoria: 0x%02X | Codice: 0x%02X | Critico: %s\n", 
                  cat, code, critical ? "SI" : "NO");
    Serial.printf("Uptime: %lu ms\n\n", millis());

    // 2. Aggiornamento stato thread-safe
    uint32_t save = spin_lock_blocking(data_lock);
    shared_data.last_error = {cat, code, millis(), critical};
    shared_data.flags.error_active = 1;
    spin_unlock(data_lock, save);
}

bool sys_is_core1_ready() { return _core1_ready; }
void sys_set_core1_ready(bool ready) { _core1_ready = ready; }

void sys_manager_handle_serial(void) {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd == "GET_GPS") {
            SystemDataPacket frame;
            sys_manager_receive_data(&frame);
            Serial.printf("GPS: %.6f,%.6f\n", frame.latitude, frame.longitude);
        } 
        else if (cmd == "GET_STATUS") {
            SystemDataPacket frame;
            sys_manager_receive_data(&frame);
            Serial.println("--- STATO SISTEMA ---");
            Serial.printf("Error Active: %d\n", frame.flags.error_active);
            if(frame.flags.error_active) {
                Serial.printf("Ultimo Errore: Cat 0x%02X, Code 0x%02X\n", 
                               frame.last_error.category, frame.last_error.code);
            } else {
                Serial.println("System Healthy.");
            }
        }
    }
}
