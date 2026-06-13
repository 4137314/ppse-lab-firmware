#include "pipeline/core0_manager.h"
#include <Arduino.h>
#include "drivers/config_pins.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "core/system_manager.h" // Modulo centralizzato per i dati
#include "drivers/peripherals.h"

// Rimosso "extern volatile SystemDataPacket real_system_data"
// Ora il Core 0 non possiede dati, li richiede al System Manager

void core0_setup() {
    Serial.begin(115200);
    uint32_t start_time = millis();
    while (!Serial && (millis() - start_time < 3000)) delay(100);

    Serial.println("\n--- PPSE FIRMWARE SYSTEM BOOTING ---");

    // Init Manager di Sistema (inizializza gli spin_lock e la memoria)
    sys_manager_init();

    pinMode(LED_ALIVE_PIN, OUTPUT);
    for(int i=0; i<3; i++) {
        digitalWrite(LED_ALIVE_PIN, HIGH); delay(100);
        digitalWrite(LED_ALIVE_PIN, LOW); delay(100);
    }
    digitalWrite(LED_ALIVE_PIN, HIGH);

    Serial.println("Powering peripherals (Core 0)...");
    pinMode(BUCK_5V_EN_PIN, OUTPUT);
    digitalWrite(BUCK_5V_EN_PIN, HIGH);
    
    peripherals_init(); 
    peripherals_init_leds(); 
    inputs_init();
    
    if (!display_hw_init()) {
        while(1) { digitalWrite(LED_ALIVE_PIN, !digitalRead(LED_ALIVE_PIN)); delay(100); }
    }
    
    ui_manager_init();
    Serial.println("--- PPSE FIRMWARE SYSTEM READY ---");
}

void core0_loop() {
    // 1. Gestione comandi seriali (spostata nel system_manager)
    sys_manager_handle_serial();
    
    // 2. Acquisizione dati atomica e UI
    SystemDataPacket frame;
    if (sys_manager_receive_data(&frame)) {
        // Aggiorna UI solo se ci sono dati pronti
        ui_manager_update(&frame);
        // Feedback basato sui dati letti
        peripherals_auto_feedback((const SystemDataPacket*)&frame);
    }
    
    // 3. Gestione Input
    ButtonId pressed_btn = inputs_get_last_press();
    if (pressed_btn != BTN_NONE) {
        ui_manager_dispatch_input(pressed_btn);
    }
    
    // 4. LED Alive (Beat)
    digitalWrite(LED_ALIVE_PIN, (millis() / 500) % 2);
}
