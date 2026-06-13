#include <Arduino.h>
#include "core/messages.h"
#include "pipeline/core0_manager.h"
#include "pipeline/core1_manager.h"
#include "pico/multicore.h" // Necessario per il lancio esplicito

volatile SystemDataPacket real_system_data;

// Entry point per il Core 1
void setup1() {
    core1_setup();
}

void loop1() {
    core1_loop();
}

void setup() {
    memset((void*)&real_system_data, 0, sizeof(SystemDataPacket));
    core0_setup();
    // Non serve altro, setup1() e loop1() vengono gestiti dal core di Arduino
}

void loop() {
    core0_loop();
}
