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
    // RIMOZIONE: Non serve più il memset qui, 
    // sys_manager_init() dentro core0_setup() se ne occupa già.
    
    // Ora core0_setup() chiama internamente config_init()
    core0_setup();
}

void loop() {
    core0_loop();
}
