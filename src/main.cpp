// Se la macro non viene iniettata dal Makefile/Compiler, 
// allora definiscila come 0 (Safe default)
#ifndef DEBUG
  #define DEBUG 0
#endif

// Se vuoi automatizzare il comportamento del Beep nel codice
#ifndef BUZZER_INIT_BEEP
  #define BUZZER_INIT_BEEP 1
#endif

#include "Modules/Functions.h"
#include "pico/stdlib.h" 

// Variabili per timing non bloccante
unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 1000; // Leggi i sensori ogni secondo

void setup() {
    // Inizializzazione seriale per Debug (se connesso a PC)
    #if DEBUG == 1
    Serial.begin(115200);
    // Attendi la connessione seriale solo per un breve timeout
    unsigned long start = millis();
    while (!Serial && (millis() - start) < 2000); 
    Serial.println("System Initializing...");
    #endif

    // Inizializzazione Filesystem Fail-Safe
    littleFS_Setup();

    // Inizializzazione Hardware (I2C, GNSS, Sensori)
    if (!init_All()) {
        #if DEBUG == 1
        Serial.println("CRITICAL ERROR: Hardware initialization failed!");
        #endif
        // Segnalazione errore visiva o blocco di sicurezza
        exit(1);
    }

    #if DEBUG == 1
    Serial.println("System Ready. High-Frequency Logging Active.");
    #endif
}

void loop() {
    // 1. Gestione GNSS (Non bloccante)
    // GPS_sync() viene chiamato a ogni iterazione per processare i caratteri UART in arrivo
    if (GPS_sync()) {
        #if DEBUG == 1
        Serial.println("Valid GNSS Sentence Received. Recording...");
        #endif
        
        // Logga i dati geografici indissolubilmente legati al timestamp
        log_gps_data(); 
    }

    // 2. Campionamento Sensori Ambientali (Timing controllato)
    unsigned long currentMillis = millis();
    if (currentMillis - lastSensorRead >= sensorInterval) {
        lastSensorRead = currentMillis;
        
        // Leggi Accelerometro e Temperatura
        update_Environmental_Data(); 
        
        #if DEBUG == 1
        Serial.println("Environmental Data Updated.");
        #endif
    }

    // 3. Gestione Interfaccia Utente
    // Gestisce gli stati dei pulsanti (precedentemente catturati dagli interrupt)
    buttonsUpdate();

    // 4. Update Display
    // Aggiorna l'OLED con i dati correnti
    update_Display();

    // NOTA: Abbiamo rimosso il delay(500). 
    // Il loop deve girare alla massima velocità possibile per svuotare i buffer UART.
}
