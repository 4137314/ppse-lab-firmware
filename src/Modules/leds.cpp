#include "leds.h"
#include "display_ui.h"

// RGB strip placeholder, verrà aggiornato in ledsInit()
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// -------------------------
// Inizializzazione LED
// -------------------------
bool ledsInit() {
    bool ok = true;

    // Inizializza striscia
    strip.begin();
    strip.clear();
    strip.show();

    // Imposta brightness iniziale
    strip.setBrightness(10);
    strip.show();


    if (strip.getBrightness() == 0) {
        Serial.println(F("LED ERROR: brightness test failed"));
        ok = false;
    } else {
        Serial.println(F("LED OK"));
    }

    // Animazione iniziale solo se LED OK
    if (ok) {
        ledsShowInitAnimation();
    }

    return ok;
}

// -------------------------
// Animazione iniziale LED
// -------------------------
void ledsShowInitAnimation() {

    // Accensione simmetrica
    for (int i = 0; i < strip.numPixels()/2; i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 0));
        strip.setPixelColor(strip.numPixels()-1-i, strip.Color(255, 0, 0));
        strip.show();
        delay(120);
    }

    // Transizione rosso → blu
    for (int step = 0; step <= 100; step++) {
        uint8_t r = 255 - (step * 255 / 100);
        uint8_t g = 0;
        uint8_t b = (step * 255 / 100);
        for (int i = 0; i < strip.numPixels(); i++)
            strip.setPixelColor(i, strip.Color(r,g,b));
        strip.show();
        delay(5); // leggermente più lento per visibilità
    }

    // Spegne tutti
    strip.clear();
    strip.show();
}

// -------------------------
// Flash temporaneo due LED
// -------------------------
void ledsFlashPair(uint8_t a, uint8_t b, uint32_t color, uint16_t duration) {
    strip.setPixelColor(a, color);
    strip.setPixelColor(b, color);
    strip.show();
    delay(duration);
    strip.setPixelColor(a, 0);
    strip.setPixelColor(b, 0);
    strip.show();
}
