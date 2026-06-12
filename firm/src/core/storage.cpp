#include "core/storage.h"
#include <LittleFS.h>
#include <Arduino.h>

// Variabili di stato globali al modulo
static bool driveConnected = false;
static bool inPrinting     = false;

static const char* LOG_FILENAME    = "/telemetry_log.csv";
static const char* CONFIG_FILENAME = "/config.bin";

bool storage_init(void) {
    // Inizializza LittleFS
    if (!LittleFS.begin()) {
        // Tentativo di formattazione se il FS è corrotto o non inizializzato
        if (!LittleFS.format()) return false;
        if (!LittleFS.begin()) return false;
    }

    // Crea il file di log con header se non esiste
    if (!LittleFS.exists(LOG_FILENAME)) {
        File f = LittleFS.open(LOG_FILENAME, "w");
        if (f) {
            f.println("timestamp,lat,lon,speed,temp,satellites");
            f.close();
        }
    }
    return true;
}

void storage_log_append(const char* entry) {
    if (driveConnected || inPrinting) return;

    // "a" apre in modalità append (il cursore viene posizionato alla fine)
    File f = LittleFS.open(LOG_FILENAME, "a");
    if (f) {
        f.println(entry);
        f.close(); 
    }
}

void storage_sync(void) {
    // LittleFS gestisce la consistenza dei dati alla chiusura del file
}

bool storage_read_line_with_tag(const char* filename, const char* tag, char* buffer, size_t len) {
    if (driveConnected) return false;

    File f = LittleFS.open(filename, "r");
    if (!f) return false;

    bool found = false;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        if (line.startsWith(tag)) {
            strncpy(buffer, line.c_str(), len);
            found = true;
            break;
        }
    }
    f.close();
    return found;
}

bool storage_save_params(const void* data, size_t size) {
    if (driveConnected || inPrinting) return false;

    File f = LittleFS.open(CONFIG_FILENAME, "w");
    if (f) {
        size_t written = f.write((const uint8_t*)data, size);
        f.close();
        return (written == size);
    }
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
