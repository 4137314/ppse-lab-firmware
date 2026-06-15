/**
 * @file tasks.h
 * @brief Dichiarazione dei task di sistema per lo scheduler.
 * @details Questo file espone i task globali che compongono l'architettura 
 * multi-tasking del firmware. Ogni task rappresenta un'unità di esecuzione 
 * periodica gestita dallo scheduler.
 * @defgroup TaskManager Gestione Task
 * @{
 */

#ifndef TASKS_H
#define TASKS_H

#include "util/scheduler.h"

/**
 * @brief Task di gestione comunicazione seriale.
 * @details Gestisce il parsing dei comandi in arrivo e l'invio della telemetria.
 */
extern Task task_serial;

/**
 * @brief Task di acquisizione input utente.
 * @details Si occupa del polling e del debounce dei pulsanti fisici.
 */
extern Task task_input;

/**
 * @brief Task di aggiornamento dell'interfaccia utente (UI).
 * @details Gestisce il render dei menu sul display OLED e la navigazione tra le viste.
 */
extern Task task_ui;

/**
 * @brief Task di monitoraggio integrità sistema (Health Check).
 * @details Controlla lo stato di carica della batteria, temperature e 
 * diagnostica errori per prevenire guasti critici.
 */
extern Task task_health;

/** @} */ // fine del gruppo TaskManager

#endif // TASKS_H
