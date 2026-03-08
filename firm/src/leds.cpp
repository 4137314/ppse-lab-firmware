#include "leds.h"

#include "buttons.h"
#include "display_ui.h"

CRGB leds[NUM_LEDS];

void ledsInit() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    return;
}

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
