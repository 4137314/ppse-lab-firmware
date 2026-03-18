#include <unity.h>
#include "core/error_codes.h"

void test_error_report_initialization() {
    error_report_t report = {ERR_CAT_POWER, ERR_PWR_BATT_LOW, 1000, false};
    
    TEST_ASSERT_EQUAL(ERR_CAT_POWER, report.category);
    TEST_ASSERT_EQUAL(ERR_PWR_BATT_LOW, report.code);
    TEST_ASSERT_FALSE(report.is_critical);
}

void test_critical_error_flag() {
    error_report_t report = {ERR_CAT_SYS, ERR_SYS_WATCHDOG, 5000, true};
    
    // Verifichiamo che il sistema riconosca la criticità per attivare il failsafe
    TEST_ASSERT_TRUE(report.is_critical);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_error_report_initialization);
    RUN_TEST(test_critical_error_flag);
    return UNITY_END();
}
