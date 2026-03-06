/**
 * @file buzzer.cpp
 * @brief Driver per la gestione del feedback acustico tramite buzzer
 * piezoelettrico.
 * * Fornisce funzioni per l'inizializzazione del pin e la generazione di
 * segnali acustici (beep) utilizzando la funzione tone() di Arduino.
 */

#include "buzzer.h"

/** * @brief Pin GPIO assegnato al buzzer.
 * @note La variabile è dichiarata @c static per limitarne lo scope a questo
 * file.
 */
static uint8_t buzzerPin;

/**
 * @brief Inizializza il buzzer e configura il relativo pin.
 * * Configura il pin come @c OUTPUT. Se la macro @ref BUZZER_INIT_BEEP è
 * impostata a 1, esegue un beep di test all'avvio.
 * * @param pin Il numero del pin GPIO a cui è collegato il buzzer.
 */
void buzzerInit(uint8_t pin) {
    buzzerPin = pin;
    pinMode(buzzerPin, OUTPUT);

#if DEBUG
    Serial.println("Buzzer initialized");
#endif

// Se la macro è definita a 1, fa il beep, altrimenti tace
#if BUZZER_INIT_BEEP == 1
    buzzerBeep();
#if DEBUG
    Serial.println("Buzzer init beep sent");
#endif
#endif
}

/**
 * @brief Genera un segnale acustico (beep) a frequenza e durata specificate.
 * * Utilizza la generazione PWM per produrre un'onda quadra alla frequenza
 * desiderata.
 * * @param freq Frequenza del tono in Hz (Default: 2000Hz).
 * @param duration Durata del tono in millisecondi (Default: 100ms).
 */
void buzzerBeep(uint16_t freq, uint16_t duration) {
    tone(buzzerPin, freq);
    delay(duration);
    noTone(buzzerPin);
}
