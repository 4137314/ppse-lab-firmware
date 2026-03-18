#include <Arduino.h>
#include <unity.h>
#include "drivers/sensors_i2c.h"
#include "drivers/display_ssd1306.h"

void test_i2c_scanner_finds_devices() {
    // Verifica che almeno il display o l'accelerometro rispondano
    TEST_ASSERT_TRUE(sensors_i2c_scan());
}

void test_display_init_sequence() {
    // Se init fallisce, il driver deve restituire false
    TEST_ASSERT_TRUE(display_init());
}

void setup() {
    delay(2000); // Attesa per serial monitor
    UNITY_BEGIN();
    RUN_TEST(test_i2c_scanner_finds_devices);
    RUN_TEST(test_display_init_sequence);
    UNITY_END();
}

void loop() {}
