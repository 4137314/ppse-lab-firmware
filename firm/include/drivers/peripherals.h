/**
 * @file peripherals.h
 * @brief Driver unificato per feedback (LED, Buzzer) e sensori ambientali.
 * @details Gestisce l'astrazione di LED RGB, feedback acustico e acquisizione
 * termica analogica, isolando le librerie hardware dal resto del sistema.
 */

#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <Arduino.h>
#include "config_pins.h"

/** @name Parametri ADC (RP2040)
 * @{ */
#define ADC_RESOLUTION      12      /**< Risoluzione nativa ADC RP2040. */
#define ADC_MAX_VALUE     4095      /**< Valore massimo (2^12 - 1). */
/** @} */

/**
 * @brief Tipi di feedback predefiniti per il sistema.
 */
typedef enum {
    FEEDBACK_SUCCESS,  /**< LED Verde + Beep corto (Conferma). */
    FEEDBACK_ERROR,    /**< LED Rosso + Beep lungo (Errore). */
    FEEDBACK_NEUTRAL,  /**< LED Blu (Navigazione). */
    FEEDBACK_OFF       /**< Spegne tutti i feedback attivi. */
} FeedbackType;

/**
 * @brief Inizializza i GPIO per LED, Buzzer e sensore di temperatura.
 * @details Configura i pin definiti in config_pins.h e imposta la risoluzione ADC.
 */
void peripherals_init();

/* --- Feedback Utente --- */

/**
 * @brief Esegue un pattern combinato LED + Buzzer basato sull'evento.
 * @param type Il tipo di feedback da riprodurre.
 */
void peripherals_trigger_feedback(FeedbackType type);

/**
 * @brief Controllo manuale del colore dei LED (RGB).
 * @param r Rosso (0-255).
 * @param g Verde (0-255).
 * @param b Blu (0-255).
 */
void peripherals_set_led(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Genera un tono sul buzzer.
 * @param freq Frequenza in Hz.
 * @param duration Durata in ms.
 */
void peripherals_beep(uint32_t freq, uint32_t duration);

/* --- Sensoristica Ambientale --- */

/**
 * @brief Esegue la lettura analogica e restituisce la temperatura.
 * @details Acquisisce il valore dal pin TEMP_ADC_PIN e applica la conversione.
 * @return float La temperatura corrente espressa in °C.
 */
float peripherals_read_temperature();

#endif /* PERIPHERALS_H */
