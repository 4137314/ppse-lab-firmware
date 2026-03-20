#include <Arduino.h>

#include "core/storage.h"
#include "core/system_manager.h"
#include "core/telemetry.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"

SystemDataPacket local_data;

void setup1() {
    if (!storage_init()) {
        Serial.println("Critico: Errore Storage!");
    }
    sys_manager_init();
    telemetry_init();
    sys_set_core1_ready(true);
}

void loop1() {
    if (storage_is_busy_by_usb()) {
        delay(100);
        return;
    }

    telemetry_update();

    SystemDataPacket frame;
    telemetry_get_frame(&frame, sizeof(SystemDataPacket));

    static uint32_t lastSaveMs = 0;

    // Correzione basata sull'errore: gps_data -> gps_status
    // E correzione dei campi lat/lon/satellites (presunti piatti o dentro gps_status)
    if (frame.gps_status > 0 && (millis() - lastSaveMs >= 10000)) {
        char log_buffer[128];

        // Se lat/lon/satellites danno ancora errore, verifica telemetry.h
        // Qui assumo che siano membri di SystemDataPacket visto l'errore precedente
        snprintf(log_buffer, sizeof(log_buffer), "%lu,%.6f,%.6f,%.2f,%.1f,%u", millis(),
                 frame.latitude, frame.longitude, frame.speed_ms, frame.temp_c, frame.satellites);

        storage_log_append(log_buffer);
        lastSaveMs = millis();
    }

    sys_manager_send_data(&frame);
    delay(250);
}

void setup() {
    Serial.begin(9600);
    while (!sys_is_core1_ready()) delay(1);

    display_hw_init();
    inputs_init();
    ui_manager_init();
}

void loop() {
    inputs_update();
    ButtonId btn = inputs_get_last_press();

    if (sys_manager_receive_data(&local_data)) {
        // Update locale ok
    }

    ui_manager_dispatch_input(btn);
    ui_manager_update(&local_data);

    delay(16);
}
