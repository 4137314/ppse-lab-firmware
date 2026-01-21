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
}
// -------------------------
// Animazione iniziale LED
// -------------------------
void ledsShowInitAnimation() {
    for(int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Purple;
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
    leds[ledsNumber] = CRGB::Blue;
    if(ledsNumber+1 == 8) {
        leds[0] = CRGB::Blue;
    } else {
        leds[ledsNumber+1] = CRGB::Red;
    }
    FastLED.show();
    delay(200);
    leds[ledsNumber] = CRGB::Black;
    if(ledsNumber+1 == 8) {
        leds[0] = CRGB::Black;
    } else {
    leds[ledsNumber+1] = CRGB::Black;
    }
    FastLED.show();
}

// -------------------------
// Flash temporaneo due LED
// -------------------------
/*void ledsFlashPair(uint8_t a, uint8_t b, uint32_t color, uint16_t duration) {
  
}*/
