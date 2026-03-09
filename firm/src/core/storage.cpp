/**
 * @file storage.cpp
 * @brief Gestione del filesystem per logging CSV e salvataggio impostazioni.
 */

#include "core/storage.h"
#include <LittleFS.h> // O <SD.h> se usi una scheda SD fisica

// Nome del file per i log di telemetria
const char* LOG_FILENAME = "/telemetry_log.csv";
const char* CONFIG_FILENAME = "/config.bin";

bool storage_init() {
    // Inizializza il filesystem LittleFS (memoria Flash interna)
    if (!LittleFS.begin()) {
        Serial.println("Critico: Errore inizializzazione LittleFS!");
        return false;
    }
    
    // Crea l'header del file CSV se il file non esiste
    if (!LittleFS.exists(LOG_FILENAME)) {
        File logFile = LittleFS.open(LOG_FILENAME, "w");
        if (logFile) {
            logFile.println("timestamp,lat,lon,speed,temp,satellites");
            logFile.close();
        }
    }
    return true;
}

void storage_log_append(const char* entry) {
    // Apertura in modalità "append"
    File logFile = LittleFS.open(LOG_FILENAME, "a");
    if (logFile) {
        logFile.println(entry);
        logFile.close();
    }
}

void storage_save_config(void* data, size_t size) {
    File configFile = LittleFS.open(CONFIG_FILENAME, "w");
    if (configFile) {
        configFile.write((uint8_t*)data, size);
        configFile.close();
    }
}

bool storage_load_config(void* data, size_t size) {
    if (!LittleFS.exists(CONFIG_FILENAME)) return false;

    File configFile = LittleFS.open(CONFIG_FILENAME, "r");
    if (configFile) {
        size_t readSize = configFile.read((uint8_t*)data, size);
        configFile.close();
        return (readSize == size);
    }
    return false;
}
