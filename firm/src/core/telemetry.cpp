/**
 * @file telemetry.cpp
 * @brief Gestione acquisizione sensori e parsing GPS (Core 1).
 */

#include <Arduino.h>
#include <cstring>              // Necessario per memset
#include "core/telemetry.h"
#include "core/messages.h"       // Per la definizione di SystemDataPacket
#include "core/system_manager.h" // Per sys_manager_send_data
#include "drivers/peripherals.h" // Per peripherals_read_temperature
#include <minmea.h>

// Pacchetto di lavoro interno al Core 1
static SystemDataPacket local_frame;

void telemetry_init() {
    // Inizializza la memoria a zero per evitare dati spazzatura
    memset(&local_frame, 0, sizeof(SystemDataPacket));
    
    // Inizializzazione Serial1 per il modulo GPS (GPIO 0 e 1 su Pico)
    Serial1.begin(9600); 
}

void telemetry_update() {
    // 1. Leggi Temperatura dai Driver (campo temp_c in messages.h)
    local_frame.temp_c = peripherals_get_temperature();
    // 2. Leggi Uptime (campo uptime_s in messages.h)
    // Convertiamo millis() in secondi come previsto dalla struct
    local_frame.uptime_s = millis() / 1000;

    // 3. Logica GPS (Placeholder per il parser minmea)
    while (Serial1.available()) {
        char c = (char)Serial1.read();
        // Nota: Qui andrà inserita la logica di parsing minmea_parse_line
        (void)c; // Evita warning per variabile non usata
    }

    // 4. Invia il pacchetto aggiornato al System Manager (IPC)
    // Questo copia i dati nella coda per il Core 0 (UI)
    sys_manager_send_data(&local_frame);
}

/**
 * @brief Copia l'ultimo frame acquisito nel buffer fornito.
 * @details Questa firma (void*, size_t) è quella richiesta dal linker.
 */
bool telemetry_get_frame(void* dest_buffer, size_t size) {
    if (dest_buffer == NULL || size != sizeof(SystemDataPacket)) {
        return false;
    }
    memcpy(dest_buffer, &local_frame, sizeof(SystemDataPacket));
    return true;
}
