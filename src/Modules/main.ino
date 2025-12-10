// ==============================================================
// Project: RP2040 Menu UI with OLED and GPS
// Description: Handles menu, submenu, home screen, and GPS info
// Author / Date: //RELAZIONE 1 15-30pg, Anno mese giorno
// ==============================================================

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <Adafruit_NeoPixel.h>
#include <stdint.h>

#include "buzzer.h"
#include "buttons.h"
#include "display_ui.h"
#include "gps_module.h"
#include "leds.h"
#include "sensor.h"
#include "init.h"

// ========================
// Setup
// ========================
void setup() {
    initHardware();
}

// ========================
// Main Loop
// ========================
void loop() {
    // 1. Leggi pulsanti e gestisci menu/submenu
    buttonsUpdate();

    // 2. Aggiorna dati dai sensori
    //int tempRaw = readTempRaw();
    //float tempC = readTempC();
    //float tempCal = readTempCalibrated(); // opzionale: offset e scale dinamici

    // 3. Aggiorna dati GPS
    gpsUpdate();

        // 2. Aggiorna i dati sensori se siamo nello screen Sensors
    if (inSubmenu && menuIndex == 1) { // Sensors submenu
        drawSensorsScreen();  // ridisegna per aggiornare temperatura, pressione ecc.
    }

    // 3. Aggiorna i dati GPS se siamo nello screen GPS
    if (inSubmenu && menuIndex == 2) { // GPS submenu
        drawGPSScreen(); // ridisegna GPS info
    }

    // 4. Aggiorna display se necessario
    updateDisplayTimeout(); // funzione che puoi creare per ridisegnare schermo

    // 5. Aggiorna LED/feedback pulsanti
    //ledsUpdate();    // animazioni o stato LED

    // 6. Aggiorna buzzer (se ci sono segnali da emettere)
    //buzzerUpdate();

    // 7. Controlla timeout inattività per standby display
    //checkTimeout();

    delay(500);
}