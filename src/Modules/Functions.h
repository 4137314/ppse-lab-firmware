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

void check_GPS_sync_req(queue_t *recv_queue, queue_t *send_queue, struct parsed_nmea *nmea_ptr);




#endif
