#ifndef VIEW_GPS_H
#define VIEW_GPS_H

#include "core/messages.h"
#include "drivers/display_ssd1306.h"

/** @brief Mostra coordinate, velocità e satelliti. */
void view_gps_render(SystemDataPacket* data);

/** @brief Gestisce lo start/stop del logging su SD. */
void view_gps_handle_input(ButtonId btn);

#endif
