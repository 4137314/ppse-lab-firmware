#include <Arduino.h>
#include "core/messages.h"
#include "pipeline/core0_manager.h"
#include "pipeline/core1_manager.h"

// Definizione variabile condivisa
volatile SystemDataPacket real_system_data;

void setup() {
    memset((void*)&real_system_data, 0, sizeof(SystemDataPacket));
    core0_setup();
}

void loop() {
    core0_loop();
}

void setup1() {
    core1_setup();
}

void loop1() {
    core1_loop();
}
