#include <Arduino.h>
#include <Wire.h>

#include "drivers/config_pins.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "drivers/sensors_i2c.h"
#include "drivers/peripherals.h"
#include "ui/ui_manager.h"
#include "core/messages.h"

// Dichiarazione "volatile" per permettere l'accesso sicuro da entrambi i core
volatile SystemDataPacket real_system_data;

// --- GESTIONE SERIALE (Core 0) ---
void handle_serial_comms() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd == "GET_GPS") {
            Serial.printf("%.6f,%.6f\n", real_system_data.latitude, real_system_data.longitude);
        } 
        else if (cmd.startsWith("WXC")) {
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

// --- SETUP E LOOP CORE 0 (UI & Seriale) ---
void setup() {
    pinMode(LED_ALIVE_PIN, OUTPUT);
    digitalWrite(LED_ALIVE_PIN, HIGH);
    Serial.begin(115200);
    
    pinMode(BUCK_5V_EN_PIN, OUTPUT);
    digitalWrite(BUCK_5V_EN_PIN, HIGH);
    delay(400);

    inputs_init();
    if (!display_hw_init()) while(1) { digitalWrite(LED_ALIVE_PIN, !digitalRead(LED_ALIVE_PIN)); delay(100); }
    ui_manager_init();
    
    memset((void*)&real_system_data, 0, sizeof(SystemDataPacket));
}

void loop() {
    handle_serial_comms();
    inputs_update();
    
    ButtonId pressed_btn = inputs_get_last_press();
    if (pressed_btn != BTN_NONE) ui_manager_dispatch_input(pressed_btn);
    
    ui_manager_update((void*)&real_system_data);
    
    digitalWrite(LED_ALIVE_PIN, (millis() / 500) % 2);
    delay(10);
}

// --- SETUP E LOOP CORE 1 (Sensori & Backend) ---
void setup1() {
    // Inizializzazione sensori su Core 1
    sensors_i2c_init();
}

void loop1() {
    // Il Core 1 si occupa solo del campionamento dati.
    // Essendo su un loop separato, non disturba la fluidità dell'UI.
    real_system_data.uptime_s = millis() / 1000;
    real_system_data.temp_c   = sensors_read_temperature_c();
    real_system_data.battery_v = sensors_read_battery_v();
    
    delay(50); // Frequenza di campionamento sensori a 20Hz
}
