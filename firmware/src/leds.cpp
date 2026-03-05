/**
 * @file leds.cpp
 * @brief Implementazione dei feedback visivi tramite LED RGB.
 * * Questo modulo utilizza la libreria FastLED per gestire una striscia di LED
 * RGB, fornendo segnali visivi per l'inizializzazione del sistema, la
 * navigazione nei menu e gli avvisi di inattività.
 */

#include "leds.h"

#include "buttons.h"
#include "display_ui.h"

/** * @brief Array globale che rappresenta lo stato dei colori della striscia
 * LED.
 */
CRGB leds[NUM_LEDS];

/**
 * @brief Inizializza l'hardware dei LED.
 * * Configura il controller FastLED utilizzando il tipo di LED, il pin e
 * l'ordine dei colori definiti in @ref leds.h.
 */
void ledsInit() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  return;
}

/**
 * @brief Esegue l'animazione di avvio del sistema.
 * * Accende sequenzialmente i LED con un colore rosa (LightPink) e li spegne
 * nello stesso ordine, creando un effetto di "caricamento" visivo.
 */
void ledsShowInitAnimation() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::LightPink;
    FastLED.show();
    delay(50);
  }
  delay(200);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
    FastLED.show();
    delay(50);
  }
}

/**
 * @brief Fornisce un feedback visivo durante la navigazione dei menu.
 * * Accende un LED Blu nella posizione corrente e un LED Rosso nella posizione
 * successiva, quindi esegue un rapido lampeggio per confermare il movimento
 * dell'indice. Gestisce automaticamente il rollover della striscia (se l'indice
 * raggiunge @ref NUM_LEDS).
 */
void ledsNavigationAnimation() {
  FastLED.clear();
  leds[ledsNumber] = CRGB::Blue;

  // Gestione rollover per striscia da 8 LED
  if (ledsNumber + 1 == 8) {
    leds[0] = CRGB::Red;
  } else {
    leds[ledsNumber + 1] = CRGB::Red;
  }

  FastLED.show();
  delay(200);
  FastLED.clear();
  FastLED.show();

  // Reset stato LED
  leds[ledsNumber] = CRGB::Black;
  if (ledsNumber + 1 == 8) {
    leds[0] = CRGB::Black;
  } else {
    leds[ledsNumber + 1] = CRGB::Black;
  }
  FastLED.show();
}

/**
 * @brief Esegue un flash verde per confermare un'azione (Toggle).
 * * Utilizzato tipicamente quando l'utente conferma una selezione o cambia
 * un'impostazione nel menu.
 */
void ledsToggleAnimation() {
  FastLED.clear();
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
  }
  FastLED.show();
  delay(200);
  FastLED.clear();
  FastLED.show();
}

/**
 * @brief Esegue un flash giallo per segnalare l'imminente timeout dello
 * schermo.
 * * Questa animazione viene richiamata dal timer di inattività prima che
 * il display torni alla schermata Home.
 */
void ledsInactivityAnimation() {
  FastLED.clear();
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Yellow;
  }
  FastLED.show();
  delay(200);
  FastLED.clear();
  FastLED.show();
}
