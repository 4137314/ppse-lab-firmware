#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool storage_init(void);
void storage_log_append(const char* entry);
void storage_sync(void);
bool storage_read_line_with_tag(const char* filename, const char* tag, char* buffer, size_t len);
bool storage_save_params(const void* data, size_t size);
bool storage_load_params(void* data, size_t size);
bool storage_is_busy_by_usb(void);
void storage_log_gps_fix(float lat, float lon);
bool storage_read_last_gps(float* lat, float* lon);
void debug_dump_gps_log();

#endif // STORAGE_H
