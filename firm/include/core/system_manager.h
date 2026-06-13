#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "core/messages.h"  // Importante per SystemDataPacket

void sys_manager_init(void);

// Usa i nomi che hai scritto nel .cpp
bool sys_manager_send_data(SystemDataPacket* packet);
bool sys_manager_receive_data(SystemDataPacket* buffer);

// Sincronizzazione core
bool sys_is_core1_ready(void);
void sys_set_core1_ready(bool ready);
// Gestisce i comandi seriali in arrivo
void sys_manager_handle_serial(void);

#endif /* SYSTEM_MANAGER_H */
