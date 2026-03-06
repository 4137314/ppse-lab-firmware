/**
 * @file filesystem.cpp
 * @brief Gestione del File System FatFS e interfaccia USB Mass Storage.
 * * Questo modulo gestisce l'inizializzazione della memoria flash interna
 * tramite FatFS e la sincronizzazione con l'host USB, garantendo l'accesso
 * esclusivo al filesystem per evitare corruzione dei dati.
 */

#include "filesystem.h"

/** @brief Flag di notifica aggiornamento stato filesystem. */
volatile bool updated = false;

/** @brief Stato della connessione USB (true se montato su PC). */
volatile bool driveConnected = false;

/** @brief Flag di semaforo per l'accesso al filesystem (evita race condition
 * con USB). */
volatile bool inPrinting = false;

/**
 * @brief Inizializza il filesystem FatFS sulla memoria Flash.
 * * Tenta il mount del filesystem. Se fallisce, procede alla formattazione
 * automatica. Abilita le funzioni POSIX tramite VFS per le operazioni di I/O
 * standard.
 * * @return true se l'inizializzazione ha successo.
 * @note Blocca l'esecuzione (while 1) in caso di errore critico hardware.
 */
bool FatFS_Setup() {
    inPrinting = true;
    if (!FatFS.begin()) {
        Serial.println("FatFS first mount failed. Formatting...");
        FatFS.format();
        if (!FatFS.begin()) {
            Serial.println("FATAL: FatFS format/mount failed!");
            while (1) delay(10);
        }
    }
    inPrinting = false;
    Serial.println("Fat filesystem started");
    VFS.root(FatFS);  // Enables POSIX functions for I/O
    return true;
}

/**
 * @brief Configura l'interfaccia USB Mass Storage.
 * * Associa i callback per gli eventi di plug, unplug e controllo di
 * mountability per la condivisione della memoria flash via USB.
 * * @return true se l'inizializzazione del dispositivo USB ha successo.
 */
bool FatFSUSB_Setup() {
    // Set up callbacks
    FatFSUSB.onUnplug(unplug);
    FatFSUSB.onPlug(plug);
    FatFSUSB.driveReady(mountable);

    if (!FatFSUSB.begin()) {
        Serial.println("FATAL failed to start USB Mass Storage");
        while (1) delay(10);
    }
    delay(3000);
    if (Serial)
        Serial.println("USB Mass Storage started");

    return true;
}

/**
 * @brief Callback eseguito alla disconnessione USB.
 * * @param i Parametro di istanza (inutilizzato).
 */
void unplug(uint32_t i) {
    (void)i;
    driveConnected = false;
    updated        = true;
}

/**
 * @brief Callback eseguito alla connessione USB.
 * * Termina l'istanza locale di FatFS per cedere il controllo esclusivo
 * del disco all'host USB (PC).
 * * @param i Parametro di istanza (inutilizzato).
 */
void plug(uint32_t i) {
    (void)i;
    driveConnected = true;
    FatFS.end();
}

/**
 * @brief Verifica se il filesystem è pronto per essere montato dall'host USB.
 * * @param i Parametro di istanza (inutilizzato).
 * @return true se non ci sono operazioni di scrittura/lettura locali in corso.
 */
bool mountable(uint32_t i) {
    (void)i;
    return !inPrinting;
}

/**
 * @brief Sincronizza lo stato del filesystem e gestisce la rimonta dopo l'uso
 * USB.
 * * Se il dispositivo è stato scollegato dal PC, riesegue il setup di FatFS
 * per rendere nuovamente disponibili i file al firmware.
 * * @return true se la sincronizzazione avviene correttamente.
 */
bool sync_files() {
    bool ret = true;

    if (updated && !driveConnected) {
        Serial.println("\n\nDisconnected, new file listing:");
        FatFS.end();
        ret     = FatFS_Setup();
        updated = false;
    }
    return ret;
}
