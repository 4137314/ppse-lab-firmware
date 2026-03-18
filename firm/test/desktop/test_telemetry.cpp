#include <unity.h>
#include "core/telemetry.h"
#include "lib/minmea/minmea.h"

void test_gps_parsing_valid_rmc() {
    const char* rmc_sentence = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
    
    // Simuliamo l'arrivo dei dati nel parser
    // (Presuppone che telemetry_update_from_buffer sia agnostica)
    bool success = telemetry_parse_line(rmc_sentence); 
    
    TEST_ASSERT_TRUE(success);
    // Verifica che i dati estratti siano corretti
    TEST_ASSERT_EQUAL_FLOAT(48.1173, telemetry_get_current_frame().latitude);
}

void test_invalid_checksum_should_fail() {
    const char* bad_sentence = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*00";
    TEST_ASSERT_FALSE(telemetry_parse_line(bad_sentence));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_gps_parsing_valid_rmc);
    RUN_TEST(test_invalid_checksum_should_fail);
    return UNITY_END();
}
