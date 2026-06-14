#include "core/config.h"

SystemConfig global_cfg; 

void config_init(void) {
    global_cfg.oled_brightness = 150;
    global_cfg.leds_enabled = true;
    global_cfg.gps_refresh_rate = 1;

    // Se compiliamo con BUZZER_SILENT=1, il buzzer parte spento
    #ifdef DEFAULT_BUZZER_OFF
        global_cfg.buzzer_enabled = false;
    #else
        global_cfg.buzzer_enabled = true;
    #endif
}
