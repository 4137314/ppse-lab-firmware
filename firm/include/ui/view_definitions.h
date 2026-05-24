#ifndef VIEW_DEFINITIONS_H
#define VIEW_DEFINITIONS_H

#include <stdint.h>
#include "drivers/inputs.h"

/**
 * @brief Interfaccia funzionale di una schermata (View).
 */
typedef struct {
    /** @brief Chiamata all'attivazione della view. */
    void (*on_enter)(void);

    /** @brief Rendering e logica. param data: SystemDataPacket*. */
    void (*on_update)(const void* data);

    /** * @brief Gestore eventi di input.
     * @param btn Il tasto fisico (es. BTN_UP)
     * @param state Lo stato (es. BTN_RELEASED)
     */
    void (*on_input)(button_t btn, button_state_t state);

    /** @brief Chiamata prima di cambiare view. */
    void (*on_exit)(void);
} view_interface_t;

// =================================================================
// AGGIUNGI QUESTE RIGHE QUI SOTTO:
// Esibisce le istanze delle viste a ui_manager.cpp senza includere i .cpp
// =================================================================
extern const view_interface_t view_home;
extern const view_interface_t view_gps;
extern const view_interface_t view_meteo;
extern const view_interface_t view_settings;
extern const view_interface_t view_info;

#endif /* VIEW_DEFINITIONS_H */
