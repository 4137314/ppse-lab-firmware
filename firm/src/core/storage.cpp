#include "core/storage.h"
#include "core/system_manager.h" // Per segnalare errori al bus
#include <LittleFS.h>
#include <Arduino.h>

static bool driveConnected = false;
static const char* LOG_FILENAME    = "/telemetry_log.csv";
static const char* CONFIG_FILENAME = "/config.bin";

bool storage_init(void) {
    if (!LittleFS.begin()) {
        if (!LittleFS.format()) {
            sys_manager_report_error(ERR_CAT_STORAGE, ERR_STG_FS_CORRUPT, true);
            return false;
        }
        if (!LittleFS.begin()) return false;
    }

    if (!LittleFS.exists(LOG_FILENAME)) {
        File f = LittleFS.open(LOG_FILENAME, "w");
        if (f) {
            f.println("timestamp,lat,lon,speed,temp,satellites");
            f.close();
        } else {
            sys_manager_report_error(ERR_CAT_STORAGE, ERR_STG_WRITE_FAIL, false);
        }
    }
    return true;
}

// Logging Asincrono: ora segnala fallimenti al sistema
void storage_log_append(const char* entry) {
    if (driveConnected) return;

    File f = LittleFS.open(LOG_FILENAME, "a");
    if (f) {
        f.println(entry);
        f.close(); 
    } else {
        // Segnala l'errore al bus di sistema, che lo passerà alla UI
        sys_manager_report_error(ERR_CAT_STORAGE, ERR_STG_WRITE_FAIL, false);
    }
}

bool storage_save_params(const void* data, size_t size) {
    if (driveConnected) return false;

    File f = LittleFS.open(CONFIG_FILENAME, "w");
    if (f) {
        size_t written = f.write((const uint8_t*)data, size);
        f.close();
        if (written != size) {
            sys_manager_report_error(ERR_CAT_STORAGE, ERR_STG_WRITE_FAIL, true);
            return false;
        }
        return true;
    }
    sys_manager_report_error(ERR_CAT_STORAGE, ERR_STG_WRITE_FAIL, true);
    return false;
}

bool storage_load_params(void* data, size_t size) {
    File f = LittleFS.open(CONFIG_FILENAME, "r");
    if (f) {
        size_t read = f.read((uint8_t*)data, size);
        f.close();
        return (read == size);
    }
    return false;
}

bool storage_is_busy_by_usb(void) {
    return driveConnected;
}
