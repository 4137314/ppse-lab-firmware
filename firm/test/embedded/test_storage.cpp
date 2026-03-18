#include <Arduino.h>
#include <unity.h>
#include "core/storage.h"

void test_storage_mount() {
    TEST_ASSERT_TRUE(storage_init());
}

void test_config_save_and_load() {
    struct {
        float pid_p;
        uint32_t magic;
    } test_params = {1.25f, 0xDEADBEEF}, loaded_params = {0};

    // Salviamo e rileggiamo per verificare l'integrità dei dati
    storage_save_params(&test_params, sizeof(test_params));
    bool success = storage_load_params(&loaded_params, sizeof(loaded_params));

    TEST_ASSERT_TRUE(success);
    TEST_ASSERT_EQUAL_UINT32(test_params.magic, loaded_params.magic);
    TEST_ASSERT_EQUAL_FLOAT(test_params.pid_p, loaded_params.pid_p);
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_storage_mount);
    RUN_TEST(test_config_save_and_load);
    UNITY_END();
}
void loop() {}
