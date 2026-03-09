#ifndef VIEW_METEO_H
#define VIEW_METEO_H

#include "core/messages.h"
#include "drivers/display_ssd1306.h"

/** @brief Renderizza temperatura, umidità e icone meteo. */
void view_meteo_render(SystemDataPacket* data);

/** @brief Permette di scorrere tra meteo attuale e previsioni (se disponibili). */
void view_meteo_handle_input(ButtonId btn);

#endif
