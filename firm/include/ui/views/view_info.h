#ifndef VIEW_INFO_H
#define VIEW_INFO_H

#include "core/messages.h"
#include "drivers/display_ssd1306.h"

/** @brief Mostra Uptime, versione Firmware e stato HW. */
void view_info_render(SystemDataPacket* data);

/** @brief Gestisce il ritorno al menu principale. */
void view_info_handle_input(ButtonId btn);

#endif
