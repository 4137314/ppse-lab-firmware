#include "core/telemetry.h"
#include "core/system_manager.h" // Per sys_manager_report_error
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

static void gps_hw_enable() {
    pinMode(SAM_EN_PIN, OUTPUT);
    digitalWrite(SAM_EN_PIN, LOW); // PMOS: LOW = ON
    delay(500);
}

void telemetry_init() {
    memset(&telemetry_data, 0, sizeof(SystemDataPacket));
    gps_hw_enable();
    
    Serial1.setTX(GPS_TX_PIN);
    Serial1.setRX(GPS_RX_PIN);
    Serial1.begin(9600);
}

static void process_nmea(const char* line) {
    struct minmea_sentence_gga gga;
    
    // Lock per scrittura atomica
    uint32_t save = spin_lock_blocking(telemetry_lock);
    
    if (minmea_parse_gga(&gga, line)) {
        telemetry_data.latitude = minmea_tocoord(&gga.latitude);
        telemetry_data.longitude = minmea_tocoord(&gga.longitude);
        telemetry_data.satellites = gga.satellites_tracked;
        telemetry_data.gps_status = (gga.fix_quality > 0);
        
        if (telemetry_data.gps_status) {
            last_fix_timestamp = millis();
            telemetry_data.flags.error_active = 0; // Clear errore GPS
        } else {
            // Segnala degrado segnale
            telemetry_data.last_error.category = ERR_CAT_SENSORS;
            telemetry_data.last_error.code = ERR_SENS_GPS_BAD_FIX;
            telemetry_data.flags.error_active = 1;
        }
    }
    
    spin_unlock(telemetry_lock, save);
}

void telemetry_update() {
    telemetry_data.temp_c = peripherals_get_temperature();
    telemetry_data.uptime_s = millis() / 1000;

    // Check salute (Watchdog software)
    if (!telemetry_is_healthy()) {
        uint32_t save = spin_lock_blocking(telemetry_lock);
        telemetry_data.last_error = {ERR_CAT_SENSORS, ERR_SENS_GPS_NO_DATA, millis(), false};
        telemetry_data.flags.error_active = 1;
        spin_unlock(telemetry_lock, save);
    }

    while (Serial1.available()) {
        char c = (char)Serial1.read();
        if (c == '$') nmea_idx = 0;
        if (nmea_idx < sizeof(nmea_buffer) - 1) nmea_buffer[nmea_idx++] = c;
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
    return (millis() - last_fix_timestamp) < 30000;
}
