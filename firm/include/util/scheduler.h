/**
 * @file scheduler.h
 * @brief Motore di esecuzione multitasking cooperativo per task periodici.
 * @details Gestisce l'invocazione di funzioni (callback) basata su intervalli 
 * temporali (polling) senza l'uso di un RTOS complesso.
 * @defgroup Scheduler Gestore Task
 * @{
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

/**
 * @brief Firma della funzione callback per un task.
 */
typedef void (*TaskCallback)(void);

/**
 * @brief Struttura dati che definisce un task periodico.
 */
typedef struct {
    TaskCallback callback; /**< Puntatore alla funzione da eseguire. */
    uint32_t interval;     /**< Intervallo di esecuzione in millisecondi. */
    uint32_t last_run;     /**< Timestamp dell'ultima esecuzione (ms). */
} Task;

/**
 * @brief Verifica ed esegue il task se l'intervallo è scaduto.
 * @details Implementa la logica di controllo non bloccante. Confronta il 
 * tempo corrente (millis) con l'ultimo tempo di esecuzione memorizzato.
 * @param t Puntatore alla struttura Task da gestire.
 */
inline void run_task(Task* t) {
    uint32_t now = millis();
    if (now - t->last_run >= t->interval) {
        t->callback();
        t->last_run = now;
    }
}

/** @} */ // fine del gruppo Scheduler

#endif // SCHEDULER_H
