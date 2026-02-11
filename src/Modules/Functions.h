#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <LittleFS.h> // REFERENCE: https://arduino-pico.readthedocs.io/en/latest/fs.html#file-system-object-littlefs-sd-sdfs-fatfs
#include <stdint.h>

#include "buttons.h"
#include "buzzer.h"
#include "display_ui.h"
#include "gps.h"
#include "leds.h"
#include "temp.h"

// It is in milli seconds
#define SERIAL_MAX_SETUP_TIME 20000

bool init_All();

void littleFS_Setup();

bool littleFS_write(File *ptr);

#endif
