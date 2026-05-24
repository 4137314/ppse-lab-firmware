#include "drivers/sensors_i2c.h"
#include "drivers/config_pins.h"
#include <Arduino.h>
#include <Wire.h>

bool sensors_i2c_init() {
    // I2C è già stato inizializzato nel driver display, 
    // qui possiamo fare solo un check di presenza sensori
    return sensors_i2c_is_healthy();
}

bool sensors_i2c_is_healthy() {
    // Esempio: scansione bus
    Wire.beginTransmission(0x19); // Esempio indirizzo LIS3DH
    return (Wire.endTransmission() == 0);
}

// --- FUNZIONI DI LETTURA REALE (AGGIUNTE) ---

float sensors_read_temperature_c() {
    // Legge il sensore ADC0 (GPIO 26)
    int raw = analogRead(TEMP_SENS_PIN);
    // Formula di conversione (da calibrare in base al tuo sensore termico)
    return (raw * 3.3f / 4096.0f) * 100.0f; 
}

float sensors_read_battery_v() {
    int raw = analogRead(ANALOG_BATT_PIN);
    // Supponendo che il calcolo ti dia, per esempio, 2.5 come fattore:
    const float FATTORE_CALIBRAZIONE = 2.5f; 
    return (raw * 3.3f / 4095.0f) * FATTORE_CALIBRAZIONE;
}

bool sensors_accel_get_data(Vector3D_t* data) {
    // Qui andrebbe l'implementazione I2C per leggere il LIS3DH
    return false; 
}
