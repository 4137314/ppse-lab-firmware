#include <Arduino.h>
#include "core/storage.h"
#include "core/system_manager.h"
#include "core/telemetry.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"

SystemDataPacket local_data;
uint32_t last_log_ms = 0;

// --- CORE 1: Acquisizione ad alta priorità ---
void setup1() {
    sys_manager_init();
    telemetry_init();
    sys_set_core1_ready(true);
}

void loop1() {
    // Aggiorna i sensori (IMU, GPS, Altitudine) alla massima velocità del bus
    telemetry_update();

    static uint32_t last_send_ms = 0;
    // Invia i dati al Core 0 ogni 100ms (10Hz è ottimo per telemetria standard)
    if (millis() - last_send_ms >= 100) {
        SystemDataPacket frame;
        telemetry_get_frame(&frame, sizeof(SystemDataPacket));
        
        sys_manager_send_data(&frame);
        last_send_ms = millis();
    }
    
    // Niente delay(250) qui: permette al modulo GPS di essere letto costantemente
    yield(); 
}

// --- CORE 0: Interfaccia Utente e Scrittura SD ---
void setup() {
    Serial.begin(115200); // Velocità aumentata per non rallentare il debug
    
    // Inizializzazione storage spostata nel Core 0 (gestore I/O)
    if (!storage_init()) {
        Serial.println("Critico: Errore Storage!");
    }

    while (!sys_is_core1_ready()) {
        delay(1);
    }

    display_hw_init();
    inputs_init();
    ui_manager_init();
}

void loop() {
    // 1. Gestione Input
    inputs_update();
    ButtonId btn = inputs_get_last_press();

    // 2. Ricezione dati dal Core 1
    if (sys_manager_receive_data(&local_data)) {
        
        // LOGGING: Spostato qui per non bloccare Core 1
        // Se il GPS ha il fix e sono passati 10 secondi
        if (local_data.gps_status > 0 && (millis() - last_log_ms >= 10000)) {
            
            // Verifichiamo che la SD non sia impegnata dal PC (USB Mass Storage)
            if (!storage_is_busy_by_usb()) {
                char log_buffer[128];
                snprintf(log_buffer, sizeof(log_buffer), "%lu,%.6f,%.6f,%.2f,%.1f,%u", 
                         millis(),
                         local_data.latitude, 
                         local_data.longitude, 
                         local_data.speed_ms, 
                         local_data.temp_c, 
                         local_data.satellites);

                storage_log_append(log_buffer);
                last_log_ms = millis();
            }
        }
    }

    // 3. Aggiornamento Interfaccia
    ui_manager_dispatch_input(btn);
    ui_manager_update(&local_data);

    delay(16); // Circa 60Hz per la UI
}
