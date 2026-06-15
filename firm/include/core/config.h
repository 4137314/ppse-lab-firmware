/**
 * @file config.h
 * @brief Gestione delle configurazioni di sistema persistenti.
 * * Questo modulo definisce la struttura SystemConfig che contiene i parametri
 * operativi del dispositivo e fornisce le interfacce per inizializzare 
 * e salvare tali configurazioni nella memoria flash.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @struct SystemConfig
 * @brief Parametri di configurazione globale del dispositivo.
 */
typedef struct {
    uint8_t  oled_brightness;  ///< Luminosità display OLED (range: 0-255)
    bool     buzzer_enabled;   ///< Stato del cicalino (true = attivo)
    bool     leds_enabled;     ///< Stato dei LED di sistema (true = attivi)
    uint8_t  gps_refresh_rate; ///< Frequenza di aggiornamento del modulo GPS (in Hz)
} SystemConfig;

/**
 * @brief Istanza globale della configurazione di sistema.
 * * Questa variabile mantiene lo stato corrente dei settaggi. 
 * Viene inizializzata durante il boot del sistema.
 */
extern SystemConfig global_cfg;

/**
 * @brief Inizializza il modulo di configurazione.
 * * Carica i parametri dalla memoria flash (LittleFS) nella variabile global_cfg.
 * Se il file di configurazione non esiste, ripristina i valori di default.
 */
void config_init(void);

/**
 * @brief Salva la configurazione corrente nella memoria flash.
 * * Serializza la struct global_cfg nel file di configurazione persistente.
 * Deve essere chiamata ogni volta che viene modificato un parametro utente.
 */
void config_save(void);

#endif // CONFIG_H
