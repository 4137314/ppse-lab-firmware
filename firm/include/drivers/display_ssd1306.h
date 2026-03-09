/**
 * @file display_ssd1306.h
 * @brief Driver di basso livello per il controllo del display OLED SSD1306.
 * @details Gestisce esclusivamente l'inizializzazione, il refresh fisico 
 * e l'accesso al buffer grafico, senza logica di interfaccia utente.
 */

#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "config_pins.h"

/** @name Costanti Fisiche Pannello
 * @{ */
#define SCREEN_WIDTH     128    /**< Larghezza display in pixel. */
#define SCREEN_HEIGHT     64    /**< Altezza display in pixel. */
#define OLED_RESET        -1    /**< Reset pin (non usato su RP2040). */
#define SCREEN_ADDRESS  0x3C    /**< Indirizzo I2C standard SSD1306. */
/** @} */

/**
 * @brief Inizializza l'hardware I2C e il controller OLED.
 * @return true se l'allocazione del buffer ha avuto successo.
 */
bool display_hw_init();

/**
 * @brief Pulisce il buffer interno del display.
 */
void display_clear();

/**
 * @brief Invia il buffer corrente al display fisico (Refresh).
 */
void display_show();

/**
 * @brief Regola l'intensità luminosa del pannello.
 * @param level Valore da 0 (spento) a 255 (max).
 */
void display_set_brightness(uint8_t level);

/**
 * @brief Fornisce l'accesso diretto all'oggetto Adafruit per le View.
 * @return Puntatore all'istanza globale del driver SSD1306.
 */
Adafruit_SSD1306* get_display_driver();

#endif /* DISPLAY_SSD1306_H */
