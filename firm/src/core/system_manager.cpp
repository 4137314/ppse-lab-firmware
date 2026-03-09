/**
 * @file system_manager.cpp
 * @brief Implementazione della comunicazione inter-core via Hardware FIFO.
 */

#include "core/system_manager.h"
#include "pico/multicore.h"
#include "hardware/irq.h"

// Buffer locale per l'ultimo pacchetto ricevuto (lato UI)
static SystemDataPacket current_packet;

void sys_manager_init() {
    // Svuota le FIFO per sicurezza all'avvio
    multicore_fifo_drain();
    
    // Inizializza il pacchetto con valori di default "safe"
    memset(&current_packet, 0, sizeof(SystemDataPacket));
    current_packet.gps_status = GPS_NO_FIX;
}

bool sys_manager_send_data(SystemDataPacket* packet) {
    /** * Spediamo il puntatore o i dati? 
     * Su RP2040, se i core condividono la RAM, possiamo passare 
     * un intero a 32-bit. Passiamo il pacchetto come copia rapida.
     */
    if (multicore_fifo_wready()) {
        // Inviamo i dati raggruppati (tecnica a blocchi o singola struct)
        // Per semplicità e sicurezza, usiamo la copia diretta se piccola.
        multicore_fifo_push_blocking((uint32_t)packet); 
        return true;
    }
    return false;
}

bool sys_manager_receive_data(SystemDataPacket* buffer) {
    // Controlla se il Core 1 ha inviato qualcosa
    if (multicore_fifo_rvalid()) {
        uint32_t raw_ptr = multicore_fifo_pop_blocking();
        SystemDataPacket* incoming = (SystemDataPacket*)raw_ptr;
        
        // Copia atomica dei dati nel buffer della UI
        memcpy(buffer, incoming, sizeof(SystemDataPacket));
        return true;
    }
    return false;
}
