/**
 * @file peripherals.h
 * @brief Driver agnostico unificato per feedback (LED, Buzzer) e sensori.
 * @details Isola le librerie hardware (PWM, ADC, WS2812) per garantire
 * la massima portabilità tra framework e core.
 * Repository: https://github.com/4137314/ppse-lab-firmware
 */

#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <stdbool.h>
#include <stdint.h>
#include "core/messages.h" // Includi direttamente il file dei messaggi

/**
 * @brief Stati logici di feedback per l'utente o il sistema di volo.
 */
typedef enum {
    FEEDBACK_SUCCESS, /**< Conferma operazione (es. Motori Armati). */
    FEEDBACK_ERROR,   /**< Allerta critica (es. Batteria Scarica). */
    FEEDBACK_NEUTRAL, /**< Stato normale (es. Standby). */
    FEEDBACK_OFF      /**< Disattiva tutti i feedback. */
} FeedbackType;

/**
 * @brief Inizializza l'hardware delle periferiche.
 * @details Prepara l'accensione elettrica immediata della PCB (GPIO 11), i canali 
 * PWM per il buzzer e i registri ADC per la sensoristica analogica di bordo.
 */
void peripherals_init(void);

/**
 * @brief Inizializza il modulo logico dei LED RGB (FastLED).
 * @details Isola la configurazione software del bus WS2812B per eseguirla 
 * esclusivamente dopo che i clock e i timer della MCU si sono stabilizzati.
 */
void peripherals_init_leds(void);

// Aggiungi queste nuove definizioni
typedef enum {
    LED_ANIM_NONE,
    LED_ANIM_BOOT,      // Sweep bianco caldo
    LED_ANIM_SEARCHING, // Pulsazione Blu Notte
    LED_ANIM_FIX_OK,    // Verde Menta fisso/dim
    LED_ANIM_SIGNAL_LOW // Ambra pulsante
} LedAnimState;

// Nuova funzione per aggiornare l'animazione in modo non bloccante
void peripherals_update_led_fx(LedAnimState state);

void peripherals_auto_feedback(const SystemDataPacket* data);

/* --- Feedback Visivo e Acustico --- */

/**
 * @brief Attiva un pattern di feedback predefinito.
 * @param type L'evento da segnalare.
 */
void peripherals_trigger_feedback(FeedbackType type);

/**
 * @brief Controllo manuale del colore LED RGB.
 * @details Astrae l'uso di librerie come NeoPixel o FastLED.
 * @param r Componente Rossa (0-255).
 * @param g Componente Verde (0-255).
 * @param b Componente Blu (0-255).
 */
void peripherals_set_led(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Genera un segnale acustico.
 * @param freq_hz Frequenza del tono.
 * @param duration_ms Durata del segnale.
 */
void peripherals_beep(uint32_t freq_hz, uint32_t duration_ms);

/* --- Acquisizione Dati Ambientali --- */

/**
 * @brief Legge la temperatura corrente.
 * @details Astrae la conversione ADC e l'equazione di Steinhart-Hart o il guadagno lineare.
 * @return float Temperatura in gradi Celsius (°C).
 */
float peripherals_get_temperature(void);

/**
 * @brief Restituisce la tensione della batteria del drone.
 * @details Fondamentale per il failsafe del drone.
 * @return float Tensione in Volt (V).
 */
float peripherals_get_battery_voltage(void);

#endif /* PERIPHERALS_H */
