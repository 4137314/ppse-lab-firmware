#include "pipeline/core0_manager.h"
#include <Arduino.h>
#include "drivers/config_pins.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "ui/ui_manager.h"
#include "core/messages.h"

extern volatile SystemDataPacket real_system_data;

static void handle_serial_comms() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd == "GET_GPS") {
            Serial.printf("%.6f,%.6f\n", real_system_data.latitude, real_system_data.longitude);
        } else if (cmd.startsWith("WXC")) {
            char* ptr = (char*)cmd.c_str() + 4;
            char* city = strtok(ptr, ",");
            char* temp = strtok(NULL, ",");
            char* wind = strtok(NULL, ",");
            char* hum  = strtok(NULL, ",");
            char* code = strtok(NULL, ",");
            if (city && temp && wind && hum && code) {
                strncpy((char*)real_system_data.weather.city, city, 23);
                real_system_data.weather.temp_ext   = atof(temp);
                real_system_data.weather.wind_speed = atof(wind);
                real_system_data.weather.humidity   = atoi(hum);
                real_system_data.weather.weather_code = atoi(code);
                real_system_data.weather.valid = true;
            }
        }
    }
}

void core0_setup() {
    pinMode(LED_ALIVE_PIN, OUTPUT);
    digitalWrite(LED_ALIVE_PIN, HIGH);
    Serial.begin(115200);
    pinMode(BUCK_5V_EN_PIN, OUTPUT);
    digitalWrite(BUCK_5V_EN_PIN, HIGH);
    delay(400);
    inputs_init();
    if (!display_hw_init()) while(1) { digitalWrite(LED_ALIVE_PIN, !digitalRead(LED_ALIVE_PIN)); delay(100); }
    ui_manager_init();
}

void core0_loop() {
    handle_serial_comms();
    inputs_update();
    ButtonId pressed_btn = inputs_get_last_press();
    if (pressed_btn != BTN_NONE) ui_manager_dispatch_input(pressed_btn);
    ui_manager_update((void*)&real_system_data);
    digitalWrite(LED_ALIVE_PIN, (millis() / 500) % 2);
    delay(10);
}
