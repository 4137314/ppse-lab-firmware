#include "leds.h"
#include "display_ui.h"
#include "buttons.h"

// RGB strip placeholder, verrà aggiornato in ledsInit()

// -------------------------
// Inizializzazione LED
// -------------------------
CRGB leds[NUM_LEDS];

void ledsInit() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS); //Fastled init
    return;
}
// -------------------------
// Animazione iniziale LED
// -------------------------
void ledsShowInitAnimation() {
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::LightPink;
        FastLED.show();
        delay(50);
    }
    delay(200);
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
        FastLED.show();
        delay(50);
    }
}


void ledsNavigationAnimation() {
    FastLED.clear();
    leds[ledsNumber] = CRGB::Blue;
    if(ledsNumber+1 == 8) {
        leds[0] = CRGB::Red;
    } else {
        leds[ledsNumber+1] = CRGB::Red;
    }
    FastLED.show();
    delay(200);
    FastLED.clear();
    FastLED.show();
    leds[ledsNumber] = CRGB::Black;
    if(ledsNumber+1 == 8) {
        leds[0] = CRGB::Black;
    } else {
    leds[ledsNumber+1] = CRGB::Black;
    }
    FastLED.show();
}

void ledsToggleAnimation() {
    FastLED.clear();
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Green;
    }
    FastLED.show();
    delay(200);
    FastLED.clear();
    FastLED.show();
}

void ledsInactivityAnimation() {
    FastLED.clear();
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Yellow;
    }
    FastLED.show();
    delay(200);
    FastLED.clear();
    FastLED.show();
}
// -------------------------
// Flash temporaneo due LED
// -------------------------

/*void ledsFlashPair(uint8_t a, uint8_t b, uint32_t color, uint16_t duration) {

}*/
