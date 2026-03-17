/**
 * @file view_definitions.h
 * @brief Definizioni astratte per il framework dell'interfaccia utente.
 */

#ifndef VIEW_DEFINITIONS_H
#define VIEW_DEFINITIONS_H

#include <stdint.h>

/**
 * @brief Interfaccia funzionale di una schermata (View).
 * @details Ogni view implementa questi callback. Il manager non sa cosa 
 * contengano, sa solo quando chiamarli.
 */
typedef struct {
    /** @brief Chiamata una sola volta quando la view diventa attiva. */
    void (*on_enter)(void);

    /** * @brief Rendering e aggiornamento logico.
     * @param data Puntatore opaco ai dati di sistema (SystemDataPacket).
     */
    void (*on_update)(const void* data);

    /** * @brief Gestore eventi di input.
     * @param input_id ID agnostico del comando (es. ButtonId castato a uint8_t).
     */
    void (*on_input)(uint8_t input_id);

    /** @brief Chiamata prima di distruggere o cambiare view per pulizia memoria. */
    void (*on_exit)(void);
} view_interface_t;

#endif /* VIEW_DEFINITIONS_H */
