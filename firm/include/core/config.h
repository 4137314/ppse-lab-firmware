#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t  oled_brightness; // 0-255
    bool     buzzer_enabled;
    bool     leds_enabled;
    uint8_t  gps_refresh_rate;
} SystemConfig;

// Dichiarazione della variabile globale (da definire in un .cpp)
extern SystemConfig global_cfg;

// Funzioni di gestione
void config_init(void);
void config_save(void);

#endif
