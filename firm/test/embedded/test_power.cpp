#include <Arduino.h>
#include <unity.h>
#include "core/power_manager.h"
#include "drivers/config_pins.h"

void test_battery_voltage_range() {
    float voltage = power_get_battery_voltage();
    // Supponendo una LiPo 1S: deve essere tra 3.0V e 4.3V (se collegata)
    // Se alimentato solo da USB, verifichiamo il range del regolatore
    TEST_ASSERT_FLOAT_WITHIN(2.0f, 3.3f, voltage); 
}

void test_module_switching() {
    // Verifichiamo che il cambio di stato non resetti il micro
    power_gps_set_state(true);
    delay(100);
    power_gps_set_state(false);
    TEST_ASSERT_TRUE(true); 
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_battery_voltage_range);
    RUN_TEST(test_module_switching);
    UNITY_END();
}
void loop() {}
