#include "pipeline/core0_manager.h"
#include <Arduino.h>
#include "drivers/config_pins.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "core/system_manager.h"
#include "drivers/peripherals.h"
#include "util/scheduler.h"

// Task schedulati
static Task task_serial = { [](){ sys_manager_handle_serial(); }, 10, 0 };
static Task task_input  = { [](){ 
    ButtonId b = inputs_get_last_press(); 
    if (b != BTN_NONE) ui_manager_dispatch_input(b); 
}, 20, 0 };

static Task task_ui = { [](){ 
    SystemDataPacket frame;
    if (sys_manager_receive_data(&frame)) {
        ui_manager_update(&frame);
        peripherals_auto_feedback(&frame);
    }
}, 33, 0 };

// Task di diagnostica integrato per la seriale
static Task task_health = { [](){ 
    SystemDataPacket frame;
    sys_manager_receive_data(&frame);
    if (!frame.flags.error_active) {
        Serial.printf("[HEARTBEAT] System Healthy | Uptime: %lus\n", millis() / 1000);
    } else {
        Serial.printf("[HEARTBEAT] WARNING: Error Active! | Code: 0x%02X\n", frame.last_error.code);
    }
}, 10000, 0 };

void core0_setup() {
    Serial.begin(115200);
    uint32_t start_time = millis();
    while (!Serial && (millis() - start_time < 1000)) delay(100);

    Serial.println("\n--- PPSE FIRMWARE SYSTEM BOOTING ---");

    sys_manager_init();

    pinMode(LED_ALIVE_PIN, OUTPUT);
    digitalWrite(LED_ALIVE_PIN, HIGH);

    Serial.println("Powering peripherals...");
    pinMode(BUCK_5V_EN_PIN, OUTPUT);
    digitalWrite(BUCK_5V_EN_PIN, HIGH);
    
    peripherals_init(); 
    peripherals_init_leds(); 
    inputs_init();
    
    if (!display_hw_init()) {
        sys_manager_report_error(ERR_CAT_HW, ERR_HW_DISPLAY_LOST, true);
    }
    
    ui_manager_init();
    Serial.println("--- PPSE FIRMWARE SYSTEM READY ---");
}

void core0_loop() {
    // Esecuzione deterministica dei task
    run_task(&task_serial);
    run_task(&task_input);
    run_task(&task_ui);
    run_task(&task_health); // Monitoraggio log seriale attivo
    
    // Heartbeat indicativo
    digitalWrite(LED_ALIVE_PIN, (millis() / 500) % 2);
}
