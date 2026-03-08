/**
 * @file leds.h
 * @brief Definizioni hardware e prototipi per la gestione dei LED RGB.
 * * Questo header contiene le macro di configurazione per la striscia LED
 * (WS2812B) e i prototipi delle funzioni di animazione utilizzate per il
 * feedback utente. La gestione fisica dei LED è affidata alla libreria FastLED.
 */

#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>
#include <FastLED.h>

/** @name Configurazione Striscia LED
 * @{ */
#define LED_PIN                                                                    \
    13                   /**< Pin GPIO dedicato alla trasmissione dati (Data Pin). \
                          */
#define NUM_LEDS 8       /**< Numero totale di LED presenti nella striscia. */
#define LED_TYPE WS2812B /**< Tipo di controller integrato nei LED (es. WS2812B, SK6812). */
#define COLOR_ORDER                                                   \
    GRB /**< Ordine dei canali colore (Green-Red-Blue) per il chipset \
           utilizzato. */
/** @} */

/**
 * @brief Inizializza il controller FastLED con i parametri definiti nelle
 * macro.
 */
void ledsInit();

/**
 * @brief Esegue una sequenza di accensione e spegnimento iniziale (Pink fade).
 */
void ledsShowInitAnimation();

/**
 * @brief Esegue un lampeggio Blu/Rosso per confermare lo spostamento nel menu.
 */
void ledsNavigationAnimation();

/**
 * @brief Esegue un flash Verde per confermare una selezione o un cambio stato.
 */
void ledsToggleAnimation();

/**
 * @brief Esegue un flash Giallo per segnalare il ritorno alla Home per
 * inattività.
 */
void ledsInactivityAnimation();

#endif  // LEDS_H
