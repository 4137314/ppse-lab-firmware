#include "core/telemetry.h"
#include "core/system_manager.h"
#include "drivers/config_pins.h"
#include "drivers/peripherals.h"
#include <Arduino.h>
#include <minmea.h>
#include <pico/sync.h>

static SystemDataPacket telemetry_data;
static spin_lock_t* telemetry_lock = NULL; // Inizializzato in telemetry_init

static char nmea_buffer[128];
static uint8_t nmea_idx = 0;
static uint32_t last_fix_timestamp = 0;
static bool error_reported = false; 

static void gps_hw_enable() {
    pinMode(SAM_EN_PIN, OUTPUT);
    digitalWrite(SAM_EN_PIN, LOW); 
    
    pinMode(SAM_RST_PIN, OUTPUT);
    digitalWrite(SAM_RST_PIN, LOW);
    delay(100);
    digitalWrite(SAM_RST_PIN, HIGH); 
    
    delay(1000); 
}

void telemetry_init() {
    // Inizializzazione sicura del lock
    if (telemetry_lock == NULL) {
        telemetry_lock = spin_lock_instance(spin_lock_claim_unused(true));
    }
    
    memset(&telemetry_data, 0, sizeof(SystemDataPacket));
    gps_hw_enable();
    
    Serial2.setTX(GPS_TX_PIN);
    Serial2.setRX(GPS_RX_PIN);
    Serial2.begin(GPSBAUD);
}

static void process_nmea(const char* line) {
    struct minmea_sentence_gga gga;
    if (minmea_parse_gga(&gga, line)) {
        uint32_t save = spin_lock_blocking(telemetry_lock);
        telemetry_data.latitude = minmea_tocoord(&gga.latitude);
        telemetry_data.longitude = minmea_tocoord(&gga.longitude);
        telemetry_data.satellites = gga.satellites_tracked;
        telemetry_data.gps_status = (gga.fix_quality > 0);
        if (telemetry_data.gps_status) last_fix_timestamp = millis();
        spin_unlock(telemetry_lock, save);
        return;
    }

    struct minmea_sentence_rmc rmc;
    if (minmea_parse_rmc(&rmc, line)) {
        if (rmc.valid) {
             uint32_t save = spin_lock_blocking(telemetry_lock);
             telemetry_data.gps_status = true;
             telemetry_data.latitude = minmea_tocoord(&rmc.latitude);
             telemetry_data.longitude = minmea_tocoord(&rmc.longitude);
             last_fix_timestamp = millis();
             spin_unlock(telemetry_lock, save);
        }
    }
}

void telemetry_update() {
    // Aggiornamento lento per non bloccare il loop
    static uint32_t last_slow_update = 0;
    if (millis() - last_slow_update > 500) {
        telemetry_data.temp_c = peripherals_get_temperature();
        telemetry_data.uptime_s = millis() / 1000;
        last_slow_update = millis();
    }

    if (!telemetry_is_healthy()) {
        if (!error_reported) {
            sys_manager_report_error(ERR_CAT_SENSORS, ERR_SENS_GPS_NO_DATA, false);
            error_reported = true;
        }
    }

    // Processing rate-limited: max 64 byte a ciclo per evitare corruzione
    int read_count = 0;
    while (Serial2.available() && read_count < 64) {
        char c = (char)Serial2.read();
        read_count++;
        
        if (c == '$') nmea_idx = 0;
        if (nmea_idx < sizeof(nmea_buffer) - 1) nmea_buffer[nmea_idx++] = c;
        
        if (c == '\n') {
            nmea_buffer[nmea_idx] = '\0';
            if (nmea_idx > 5) {
                Serial.print("RAW: "); Serial.print(nmea_buffer); // DEBUG
                process_nmea(nmea_buffer);
            }
            nmea_idx = 0;
        }
    }
}

bool telemetry_get_frame(SystemDataPacket* dest) {
    if (!dest || !telemetry_lock) return false;
    uint32_t save = spin_lock_blocking(telemetry_lock);
    *dest = telemetry_data;
    spin_unlock(telemetry_lock, save);
    return true;
}

bool telemetry_is_healthy() {
    if (last_fix_timestamp == 0) return (millis() < 60000);
    return (millis() - last_fix_timestamp) < 30000;
}
