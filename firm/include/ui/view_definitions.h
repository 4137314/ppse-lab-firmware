/**
 * @file view_definitions.h
 * @brief Definizione del contratto per le View dell'interfaccia utente.
 * @details Definisce l'interfaccia (`view_interface_t`) che ogni schermata 
 * deve implementare per essere gestita dall'orchestratore UI.
 * @defgroup ViewDefinitions Definizioni Viste
 * @{
 */

#ifndef VIEW_DEFINITIONS_H
#define VIEW_DEFINITIONS_H

#include <stdint.h>
#include "drivers/inputs.h"

/**
 * @brief Struttura che definisce l'interfaccia funzionale di una schermata (View).
 * @details Implementa il pattern 'Interface' in C tramite puntatori a funzione.
 * Ogni vista deve esporre queste quattro funzioni per il lifecycle.
 */
typedef struct {
    /** @brief Chiamata una sola volta all'attivazione della view (es. reset stato). */
    void (*on_enter)(void);

    /** * @brief Rendering grafico e logica di aggiornamento. 
     * @param data Puntatore al pacchetto di sistema (SystemDataPacket*). 
     */
    void (*on_update)(const void* data);

    /** * @brief Gestore degli eventi di input inviati dall'orchestratore.
     * @param btn Il tasto fisico (es. BTN_UP).
     * @param state Lo stato (es. BTN_RELEASED).
     */
    void (*on_input)(button_t btn, button_state_t state);

    /** @brief Chiamata prima di disattivare la view corrente per passare a un'altra. */
    void (*on_exit)(void);
} view_interface_t;

/**
 * @name Istanze delle Viste di Sistema
 * @{
 * @brief Esportazione delle interfacce implementate nei relativi moduli.
 */
extern const view_interface_t view_home;     /**< Vista Dashboard principale. */
extern const view_interface_t view_gps;      /**< Vista dati di navigazione. */
extern const view_interface_t view_meteo;    /**< Vista meteo. */
extern const view_interface_t view_settings; /**< Vista menu impostazioni. */
extern const view_interface_t view_info;     /**< Vista informazioni di sistema. */
/** @} */

/** @} */ // fine del gruppo ViewDefinitions

#endif /* VIEW_DEFINITIONS_H */
