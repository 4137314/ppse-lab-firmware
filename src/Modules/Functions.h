#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <stdint.h>

#include "buttons.h"
#include "buzzer.h"
#include "display_ui.h"
#include "gps.h"
#include "leds.h"
#include "temp.h"
#include "Filesystem.h"

// It is in milli seconds
#define SERIAL_MAX_SETUP_TIME 20000

bool init_All();



#endif
