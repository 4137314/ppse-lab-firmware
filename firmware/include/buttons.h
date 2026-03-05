/**
 * @file buttons.h
 * @brief Definizioni e prototipi per la gestione degli input utente e della UI.
 * * Questo header contiene le definizioni dei pin per i pulsanti di
 * navigazione, i flag di stato per la macchina a stati del menu e i prototipi
 * per la gestione degli interrupt e del debounce su RP2040.
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

#include "display_ui.h"
#include "gps.h"
#include "hardware/gpio.h"
#include "leds.h"
#include "pico/stdlib.h"

/** @name Mapping Hardware Pulsanti
 * @{ */
#define SW_UP 22    /**< Pin GPIO per il pulsante Su. */
#define SW_DOWN 23  /**< Pin GPIO per il pulsante Giù. */
#define SW_RIGHT 24 /**< Pin GPIO per il pulsante Destra / Invio. */
#define SW_LEFT 25  /**< Pin GPIO per il pulsante Sinistra / Indietro. */
/** @} */

/** @name Variabili di Stato UI
 * @{ */
extern int menuIndex;  /**< Indice corrente della voce selezionata nel menu
                          principale. */
extern bool menuOpen;  /**< Stato di visibilità del menu principale. */
extern bool inSubmenu; /**< Indica se l'utente si trova all'interno di una
                          schermata sottomenu. */
extern bool
    ambientDisplay; /**< Indica se il sistema è in modalità Home (Ambient). */
/** @} */

/** @name Flag di Interrupt (Volatile)
 * @{ */
extern volatile bool up_press; /**< Segnala l'avvenuto debounce del tasto UP. */
extern volatile bool
    down_press; /**< Segnala l'avvenuto debounce del tasto DOWN. */
extern volatile bool
    left_press; /**< Segnala l'avvenuto debounce del tasto LEFT. */
extern volatile bool
    right_press; /**< Segnala l'avvenuto debounce del tasto RIGHT. */
/** @} */

/** @name Parametri Temporali e Telemetria
 * @{ */
extern uint32_t tLeft;  /**< Timestamp dell'ultima pressione sinistra per
                           rilevamento combo. */
extern uint32_t tRight; /**< Timestamp dell'ultima pressione destra per
                           rilevamento combo. */
extern const uint32_t COMBO_MS; /**< Finestra temporale massima per validare una
                                   pressione simultanea. */
extern uint32_t lastActivity; /**< Timestamp dell'ultima attività registrata per
                                 il timeout. */
extern const uint32_t
    DISPLAY_TIMEOUT_MS; /**< Tempo di inattività prima del ritorno alla home. */
/** @} */

/** @brief Identificatore per l'animazione dei LED di navigazione. */
extern int ledsNumber;

/**
 * @brief Tipi di eventi generabili dalla pressione dei pulsanti.
 */
enum KeyEvent : uint8_t {
  KEY_NONE, /**< Nessun tasto premuto. */
  KEY_UP,   /**< Evento tasto Su. */
  KEY_DOWN, /**< Evento tasto Giù. */
  KEY_LEFT, /**< Evento tasto Sinistra. */
  KEY_RIGHT /**< Evento tasto Destra. */
};

/* --- Funzioni Core --- */

/**
 * @brief Inizializza i pin GPIO e configura gli interrupt per tutti i pulsanti.
 */
void buttonsInit();

/**
 * @brief Esegue il polling della logica dei menu e gestisce gli eventi dei
 * tasti.
 * @note Deve essere chiamata ciclicamente nel loop principale.
 */
void buttonsUpdate();

/**
 * @brief Gestisce l'azione combinata (pressione simultanea) di LEFT e RIGHT.
 * Tipicamente utilizzata per il toggle tra Home e Menu.
 */
void onLeftRightCombo();

/**
 * @brief Analizza l'evento corrente per verificare se fa parte di una combo.
 * @param ev L'evento tasto catturato.
 * @return true se l'evento è stato consumato come parte di una combo.
 */
bool checkLeftRightCombo(KeyEvent ev);

/* --- Prototipi Callback --- */

/**
 * @brief Funzione richiamata dall'allarme hardware per confermare la pressione
 * (Debounce).
 * @param id ID dell'allarme hardware Pico.
 * @param user_data Dati utente (generalmente il numero del pin).
 * @return long long Tempo per la prossima esecuzione (0 per esecuzione
 * singola).
 */
long long debounce_callback(alarm_id_t id, void* user_data);

/**
 * @brief Callback centralizzato per gli interrupt GPIO dei pulsanti.
 * @param gpio Numero del pin GPIO che ha scatenato l'IRQ.
 * @param events Maschera degli eventi (rise/fall).
 */
void buttons_callback(uint gpio, uint32_t events);

#endif  // BUTTONS_H
