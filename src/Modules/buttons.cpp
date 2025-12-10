#include "buttons.h"
#include "display_ui.h"
#include "leds.h"
#include "gps_module.h"

// Variabili globali per menu
int menuIndex = 0;
bool menuOpen = true;      // Menu sempre visibile
bool inSubmenu = false;

// Debounce
static unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 120; // ms

// Numero di voci menu
const int MENU_LENGTH = 5;

// Inizializzazione dei pulsanti
bool buttonsInit() {
    const uint8_t pins[] = {SW_UP, SW_DOWN, SW_LEFT, SW_RIGHT};
    for (uint8_t i = 0; i < sizeof(pins); i++) {
        pinMode(pins[i], INPUT_PULLUP);
    }
    Serial.println(F("Buttons initialized"));
    return true;
}

// Funzione generica debounce e lettura pulsante
bool isButtonPressed(uint8_t buttonPin) {
    return digitalRead(buttonPin) == LOW; // active LOW
}

// Gestione dei pulsanti, da chiamare nel loop
void buttonsUpdate() {
    unsigned long now = millis();
    if (now - lastDebounceTime < DEBOUNCE_DELAY) return;
    lastDebounceTime = now;

    // --- Se siamo in un sottomenu ---
    if (inSubmenu) {
        if (isButtonPressed(SW_LEFT)) { // esci dal submenu
            inSubmenu = false;
            drawMenu(menuIndex);
        }
        return;
    }

    // --- Menu aperto ---
    if (isButtonPressed(SW_UP)) {           // scroll up
        menuIndex = (menuIndex == 0) ? MENU_LENGTH - 1 : menuIndex - 1;
        drawMenu(menuIndex);
        return;
    }

    if (isButtonPressed(SW_DOWN)) {         // scroll down
        menuIndex = (menuIndex + 1) % MENU_LENGTH;
        drawMenu(menuIndex);
        return;
    }

    if (isButtonPressed(SW_RIGHT)) {        // entra nel submenu
        inSubmenu = true;
        switch (menuIndex) {
            case 0: drawSettingsScreen(); break;
            case 1: drawSensorsScreen();  break;
            case 2: drawGPSScreen();      break;
            case 3: drawSystemScreen();   break;
            case 4: drawInfoScreen();     break;
        }
    }

    if (isButtonPressed(SW_LEFT)) {         // torni al menu (resta aperto)
        drawMenu(menuIndex);                 // utile se vuoi refresh manuale
    }
}
