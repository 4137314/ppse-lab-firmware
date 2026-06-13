#include "pipeline/core1_manager.h"
#include <Arduino.h>
#include "drivers/sensors_i2c.h"
#include "core/messages.h"

extern volatile SystemDataPacket real_system_data;

void core1_setup() {
    sensors_i2c_init();
}

void core1_loop() {
    real_system_data.uptime_s = millis() / 1000;
    real_system_data.temp_c   = sensors_read_temperature_c();
    real_system_data.battery_v = sensors_read_battery_v();
    delay(50);
}
