/**
 * @file display_ui.h
 * @brief Definizioni hardware e prototipi per l'interfaccia utente OLED.
 * * Questo header definisce la configurazione del display SSD1306 (pin,
 * dimensioni, indirizzi) e fornisce l'interfaccia per il rendering delle
 * diverse schermate applicative.
 */

#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "buttons.h"
#include "gps.h"
#include "leds.h"
#include "temp.h"

/** @name Configurazione Hardware Display
 * @{ */
#define DISPLAY_SDA_I2C_PIN 0 /**< Pin GPIO per segnale I2C SDA. */
#define DISPLAY_SCL_I2C_PIN 1 /**< Pin GPIO per segnale I2C SCL. */
#define SCREEN_WIDTH 128      /**< Larghezza del display OLED in pixel. */
#define SCREEN_HEIGHT 64      /**< Altezza del display OLED in pixel. */
#define OLED_RESET -1 /**< Pin di reset (non utilizzato/non disponibile). */
#define SCREEN_ADDRESS 0x3C /**< Indirizzo I2C standard per display SSD1306. \
                             */
/** @} */

/** @name Configurazione Estetica e Navigazione
 * @{ */
extern uint8_t ScreenStyle;     /**< Stile visivo: 1 = Semplice (testo), 0 =
                                   Highlight (barra invertita). */
extern const char* menuItems[]; /**< Array di stringhe contenente i nomi delle
                                   voci del menu. */
extern const int menuLength;    /**< Numero totale di elementi nel menu. */
extern struct parsed_nmea gpsData; /**< Struttura globale contenente i dati
                                      telemetrici GPS correnti. */
/** @} */

/** @name Indici di Navigazione Meteo
 * @{ */
extern uint8_t
    gpsDayIndex; /**< Offset del giorno selezionato per il forecast (0-6). */
extern uint8_t
    gpsHourIndex; /**< Offset dell'ora selezionata per il forecast (0-23). */
/** @} */

/* --- Funzioni di Ciclo Vita --- */

/**
 * @brief Inizializza il bus I2C e il controller SSD1306.
 * @return true Se il display è stato allocato correttamente, false in caso di
 * errore.
 */
bool displayInit();

/**
 * @brief Gestisce il timeout dell'inattività e il ritorno automatico alla Home.
 */
void updateDisplayTimeout();

/**
 * @brief Regola l'intensità luminosa (contrasto) del pannello OLED.
 * @param level Valore di contrasto da 0 (min) a 255 (max).
 */
void setBrightness(uint8_t level);

/* --- Funzioni di Rendering (Schermate) --- */

/**
 * @brief Renderizza la schermata Home con ora, data e meteo corrente.
 */
void drawHomeScreen();

/**
 * @brief Disegna il menu principale con lo stile selezionato in @ref
 * ScreenStyle.
 * @param index Indice della voce di menu da evidenziare.
 */
void drawMenu(int index);

/**
 * @brief Renderizza forzatamente il menu con stile evidenziato (testo
 * invertito).
 * @param index Indice della voce di menu da evidenziare.
 */
void drawMenu_evi(int index);

/**
 * @brief Mostra una schermata di segnaposto per le voci selezionate.
 * @param title Titolo o nome della sezione da visualizzare.
 */
void drawSelected(const char* title);

/** @brief Renderizza la schermata di configurazione impostazioni. */
void drawSettingsScreen();

/** @brief Renderizza la schermata dettagliata delle previsioni meteo. */
void drawMeteoScreen();

/** @brief Renderizza la schermata di stato del sistema. */
void drawSystemScreen();

/** @brief Renderizza la schermata crediti e informazioni. */
void drawInfoScreen();

/** @brief Renderizza la schermata dei dati GPS in tempo reale. */
void drawGPSScreen();

#endif  // DISPLAY_UI_H
