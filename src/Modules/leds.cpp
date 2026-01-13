#include "leds.h"
#include "display_ui.h"

// RGB strip placeholder, verrà aggiornato in ledsInit()

// -------------------------
// Inizializzazione LED
// -------------------------
CRGB leds[NUM_LEDS];

bool ledsInit() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS); //Fastled init
}
// -------------------------
// Animazione iniziale LED
// -------------------------
void ledsShowInitAnimation() {

}

// -------------------------
// Flash temporaneo due LED
// -------------------------
void ledsFlashPair(uint8_t a, uint8_t b, uint32_t color, uint16_t duration) {
  
}
