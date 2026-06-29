#include <Arduino.h>
#include "core/messages.h"
#include "core/config.h"
#include "pipeline/core0_manager.h"
#include "pipeline/core1_manager.h"
#include "pico/multicore.h"

// La variabile globale è definita in config.cpp, qui è solo un riferimento
extern SystemConfig global_cfg; 

void setup1() {
    core1_setup();
}

void loop1() {
    core1_loop();
}

void setup() {
    // disable the watchdog timer
    watchdog_hw->ctrl &= ~(1<<30);
    core0_setup();
}

void loop() {
    core0_loop();
}
