#include "core/system_manager.h"
#include "core/messages.h"
#include <Arduino.h>
#include <pico/sync.h>

static SystemDataPacket shared_data;
static spin_lock_t* data_lock = spin_lock_init(spin_lock_claim_unused(true));
static volatile bool _core1_ready = false;

void sys_manager_init() {
    uint32_t save = spin_lock_blocking(data_lock);
    memset(&shared_data, 0, sizeof(SystemDataPacket));
    spin_unlock(data_lock, save);
    _core1_ready = false;
}

bool sys_manager_send_data(SystemDataPacket* packet) {
    uint32_t save = spin_lock_blocking(data_lock);
    shared_data = *packet;
    spin_unlock(data_lock, save);
    return true;
}

bool sys_manager_receive_data(SystemDataPacket* buffer) {
    uint32_t save = spin_lock_blocking(data_lock);
    *buffer = shared_data;
    spin_unlock(data_lock, save);
    return true;
}

void sys_manager_report_error(error_category_t cat, error_code_t code, bool critical) {
    // 1. Log immediato sulla seriale (Push)
    Serial.printf("\n[!!!] SISTEMA: ERRORE RILEVATO!\n");
    Serial.printf("Categoria: 0x%02X | Codice: 0x%02X | Critico: %s\n", 
                  cat, code, critical ? "SI" : "NO");
    Serial.printf("Uptime: %lu ms\n\n", millis());

    // 2. Aggiornamento stato thread-safe
    uint32_t save = spin_lock_blocking(data_lock);
    shared_data.last_error = {cat, code, millis(), critical};
    shared_data.flags.error_active = 1;
    spin_unlock(data_lock, save);
}

bool sys_is_core1_ready() { return _core1_ready; }
void sys_set_core1_ready(bool ready) { _core1_ready = ready; }

void sys_manager_handle_serial(void) {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        // 1. Comando per recuperare la posizione GPS
        if (cmd == "GET_GPS") {
            SystemDataPacket frame;
            sys_manager_receive_data(&frame);
            
            // Invio con prefisso univoco GPS_DATA:
            Serial.printf("GPS_DATA:%.6f,%.6f\n", frame.latitude, frame.longitude);
        } 
        
        // 2. Comando per ricezione dati meteo (WXC,City,Temp,Wind,Hum,Code)
        else if (cmd.startsWith("WXC,")) {
            // Parsing manuale del pacchetto WXC
            int p1 = cmd.indexOf(',');
            int p2 = cmd.indexOf(',', p1 + 1);
            int p3 = cmd.indexOf(',', p2 + 1);
            int p4 = cmd.indexOf(',', p3 + 1);
            int p5 = cmd.indexOf(',', p4 + 1);

            if (p5 != -1) {
                WeatherDataPacket w; // Creiamo una struct locale temporanea
                
                // Estrazione dati nella struct locale
                cmd.substring(p1 + 1, p2).toCharArray(w.city, 32);
                w.temp_ext = cmd.substring(p2 + 1, p3).toFloat();
                w.wind_speed = cmd.substring(p3 + 1, p4).toFloat();
                w.humidity = cmd.substring(p4 + 1, p5).toInt();
                w.weather_code = cmd.substring(p5 + 1).toInt();
                w.valid = true; // Imposta come valido
                
                // Salva nel bus di sistema usando la funzione CHIRURGICA
                // Questo NON tocca il GPS o altri campi del pacchetto
                sys_manager_update_weather(w);
                
                Serial.println("OK_WXC"); 
            }
        }
        
        // 3. Comando per stato diagnostico
        else if (cmd == "GET_STATUS") {
            SystemDataPacket frame;
            sys_manager_receive_data(&frame);
            
            Serial.println("--- STATO SISTEMA ---");
            Serial.printf("Error Active: %d\n", frame.flags.error_active);
            
            if(frame.flags.error_active) {
                Serial.printf("Ultimo Errore: Cat 0x%02X, Code 0x%02X\n", 
                               frame.last_error.category, frame.last_error.code);
            } else {
                Serial.println("System Healthy.");
            }
        }
    }
}

// Funzione chirurgica per il Meteo (Usata solo dal Core 0/Seriale)
bool sys_manager_update_weather(const WeatherDataPacket& weather) {
    uint32_t save = spin_lock_blocking(data_lock);
    shared_data.weather = weather;
    shared_data.weather.valid = true;
    spin_unlock(data_lock, save);
    return true;
}

// Funzione chirurgica per il GPS (Usata solo dal Core 1)
bool sys_manager_update_gps(float lat, float lon) {
    uint32_t save = spin_lock_blocking(data_lock);
    shared_data.latitude = lat;
    shared_data.longitude = lon;
    spin_unlock(data_lock, save);
    return true;
}
