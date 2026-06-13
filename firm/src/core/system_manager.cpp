#include "core/system_manager.h"

#include "core/messages.h"  // Per sicurezza includilo esplicitamente
#include "pico/util/queue.h"
#include "core/system_manager.h"
#include <Arduino.h>     // <--- AGGIUNGI QUESTO!


static queue_t Qdata;
static volatile bool _core1_ready = false;

void sys_manager_init() {
    // Ora sizeof(SystemDataPacket) funzionerà perché il nome è corretto
    queue_init(&Qdata, sizeof(SystemDataPacket), 4);
    _core1_ready = false;
}

bool sys_manager_send_data(SystemDataPacket* packet) {
    return queue_try_add(&Qdata, packet);
}

bool sys_manager_receive_data(SystemDataPacket* buffer) {
    bool updated = false;
    while (queue_try_remove(&Qdata, buffer)) {
        updated = true;
    }
    return updated;
}

bool sys_is_core1_ready() {
    return _core1_ready;
}
void sys_set_core1_ready(bool ready) {
    _core1_ready = ready;
}

void sys_manager_handle_serial(void) {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd == "GET_GPS") {
            // Accediamo in sicurezza ai dati condivisi
            SystemDataPacket frame;
            sys_manager_receive_data(&frame);
            Serial.printf("%.6f,%.6f\n", frame.latitude, frame.longitude);
        }
    }
}
