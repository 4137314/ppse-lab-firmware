/**
 * @file telemetry.h
 * @brief Gestione telemetria con accesso atomico per architetture multi-core.
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "core/messages.h"

void telemetry_init(void);
void telemetry_update(void);
bool telemetry_get_frame(SystemDataPacket* dest); // Passaggio per puntatore tipizzato
bool telemetry_is_healthy(void);

#endif
