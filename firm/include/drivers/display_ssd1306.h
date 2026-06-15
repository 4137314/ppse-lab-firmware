/**
 * @file display_ssd1306.h
 * @brief Driver di basso livello agnostico per il controllo dell'OLED SSD1306.
 * @details Questa interfaccia astrae le dipendenze da Adafruit, permettendo
 * alla logica superiore di operare indipendentemente dall'implementazione hardware.
 * @defgroup DisplayDriver Driver Display OLED
 * @{
 */

#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H

#include <stdbool.h>
#include <stdint.h>

/** * @name Costanti Fisiche Pannello
 * @{ 
 */
#define SCREEN_WIDTH   128  /**< Larghezza display in pixel. */
#define SCREEN_HEIGHT  64   /**< Altezza display in pixel. */
#define SCREEN_ADDRESS 0x3C /**< Indirizzo I2C standard SSD1306. */
/** @} */

/**
 * @brief Inizializza l'hardware I2C e il controller OLED.
 * @details Configura il bus I2C0 dell'RP2040 e alloca il buffer di memoria.
 * @return true se il display è stato trovato e inizializzato correttamente.
 */
bool display_hw_init(void);

/**
 * @brief Verifica se il display è fisicamente connesso al bus I2C.
 * @return true se il device risponde all'indirizzo SCREEN_ADDRESS.
 */
bool display_is_alive(void);

/**
 * @brief Pulisce il buffer interno (RAM) del display.
 * @details Riempie il buffer con pixel "spenti" (nero).
 */
void display_clear(void);

/**
 * @brief Invia il buffer corrente al display fisico (Refresh).
 * @details Questa è l'operazione più onerosa in termini di tempo sul bus I2C.
 */
void display_show(void);

/**
 * @brief Gestione energetica del pannello.
 * @param on true per attivare il display, false per metterlo in Sleep (consumo < 10uA).
 */
void display_set_power(bool on);

/**
 * @brief Regola il contrasto (luminosità) dei pixel OLED.
 * @param level Valore da 0 (minimo contrasto) a 255 (massimo).
 */
void display_set_brightness(uint8_t level);

/**
 * @brief Funzione primitiva di disegno agnostica.
 * @param x Coordinata orizzontale.
 * @param y Coordinata verticale.
 * @param color 1 per acceso, 0 per spento.
 */
void display_draw_pixel(int16_t x, int16_t y, uint16_t color);

/**
 * @brief Fornisce l'accesso al driver sottostante (uso avanzato).
 * @details Restituisce un puntatore generico (void*) per mantenere l'header 
 * agnostico da librerie specifiche (es. Adafruit GFX).
 * @note Il chiamante dovrà effettuare il cast esplicito a (Adafruit_SSD1306*) nel proprio file .cpp.
 * @return Puntatore all'oggetto driver hardware.
 */
void* get_display_driver(void);

/** @} */ // fine del gruppo DisplayDriver

#endif /* DISPLAY_SSD1306_H */
