#pragma once

#ifndef UNIT_TEST
#include <Arduino.h>

#include "pico/util/queue.h"
#else
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// --- Costanti Arduino ---
#define HIGH         0x1
#define LOW          0x0
#define INPUT        0x0
#define OUTPUT       0x1
#define INPUT_PULLUP 0x2

// La macro F() su PC non serve (non c'è memoria Flash separata)
#define F(s)         s

typedef uint8_t byte;
typedef bool boolean;

#ifdef __cplusplus
extern "C" {
#endif
inline uint32_t millis() {
    return 0;
}
inline void delay(uint32_t ms) {}
inline void pinMode(int pin, int mode) {}
inline int digitalRead(int pin) {
    return LOW;
}
inline void digitalWrite(int pin, int val) {}

typedef struct {
    int dummy;
} queue_t;
inline void queue_init(queue_t* q, size_t size, unsigned int depth) {}
inline bool queue_try_add(queue_t* q, const void* data) {
    return true;
}
inline bool queue_try_remove(queue_t* q, void* data) {
    return false;
}
inline bool queue_is_empty(queue_t* q) {
    return true;
}
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// --- Mock Serial ---
class MockSerial {
   public:
    void begin(long s) {}
    bool available() {
        return false;
    }
    int read() {
        return -1;
    }
    void print(const char* s) {
        printf("%s", s);
    }
    void print(int n) {
        printf("%d", n);
    }  // Gestisce print di numeri
    void println(const char* s) {
        printf("%s\n", s);
    }
    void printf(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
};
extern MockSerial Serial;
extern MockSerial Serial1;

// --- Mock Adafruit GFX & SSD1306 ---
#define WHITE         1
#define BLACK         0
#define SSD1306_WHITE 1
#define SSD1306_BLACK 0

class Adafruit_GFX {
   public:
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
    void setCursor(int16_t x, int16_t y) {}
    void setTextColor(uint16_t c) {}
    void setTextColor(uint16_t c, uint16_t bg) {}  // Supporto testo invertito
    void setTextSize(uint8_t s) {}

    // Gestione print e printf
    void print(const char* s) {}
    void print(int n) {}
    void println(const char* s) {}
    void printf(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }

    // Funzioni di disegno mancanti
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {}
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {}
};

class Adafruit_SSD1306 : public Adafruit_GFX {
   public:
    Adafruit_SSD1306(int w, int h, void* wire = NULL, int rst = -1) {}
    bool begin(uint8_t vcc, uint8_t addr) {
        return true;
    }
    void display() {}
    void clearDisplay() {}
    void drawPixel(int16_t x, int16_t y, uint16_t color) override {}
};
#endif
#endif
