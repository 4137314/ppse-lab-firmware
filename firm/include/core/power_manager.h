/**
 * @file power_manager.h
 * @brief Gestore agnostico dell'alimentazione e dei carichi hardware.
 * @details Centralizza il controllo dei moduli (GPS, WiFi, USB) e implementa
 * logiche di protezione per la batteria LiPo.
 */

#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

/** * @brief Profili energetici del sistema.
 * @details Definiscono il comportamento dei carichi in base alla missione.
 */
typedef enum {
    PWR_MODE_PERFORMANCE = 0, /**< Massima potenza: tutti i moduli attivi. */
    PWR_MODE_BALANCED,        /**< Risparmio dinamico: WiFi on-demand. */
    PWR_MODE_LOW_POWER,       /**< Solo telemetria essenziale e GPS. */
    PWR_MODE_CRITICAL         /**< Emergenza: spegnimento carichi non vitali (Failsafe). */
} power_mode_t;

/**
 * @brief Inizializza i controller di potenza (Buck, Enable pins).
 * @details Configura i GPIO di uscita e verifica la tensione di avvio.
 */
void power_manager_init(void);

/**
 * @brief Task di monitoraggio energetico.
 * @details Da chiamare periodicamente per gestire le transizioni di stato 
 * basate sulla tensione della batteria.
 */
void power_manager_update(void);

/** @name Controllo Moduli Radio
 * @{ */

/** @brief Abilita/Disabilita l'alimentazione del modulo GPS (SAM-M8Q). */
void power_gps_set_state(bool enable);

/** @brief Esegue un reset hardware del modulo GPS. */
void power_gps_reset(void);

/** @brief Abilita/Disabilita l'alimentazione del modulo WiFi (ESP-01). */
void power_wifi_set_state(bool enable);

/** @} */

/** @name Gestione Carichi di Potenza
 * @{ */

/** @brief Abilita l'uscita di alimentazione per periferiche USB esterne. */
void power_usb_output_enable(bool enable);

/** @brief Controllo del convertitore Buck principale della board. */
void power_main_buck_enable(bool enable);

/** @} */

/** @name Telemetria e Diagnostica
 * @{ */

/** @brief Restituisce la tensione della batteria filtrata. 
 * @return float Tensione in Volt (V). */
float power_get_battery_voltage(void);

/** @brief Restituisce la percentuale di carica residua della LiPo.
 * @return int8_t Percentuale (0-100), o -1 se non rilevata. */
int8_t power_get_battery_level(void);

/** @brief Verifica se il sistema è alimentato via cavo USB. */
bool power_source_is_usb(void);

/** @brief Imposta manualmente il profilo energetico. */
void power_set_mode(power_mode_t mode);

/** @} */

#endif /* POWER_MANAGER_H */
