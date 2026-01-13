#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdint.h>
#include <Arduino.h>


#include "buzzer.h"
#include "buttons.h"
#include "display_ui.h"
#include "leds.h"
#include "temp.h"

#include "gps.h"

// It is in milli seconds
#define SERIAL_MAX_SETUP_TIME 20000

    bool init_All();


#endif

