/**
 * @file core1_manager.h
 * @brief Punto di ingresso e ciclo di elaborazione per il Core 1.
 * @details Il Core 1 funge da "Worker" dedicato, gestendo task a bassa latenza 
 * come l'acquisizione GPS, la lettura dei sensori IMU e il calcolo dei cicli di controllo.
 * @defgroup Core1 RealTimeWorker
 * @{
 */

#pragma once

/**
 * @brief Setup iniziale del Core 1.
 * @details Configura le periferiche real-time, i bus di comunicazione (I2C/UART) 
 * e sincronizza lo stato con il Core 0 prima di entrare nel loop.
 */
void core1_setup(void);

/**
 * @brief Ciclo principale di elaborazione del Core 1.
 * @details Gestisce il processing continuo dei dati sensoristici e mantiene
 * aggiornato il buffer di telemetria condiviso con il Core 0 tramite IPC.
 */
void core1_loop(void);

/** @} */ // fine del gruppo Core1
