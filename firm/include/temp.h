/**
 * @file temp.h
 * @brief Definizioni hardware e prototipi per la lettura della temperatura.
 * * Questo header definisce i parametri di configurazione per l'ADC dell'RP2040
 * e i prototipi delle funzioni necessarie per l'acquisizione termica tramite
 * sensore analogico lineare (es. TC1047A o LM35).
 */

#ifndef TEMP_H
#define TEMP_H

#include <Arduino.h>

/** @name Configurazione Hardware ADC
 * @{ */
#define TEMP_PIN    26   /**< Pin GPIO (ADC0) collegato all'uscita del sensore di temperatura. */
#define ADC_BITS    12   /**< Risoluzione dei bit dell'ADC (12-bit per RP2040). */
#define ADC_MAX_VAL 4095 /**< Valore massimo dell'ADC calcolato come (2^ADC_BITS) - 1. */
/** @} */

/* --- Funzioni di Interfaccia --- */

/**
 * @brief Inizializza il pin del sensore e configura i parametri dell'ADC.
 * * Prepara il GPIO selezionato come ingresso analogico e imposta la
 * risoluzione corretta per le letture successive.
 */
void TempInit();

/**
 * @brief Esegue la lettura analogica e restituisce il valore in gradi Celsius.
 * * Acquisisce il valore di tensione dal pin @ref TEMP_PIN e applica la formula
 * di conversione lineare specifica per il sensore utilizzato.
 * * @return float La temperatura corrente espressa in °C.
 */
float readTemp();

#endif  // TEMP_H
