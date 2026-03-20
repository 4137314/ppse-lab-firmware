#include "core/storage.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include "ff.h"

/**
 * IMPORTANTE: Nel core RP2040, le funzioni FatFS risiedono nel namespace fatfs.
 * Ma le MACRO (come f_size) devono essere usate senza prefisso.
 */
using namespace fatfs;

bool driveConnected = false;
bool inPrinting     = false;

static const char* LOG_FILENAME    = "/telemetry_log.csv";
static const char* CONFIG_FILENAME = "/config.bin";

bool storage_init() {
    static FATFS fs;
    FRESULT res = fatfs::f_mount(&fs, "", 1);
    if (res != FR_OK)
        return false;

    FIL f;
    if (fatfs::f_open(&f, LOG_FILENAME, FA_READ) != FR_OK) {
        if (fatfs::f_open(&f, LOG_FILENAME, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
            const char* header = "timestamp,lat,lon,speed,temp,satellites\n";
            UINT bw;
            fatfs::f_write(&f, (const void*)header, (UINT)strlen(header), &bw);
            fatfs::f_close(&f);
        }
    }
    return true;
}

static bool manual_gets(char* buffer, size_t len, FIL* f) {
    size_t i = 0;
    UINT br;
    char c;
    if (len == 0)
        return false;

    while (i < len - 1) {
        if (fatfs::f_read(f, &c, 1, &br) != FR_OK || br == 0) {
            if (i == 0)
                return false;
            break;
        }
        buffer[i++] = c;
        if (c == '\n')
            break;
    }
    buffer[i] = '\0';
    return true;
}

bool storage_read_line_with_tag(const char* filename, const char* tag, char* buffer, size_t len) {
    if (driveConnected)
        return false;

    FIL f;
    if (fatfs::f_open(&f, filename, FA_READ) != FR_OK)
        return false;

    bool found = false;
    while (manual_gets(buffer, len, &f)) {
        if (strncmp(buffer, tag, strlen(tag)) == 0) {
            found = true;
            break;
        }
    }

    fatfs::f_close(&f);
    return found;
}

void storage_log_append(const char* entry) {
    if (driveConnected || inPrinting)
        return;
    FIL f;

    FRESULT res = fatfs::f_open(&f, LOG_FILENAME, FA_WRITE | FA_OPEN_ALWAYS);

    if (res == FR_OK) {
        // CORREZIONE: f_size è una macro, usala senza fatfs::
        fatfs::f_lseek(&f, f_size(&f));

        char tmp[256];
        int l = snprintf(tmp, sizeof(tmp), "%s\n", entry);
        UINT bw;
        fatfs::f_write(&f, (const void*)tmp, (UINT)l, &bw);
        fatfs::f_close(&f);
    }
}

bool storage_save_params(const void* data, size_t size) {
    if (driveConnected || inPrinting)
        return false;
    FIL f;
    if (fatfs::f_open(&f, CONFIG_FILENAME, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
        UINT bw;
        FRESULT res = fatfs::f_write(&f, data, (UINT)size, &bw);
        fatfs::f_close(&f);
        return (res == FR_OK && bw == (UINT)size);
    }
    return false;
}

bool storage_load_params(void* data, size_t size) {
    FIL f;
    if (fatfs::f_open(&f, CONFIG_FILENAME, FA_READ) == FR_OK) {
        UINT br;
        FRESULT res = fatfs::f_read(&f, data, (UINT)size, &br);
        fatfs::f_close(&f);
        return (res == FR_OK && br == (UINT)size);
    }
    return false;
}

bool storage_is_busy_by_usb() {
    return driveConnected;
}
