/**
 * @file radio_uart.h
 * @brief Driver agnostico per la gestione delle interfacce seriali (UART0 e UART1).
 * @details Astrae le comunicazioni con il modulo GPS (SAM-M8Q) e WiFi (ESP-01).
 * Fornisce un'interfaccia a basso livello per il buffering dei dati.
 * Repository: https://github.com/4137314/ppse-lab-firmware
 */

#ifndef RADIO_UART_H
#define RADIO_UART_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Configura le periferiche UART0 e UART1 dell'RP2040.
 * @details Imposta i baudrate specifici: solitamente 9600 per il GPS 
 * e 115200 per l'ESP-01.
 */
void radio_uart_init(void);

/** @name Interfaccia GPS (UART1 - SAM-M8Q) 
 * @{ */

/** @brief Verifica se ci sono byte pronti nel buffer di ricezione del GPS. */
bool radio_gps_available(void);

/** @brief Legge un singolo byte dal modulo GPS. */
uint8_t radio_gps_read(void);

/** * @brief Invia un comando o un pacchetto di configurazione al GPS.
 * @param buffer Puntatore ai dati da inviare.
 * @param len Lunghezza dei dati.
 */
void radio_gps_send(const uint8_t* buffer, size_t len);

/** @} */

/** @name Interfaccia WiFi (UART0 - ESP-01) 
 * @{ */

/** @brief Verifica se ci sono dati in arrivo dal modulo WiFi. */
bool radio_esp_available(void);

/** @brief Legge un singolo byte dal modulo WiFi. */
uint8_t radio_esp_read(void);

/** * @brief Invia dati o comandi AT al modulo ESP.
 * @param buffer Puntatore ai dati (es. stringa AT o pacchetto UDP).
 * @param len Lunghezza dei dati.
 */
void radio_esp_send(const uint8_t* buffer, size_t len);

/** @} */

#endif /* RADIO_UART_H */
