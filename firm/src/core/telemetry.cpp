#include "core/telemetry.h"
#include "core/system_manager.h"
#include "drivers/config_pins.h"
#include "drivers/peripherals.h"
#include <Arduino.h>
#include <minmea.h>

static char nmea_buffer[128];
static uint8_t nmea_idx = 0;
static uint32_t last_fix_timestamp = 0;
static bool error_reported = false;
static bool gps_timeout_flagged = false;

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
    gps_hw_enable();
    Serial2.setTX(GPS_TX_PIN);
    Serial2.setRX(GPS_RX_PIN);
    Serial2.begin(GPSBAUD);
}

static void process_nmea(const char* line) {
    struct minmea_sentence_gga gga;
    if (!minmea_parse_gga(&gga, line)) return;
    if (gga.fix_quality <= 0) return;

    float lat = minmea_tocoord(&gga.latitude);
    float lon = minmea_tocoord(&gga.longitude);
    int sats  = gga.satellites_tracked;

    sys_manager_update_gps(lat, lon, true, sats);

    last_fix_timestamp = millis();
    gps_timeout_flagged = false;
    error_reported = false;
}

void telemetry_update() {
    if (!gps_timeout_flagged && last_fix_timestamp != 0 &&
        (millis() - last_fix_timestamp > 3000)) {
        sys_manager_set_gps_timeout();
        gps_timeout_flagged = true;
    }

    static uint32_t last_slow_update = 0;
    if (millis() - last_slow_update > 500) {
        sys_manager_update_health(peripherals_get_temperature());
        last_slow_update = millis();
    }

    if (!telemetry_is_healthy()) {
        if (!error_reported) {
            sys_manager_report_error(ERR_CAT_SENSORS, ERR_SENS_GPS_NO_DATA, false);
            error_reported = true;
        }
    }

    int read_count = 0;
    while (Serial2.available() && read_count < 64) {
        char c = (char)Serial2.read();
        read_count++;

        if (c == '$') nmea_idx = 0;
        if (nmea_idx < sizeof(nmea_buffer) - 1) nmea_buffer[nmea_idx++] = c;

        if (c == '\n') {
            nmea_buffer[nmea_idx] = '\0';
            if (nmea_idx > 5) {
                process_nmea(nmea_buffer);
            }
            nmea_idx = 0;
        }
    }
}

bool telemetry_is_healthy() {
    if (last_fix_timestamp == 0) return true;
    return (millis() - last_fix_timestamp) < 5000;
}