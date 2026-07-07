#include "core/telemetry.h"
#include "core/system_manager.h"
#include "drivers/config_pins.h"
#include "drivers/peripherals.h"
#include <Arduino.h>
#include <minmea.h>
#include <pico/sync.h>
#include "core/storage.h" // Oppure aggiungi la dichiarazione qui sotto:

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
    bool has_real_fix = false;
    float lat = 0.0f, lon = 0.0f;
    int sats = 0;

    struct minmea_sentence_gga gga;
    if (minmea_parse_gga(&gga, line)) {
        // Il fix_quality > 0 è l'unico vero segnale di posizione valida
        if (gga.fix_quality > 0) {
            lat = minmea_tocoord(&gga.latitude);
            lon = minmea_tocoord(&gga.longitude);
            sats = gga.satellites_tracked;
            has_real_fix = true;
        }
    }

    // Se abbiamo un fix, aggiorniamo e logghiamo
    if (has_real_fix) {
        uint32_t save = spin_lock_blocking(telemetry_lock);
        telemetry_data.latitude = lat;
        telemetry_data.longitude = lon;
        telemetry_data.gps_status = true;
        telemetry_data.satellites = sats;
        last_fix_timestamp = millis();
        spin_unlock(telemetry_lock, save);
    }
}

void telemetry_update() {
    // 1. LOGICA DI TIMEOUT: Se è passato troppo tempo dall'ultimo fix, azzera lo stato.
    // Questo garantisce che la UI e i LED tornino allo stato "Searching" correttamente.
    uint32_t save = spin_lock_blocking(telemetry_lock);
    if (telemetry_data.gps_status && (millis() - last_fix_timestamp > 3000)) {
        telemetry_data.gps_status = false;
        telemetry_data.satellites = 0;
        // Opzionale: potresti voler azzerare anche lat/lon qui se non vuoi mostrare l'ultima posizione nota
    }
    spin_unlock(telemetry_lock, save);

    // 2. Aggiornamento lento (Temp/Uptime) - Eseguito solo ogni 500ms
    static uint32_t last_slow_update = 0;
    if (millis() - last_slow_update > 500) {
        telemetry_data.temp_c = peripherals_get_temperature();
        telemetry_data.uptime_s = millis() / 1000;
        last_slow_update = millis();
    }

    // 3. Gestione errori (Controllo salute)
    if (!telemetry_is_healthy()) {
        if (!error_reported) {
            sys_manager_report_error(ERR_CAT_SENSORS, ERR_SENS_GPS_NO_DATA, false);
            error_reported = true;
        }
    }

    // 4. Processing rate-limited: max 64 byte a ciclo per evitare saturazione del bus
    int read_count = 0;
    while (Serial2.available() && read_count < 64) {
        char c = (char)Serial2.read();
        read_count++;
        
        if (c == '$') nmea_idx = 0;
        if (nmea_idx < sizeof(nmea_buffer) - 1) nmea_buffer[nmea_idx++] = c;
        
        if (c == '\n') {
            nmea_buffer[nmea_idx] = '\0';
            if (nmea_idx > 5) {
                Serial.print("RAW: "); Serial.print(nmea_buffer); // DEBUG - opzionale
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
    // Se non abbiamo ancora un fix, il sistema è "sano" (sta cercando)
    if (last_fix_timestamp == 0) return true; 
    // Se sono passati più di 5 secondi dall'ultimo fix, segnala errore
    return (millis() - last_fix_timestamp) < 5000; 
}

