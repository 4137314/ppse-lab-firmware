#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include "core/messages.h"

void sys_manager_init(void);

// Comunicazione Dati tra Core
bool sys_manager_send_data(SystemDataPacket* packet);
bool sys_manager_receive_data(SystemDataPacket* buffer);

// Diagnostica e Errori
void sys_manager_report_error(error_category_t cat, error_code_t code, bool critical);

// Gestione Sincronizzazione Core
bool sys_is_core1_ready(void);
void sys_set_core1_ready(bool ready);

// Gestione Interfaccia Seriale
void sys_manager_handle_serial(void);

// Aggiungi queste due righe nel tuo .h
bool sys_manager_update_weather(const WeatherDataPacket& weather);
bool sys_manager_update_gps(float lat, float lon, bool fix, uint8_t sats);
// Aggiungi in core/system_manager.h
void sys_manager_clear_error();

#endif /* SYSTEM_MANAGER_H */
