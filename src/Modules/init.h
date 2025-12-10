#ifndef INIT_H
#define INIT_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "buzzer.h"
#include "leds.h"
#include "sensor.h"
#include "display_ui.h"
#include "buttons.h"

// Pin esterni (puoi includere altri se vuoi)
#define UART0_TX_PIN 16
#define UART0_RX_PIN 17
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

// Inizializza tutte le periferiche: USB Serial, I2C, Buzzer, LED, OLED, GPS, sensori
bool initHardware();

#endif // INIT_H