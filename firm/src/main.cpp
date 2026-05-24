#include <Arduino.h>
#include <Wire.h>
#include "drivers/config_pins.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "drivers/sensors_i2c.h"
#include "drivers/peripherals.h"
#include "ui/ui_manager.h"
#include "core/messages.h"

static SystemDataPacket real_system_data;

void handle_serial_comms() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim(); // Rimuove spazi/newline extra
        
        if (cmd == "GET_GPS") {
            Serial.printf("%.6f,%.6f\n", real_system_data.latitude, real_system_data.longitude);
        } 
        else if (cmd.startsWith("WXC")) {
            // Parsing manuale più sicuro
            char* ptr = (char*)cmd.c_str() + 4; // Salta "WXC,"
            
            // Estrazione campi usando strtok
            char* city = strtok(ptr, ",");
            char* temp = strtok(NULL, ",");
            char* wind = strtok(NULL, ",");
            char* hum  = strtok(NULL, ",");
            char* code = strtok(NULL, ",");

            if (city && temp && wind && hum && code) {
                strncpy(real_system_data.weather.city, city, 23);
                real_system_data.weather.temp_ext   = atof(temp);
                real_system_data.weather.wind_speed = atof(wind);
                real_system_data.weather.humidity   = atoi(hum);
                real_system_data.weather.weather_code = atoi(code);
                real_system_data.weather.valid = true;
            }
        }
    }
}

void setup() {
    pinMode(LED_ALIVE_PIN, OUTPUT);
    digitalWrite(LED_ALIVE_PIN, HIGH);
    Serial.begin(115200);
    
    pinMode(BUCK_5V_EN_PIN, OUTPUT);
    digitalWrite(BUCK_5V_EN_PIN, HIGH);
    delay(400);

    inputs_init();
    if (!display_hw_init()) while(1) { digitalWrite(LED_ALIVE_PIN, !digitalRead(LED_ALIVE_PIN)); delay(100); }

    sensors_i2c_init();
    ui_manager_init();
    
    memset(&real_system_data, 0, sizeof(SystemDataPacket));
}

void loop() {
    handle_serial_comms();
    inputs_update();
    ButtonId pressed_btn = inputs_get_last_press();
    if (pressed_btn != BTN_NONE) ui_manager_dispatch_input(pressed_btn);
    
    real_system_data.uptime_s = millis() / 1000;
    real_system_data.temp_c   = sensors_read_temperature_c();
    real_system_data.battery_v = sensors_read_battery_v();

    ui_manager_update(&real_system_data);
    digitalWrite(LED_ALIVE_PIN, (millis() / 500) % 2);
    delay(20);
}
