#include <Arduino.h>

#include "core/system_manager.h"
#include "core/telemetry.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"

SystemDataPacket local_data;

void setup1() {
    sys_manager_init();
    telemetry_init();
    sys_set_core1_ready(true);
}

void loop1() {
    telemetry_update();
    // FIX: Nome corretto della funzione
    // Corretto: passi l'indirizzo della struct e la sua dimensione
    SystemDataPacket frame;
    telemetry_get_frame(&frame, sizeof(SystemDataPacket));
    // FIX: Nome corretto della funzione
    sys_manager_send_data(&frame);
    delay(250);
}

void setup() {
    Serial.begin(9600);
    display_hw_init();
    inputs_init();
    ui_manager_init();

    while (!sys_is_core1_ready()) delay(1);
}

void loop() {
    inputs_update();
    ButtonId btn = inputs_get_last_press();

    // FIX: Nome corretto della funzione
    if (sys_manager_receive_data(&local_data)) {
        // Opzionale: logica se i dati sono cambiati
    }

    // FIX: Nome corretto della funzione (dispatch invece di handle)
    ui_manager_dispatch_input(btn);
    ui_manager_update(&local_data);

    delay(16);
}
