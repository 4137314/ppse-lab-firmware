#include <unity.h>
#include "ui/ui_manager.h"

void test_ui_navigation_flow() {
    ui_manager_init(); // Parte da VIEW_HOME
    TEST_ASSERT_EQUAL(VIEW_ID_HOME, ui_manager_get_current_view());
    
    ui_manager_navigate_to(VIEW_ID_GPS);
    TEST_ASSERT_EQUAL(VIEW_ID_GPS, ui_manager_get_current_view());
}

void test_ui_dispatch_input_bounds() {
    // Verifichiamo che input fuori range non mandino in crash il sistema
    ui_manager_dispatch_input(255); 
    TEST_ASSERT_EQUAL(VIEW_ID_GPS, ui_manager_get_current_view()); // Deve restare dov'era
}
