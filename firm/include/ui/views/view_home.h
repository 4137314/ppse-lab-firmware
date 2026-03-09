#ifndef VIEW_HOME_H
#define VIEW_HOME_H

#include "core/messages.h"
#include "drivers/display_ssd1306.h"

/** @brief Renderizza la dashboard principale (Watchface). */
void view_home_render(SystemDataPacket* data);

/** @brief Gestisce l'ingresso nel menu principale o lo switch delle info rapide. */
void view_home_handle_input(ButtonId btn);

#endif
