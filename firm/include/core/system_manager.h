/**
 * @file system_manager.h
 * @brief Gestione del middleware di sistema e comunicazione IPC tra core.
 * @details Coordina lo scambio dei pacchetti di telemetria, la gestione dei
 * report di errore e lo stato di sincronizzazione tra il Core principale
 * e il Core di elaborazione/IO.
 * @defgroup SystemManager Gestione Sistema
 * @{
 */

#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "core/messages.h"

/**
 * @brief Inizializza i meccanismi di comunicazione inter-core.
 */
void sys_manager_init(void);

/**
 * @brief Invia un pacchetto di telemetria al core partner.
 * @param packet Puntatore alla struttura SystemDataPacket da trasmettere.
 * @return true se l'invio è avvenuto con successo.
 */
bool sys_manager_send_data(SystemDataPacket* packet);

/**
 * @brief Riceve un pacchetto di telemetria dal core partner.
 * @param buffer Buffer di destinazione dove memorizzare i dati ricevuti.
 * @return true se il pacchetto è stato ricevuto e validato.
 */
bool sys_manager_receive_data(SystemDataPacket* buffer);

/**
 * @brief Notifica un evento di errore al sistema di diagnostica.
 * @param cat Categoria dell'errore (error_category_t).
 * @param code Codice specifico dell'errore (error_code_t).
 * @param critical Se true, innesca procedure di Failsafe immediate.
 */
void sys_manager_report_error(error_category_t cat, error_code_t code, bool critical);

/**
 * @brief Verifica se il Core 1 è pronto a ricevere comandi o dati.
 * @return true se il Core 1 è nello stato 'ready'.
 */
bool sys_is_core1_ready(void);

/**
 * @brief Imposta lo stato di disponibilità del Core 1.
 * @param ready Stato di disponibilità.
 */
void sys_set_core1_ready(bool ready);

/**
 * @brief Gestisce il parsing dei comandi in arrivo dalla porta seriale.
 */
void sys_manager_handle_serial(void);

/**
 * @brief Aggiorna localmente i dati meteo di sistema.
 * @param weather Riferimento alla struttura WeatherDataPacket aggiornata.
 * @return true se l'aggiornamento è andato a buon fine.
 */
bool sys_manager_update_weather(const WeatherDataPacket& weather);

/**
 * @brief Aggiorna le coordinate GPS correnti.
 * @param lat Latitudine.
 * @param lon Longitudine.
 * @param fix Flag di validità del fix.
 * @param sats Numero di satelliti rilevati.
 * @return true se i dati sono stati correttamente aggiornati.
 */
bool sys_manager_update_gps(float lat, float lon, bool fix, uint8_t sats);

/**
 * @brief Aggiorna esclusivamente la temperatura interna in shared_data.
 */
bool sys_manager_update_health(float temp_c);

/**
 * @brief Azzera gps_status/satellites in caso di timeout fix, preservando
 * l'ultima posizione nota (lat/lon invariati).
 */
bool sys_manager_set_gps_timeout(void);

/**
 * @brief Incrementa uptime_s di 1 secondo.
 */
bool sys_manager_increment_uptime(void);

/**
 * @brief Resetta lo stato di errore globale, rimuovendo notifiche pendenti.
 */
void sys_manager_clear_error(void);

/** @} */ // fine del gruppo SystemManager

#endif /* SYSTEM_MANAGER_H */
