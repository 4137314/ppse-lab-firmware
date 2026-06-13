#include "core/telemetry.h"
#include "core/system_manager.h"
#include "drivers/config_pins.h"
#include "drivers/peripherals.h"
#include <Arduino.h>
#include <minmea.h>
#include <pico/sync.h>

static SystemDataPacket telemetry_data;
static spin_lock_t* telemetry_lock = spin_lock_init(spin_lock_claim_unused(true));

static char nmea_buffer[128];
static uint8_t nmea_idx = 0;
static uint32_t last_fix_timestamp = 0;
static bool error_reported = false; 

static void gps_hw_enable() {
    // Gestione EN (PMOS attivo basso)
    pinMode(SAM_EN_PIN, OUTPUT);
    digitalWrite(SAM_EN_PIN, LOW); 
    
    // Gestione RST (Reset attivo basso: HIGH = operativo)
    pinMode(SAM_RST_PIN, OUTPUT);
    digitalWrite(SAM_RST_PIN, LOW);
    delay(100);
    digitalWrite(SAM_RST_PIN, HIGH); // Rilascio reset
    
    delay(1000); // Attesa stabilizzazione hardware modulo
}

void telemetry_init() {
    memset(&telemetry_data, 0, sizeof(SystemDataPacket));
    gps_hw_enable();
    
    // Inizializzazione UART1 corretta
    Serial2.setTX(GPS_TX_PIN);
    Serial2.setRX(GPS_RX_PIN);
    Serial2.begin(GPSBAUD);
}

static void process_nmea(const char* line) {
    struct minmea_sentence_gga gga;
    
    // Tentativo di parsing della frase GGA
    if (minmea_parse_gga(&gga, line)) {
        uint32_t save = spin_lock_blocking(telemetry_lock);
        
        telemetry_data.latitude = minmea_tocoord(&gga.latitude);
        telemetry_data.longitude = minmea_tocoord(&gga.longitude);
        telemetry_data.satellites = gga.satellites_tracked;
        telemetry_data.gps_status = (gga.fix_quality > 0);
        
        if (telemetry_data.gps_status) {
            last_fix_timestamp = millis();
            telemetry_data.flags.error_active = 0;
            error_reported = false; 
        }
        
        spin_unlock(telemetry_lock, save);
    }
}

void telemetry_update() {
    telemetry_data.temp_c = peripherals_get_temperature();
    telemetry_data.uptime_s = millis() / 1000;

    // Check salute: segnalazione errore se non abbiamo ricevuto un fix recente
    if (!telemetry_is_healthy()) {
        if (!error_reported) {
            sys_manager_report_error(ERR_CAT_SENSORS, ERR_SENS_GPS_NO_DATA, false);
            error_reported = true;
        }
    }

    // Processing del buffer UART
    while (Serial2.available()) {
        char c = (char)Serial2.read();
        
        // --- DIAGNOSTICA ATTIVA ---
        // Se non vedi FIX, decommenta la riga sotto per vedere i dati grezzi.
        // Se vedi caratteri illeggibili, cambia il GPSBAUD in config_pins.h
        // Serial.write(c); 

        if (c == '$') nmea_idx = 0;
        
        if (nmea_idx < sizeof(nmea_buffer) - 1) {
            nmea_buffer[nmea_idx++] = c;
        }
        
        if (c == '\n') {
            nmea_buffer[nmea_idx] = '\0';
            if (nmea_idx > 0) process_nmea(nmea_buffer);
            nmea_idx = 0;
        }
    }
}

bool telemetry_get_frame(SystemDataPacket* dest) {
    if (!dest) return false;
    uint32_t save = spin_lock_blocking(telemetry_lock);
    *dest = telemetry_data;
    spin_unlock(telemetry_lock, save);
    return true;
}

bool telemetry_is_healthy() {
    // Al primo avvio, tolleranza di 60s, poi 30s di timeout per il fix
    if (last_fix_timestamp == 0) return (millis() < 60000);
    return (millis() - last_fix_timestamp) < 30000;
}
