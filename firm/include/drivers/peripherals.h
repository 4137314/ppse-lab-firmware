/**
 * @file peripherals.h
 * @brief Driver agnostico unificato per feedback (LED, Buzzer) e sensori.
 * @details Fornisce un'interfaccia di alto livello per gestire le periferiche 
 * di feedback visivo e acustico, e per l'acquisizione di dati ambientali.
 * @defgroup Peripherals Driver Periferiche
 * @{
 */

#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <stdbool.h>
#include <stdint.h>
#include "core/messages.h"

/**
 * @brief Tipologie di feedback utente per azioni UI.
 */
typedef enum {
    FEEDBACK_SUCCESS, /**< Feedback positivo (es. OK salvato). */
    FEEDBACK_ERROR,   /**< Feedback negativo (es. errore operazione). */
    FEEDBACK_NEUTRAL, /**< Feedback informativo neutro. */
    FEEDBACK_OFF      /**< Disabilita feedback. */
} FeedbackType;

/**
 * @brief Stati di animazione LED per sistema di feedback visuale.
 */
typedef enum {
    LED_ANIM_NONE,        /**< Nessuna animazione. */
    LED_ANIM_BOOT,        /**< Animazione fase di avvio. */
    LED_ANIM_BOOT_DONE,   /**< Segnalazione completamento boot. */
    LED_ANIM_SEARCHING,   /**< Ricerca segnale (es. GPS/Wi-Fi). */
    LED_ANIM_FIX_OK,      /**< Segnale acquisito correttamente. */
    LED_ANIM_SIGNAL_LOW   /**< Segnale debole o degradato. */
} LedAnimState;

/**
 * @name Inizializzazione
 * @{
 */
void peripherals_init(void);
void peripherals_init_leds(void);
/** @} */

/**
 * @name Controllo Fisico Base
 * @{
 */
void peripherals_set_leds(bool enabled);
void peripherals_set_buzzer(bool enabled);
/** @} */

/**
 * @name Feedback Visivo e Acustico
 * @{
 */

/**
 * @brief Aggiorna l'animazione LED basata sullo stato di sistema.
 * @param state Stato dell'animazione desiderato (LedAnimState).
 * @param val Valore opzionale (es. luminosità 0.0-1.0 o intensità).
 */
void peripherals_update_led_fx(LedAnimState state, float val);

/**
 * @brief Esegue un feedback automatico basato sui dati di telemetria.
 * @param data Pacchetto telemetrico da analizzare.
 */
void peripherals_auto_feedback(const SystemDataPacket* data);

/**
 * @brief Attiva un feedback sonoro/visivo basato su un evento.
 * @param type Tipologia di feedback (FeedbackType).
 */
void peripherals_trigger_feedback(FeedbackType type);

/**
 * @brief Imposta colore RGB statico per i LED.
 */
void peripherals_set_led(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Esegue un breve suono (beep).
 * @param freq_hz Frequenza in Hertz.
 * @param duration_ms Durata in millisecondi.
 */
void peripherals_beep(uint32_t freq_hz, uint32_t duration_ms);

/**
 * @brief Esegue il tipico suono di "click" per conferma UI.
 */
void peripherals_play_click(void);
/** @} */

/**
 * @name Acquisizione Dati Ambientali
 * @{
 */
float peripherals_get_temperature(void);
float peripherals_get_battery_voltage(void);
/** @} */

/** @} */ // fine del gruppo Peripherals

#endif /* PERIPHERALS_H */
