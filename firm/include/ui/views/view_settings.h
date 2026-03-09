#ifndef VIEW_SETTINGS_H
#define VIEW_SETTINGS_H

#include "core/messages.h"
#include "drivers/display_ssd1306.h"

/** @brief Renderizza la lista delle impostazioni (Luminosità, LED, etc). */
void view_settings_render(SystemDataPacket* data);

/** @brief Gestisce la navigazione tra le voci e la modifica dei valori. */
void view_settings_handle_input(ButtonId btn);

#endif
