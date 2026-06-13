#include "pipeline/tasks.h"
#include "core/system_manager.h"
#include "ui/ui_manager.h"
#include "drivers/peripherals.h"
#include "drivers/inputs.h"

// Qui definiamo i task che prima avevi sparsi nel manager
Task task_serial = { [](){ sys_manager_handle_serial(); }, 10, 0 };

Task task_input  = { [](){ 
    ButtonId b = inputs_get_last_press(); 
    if (b != BTN_NONE) ui_manager_dispatch_input(b); 
}, 20, 0 };

Task task_ui = { [](){ 
    SystemDataPacket frame;
    if (sys_manager_receive_data(&frame)) {
        ui_manager_update(&frame);
        peripherals_auto_feedback(&frame);
    }
}, 33, 0 };

Task task_health = { [](){ 
    SystemDataPacket frame;
    sys_manager_receive_data(&frame);
    if (!frame.flags.error_active) {
        Serial.printf("[HEARTBEAT] System Healthy | Uptime: %lus\n", millis() / 1000);
    } else {
        Serial.printf("[HEARTBEAT] WARNING: Error Active! | Code: 0x%02X\n", frame.last_error.code);
    }
}, 10000, 0 };
