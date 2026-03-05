/**
 * @file buzzer.h
 * @brief Definizioni e prototipi per il controllo del feedback sonoro.
 * * Questo modulo permette di gestire un buzzer piezoelettrico per fornire
 * feedback acustico all'utente durante la navigazione dei menu o eventi di sistema.
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

/** * @brief Pin GPIO predefinito per il collegamento del buzzer. 
 */
#define BUZZER 21

/**
 * @brief Configura il pin del buzzer e prepara l'hardware.
 * * Imposta il pin specificato come output. Se configurato nelle macro globali,
 * può emettere un suono di conferma all'avvio.
 * * @param pin Il numero del pin GPIO da inizializzare.
 */
void buzzerInit(uint8_t pin);

/**
 * @brief Genera un suono impulsivo (beep).
 * * Produce un tono alla frequenza desiderata per una durata specifica.
 * La funzione è bloccante per l'intera durata del suono.
 * * @param freq Frequenza del tono in Hertz. Valore predefinito: @c 1200 Hz.
 * @param duration Durata del suono in millisecondi. Valore predefinito: @c 60 ms.
 */
void buzzerBeep(uint16_t freq = 1200, uint16_t duration = 60);

#endif // BUZZER_H
