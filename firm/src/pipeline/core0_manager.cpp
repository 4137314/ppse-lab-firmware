#include "pipeline/core0_manager.h"

#include <Arduino.h>

#include "core/system_manager.h"
#include "core/telemetry.h"
#include "drivers/config_pins.h"
#include "drivers/display_ssd1306.h"
#include "drivers/inputs.h"
#include "drivers/peripherals.h"
#include "ui/boot_anim.h"
#include "ui/ui_manager.h"
#include "util/scheduler.h"
#include "core/config.h" // Assicurati di avere questo header che definisce la structa
#include "core/storage.h"
#include <math.h>
#include <LittleFS.h>

// DICHIARA L'ESISTENZA DELLA VARIABILE GLOBALE
extern SystemConfig global_cfg;

// Variabili per tracciamento tempo
static uint32_t last_uptime_tick = 0;

// Task schedulati
static Task task_serial = {[]() { sys_manager_handle_serial(); }, 10, 0};

static Task task_input = {[]() {
                              ButtonId b = inputs_get_last_press();
                              if (b != BTN_NONE)
                                  ui_manager_dispatch_input(b);
                          },
                          20, 0};

// Task Uptime: aggiorna il contatore nel pacchetto condiviso
static Task task_uptime = {[]() {
                               if (millis() - last_uptime_tick >= 1000) {
                                   last_uptime_tick = millis();
                                   SystemDataPacket frame;
                                   if (sys_manager_receive_data(&frame)) {
                                       frame.uptime_s++;
                                       sys_manager_send_data(&frame);
                                   }
                               }
                           },
                           100, 0};

// Task UI separato per fluidità: aggiornamento a ~30Hz
static Task task_ui = {[]() {
                           SystemDataPacket frame;
                           if (sys_manager_receive_data(&frame)) {
                               ui_manager_update(&frame);
                           }
                       },
                       33, 0};

static Task task_leds = {[]() {
                             SystemDataPacket frame;
                             if (sys_manager_receive_data(&frame)) {
                                 peripherals_auto_feedback(&frame);
                             }
                         },
                         100, 0};

static float last_logged_lat = 0.0f, last_logged_lon = 0.0f;


static Task task_gps_log = {[]() {
    SystemDataPacket frame;
    if (sys_manager_receive_data(&frame) && frame.gps_status) {
        if (fabsf(frame.latitude  - last_logged_lat) > 0.005f ||
            fabsf(frame.longitude - last_logged_lon) > 0.005f) {
            storage_log_gps_fix(frame.latitude, frame.longitude);
            last_logged_lat = frame.latitude;
            last_logged_lon = frame.longitude;
        }
    }
}, 2000, 0};

// static Task task_health = {
//     []() {
//         SystemDataPacket frame;
//         sys_manager_receive_data(&frame);
//         if (!frame.flags.error_active) {
//             Serial.printf("[HEARTBEAT] System Healthy | Uptime: %lus\n", frame.uptime_s);
//         } else {
//             Serial.printf("[HEARTBEAT] WARNING: Error Active! | Code: 0x%02X\n",
//                           frame.last_error.code);
//         }
//     },
//     10000, 0};

static Task task_health = {
    []() {
        #if defined(DEBUG) && DEBUG == 1
            SystemDataPacket frame;
            sys_manager_receive_data(&frame);
            if (!frame.flags.error_active) {
                Serial.printf("[HEARTBEAT] System Healthy | Uptime: %lus\n", frame.uptime_s);
            } else {
                Serial.printf("[HEARTBEAT] WARNING: Error Active! | Code: 0x%02X\n",
                              frame.last_error.code);
            }
        #endif
    },
    10000, 0};

void boot_led_update_wrapper() {
    static uint32_t last_led_update = 0;
    if (millis() - last_led_update > 50) {
        peripherals_update_led_fx(LED_ANIM_BOOT, 0.0f);
        last_led_update = millis();
    }
}

void core0_setup() {
    Serial.begin(115200);
    uint32_t start_time = millis();
    while (!Serial && (millis() - start_time < 1000)) delay(100);

    // Messaggi di boot condizionali (visibili solo in Debug)
    #if defined(DEBUG) && DEBUG == 1
        Serial.println("\n--- PPSE FIRMWARE SYSTEM BOOTING ---");
    #endif

    // 1. Inizializzazione configurazione (Applica i default/flag di compilazione)
    config_init();

	// if (LittleFS.begin()) {
	//        Serial.println("\n[SYSTEM] Filesystem montato.");
	//        debug_dump_gps_log();
	//    } else {
	//        Serial.println("\n[SYSTEM] ERRORE: LittleFS non montato.");
	//    }
delay(3000); // Aspetta 3 secondi, non 1
    
    Serial.println("\n\n--- INIZIO TEST DI BOOT FORZATO ---");

    // Prova ad inizializzare SOLO la Flash
    if (LittleFS.begin()) {
        Serial.println("LittleFS OK.");
    } else {
        Serial.println("LittleFS FALLITO.");
        // Se fallisce qui, il file system è corrotto a livello hardware
    }

    // Ora prova a scrivere un file di test per vedere se la flash risponde
    File f = LittleFS.open("/test.txt", "w");
    if (f) {
        f.println("Test scrittura riuscito");
        f.close();
        Serial.println("Scrittura test.txt OK.");
    } else {
        Serial.println("Scrittura test.txt FALLITA.");
    }

    // 2. Inizializzazione sottosistemi core
    sys_manager_init();
    telemetry_init();


    // 3. Setup hardware di base
    pinMode(LED_ALIVE_PIN, OUTPUT);
    digitalWrite(LED_ALIVE_PIN, HIGH);

    pinMode(BUCK_5V_EN_PIN, OUTPUT);
    digitalWrite(BUCK_5V_EN_PIN, HIGH);

    // 4. Inizializzazione periferiche (ora leggono correttamente global_cfg)
    peripherals_init();
    peripherals_init_leds();
    inputs_init();

    // 5. Setup UI
    if (!display_hw_init()) {
        sys_manager_report_error(ERR_CAT_HW, ERR_HW_DISPLAY_LOST, true);
    } else {
        run_hacker_boot_sequence(boot_led_update_wrapper);
    }

    ui_manager_init();
    
    // Messaggio finale condizionale
    #if defined(DEBUG) && DEBUG == 1
        Serial.println("--- PPSE FIRMWARE SYSTEM READY ---");
    #endif
}

void core0_loop() {
    run_task(&task_serial);
    run_task(&task_input);
    run_task(&task_uptime);  // Gestione uptime integrata
    run_task(&task_ui);
    run_task(&task_leds);
    run_task(&task_health);
    run_task(&task_gps_log);

    digitalWrite(LED_ALIVE_PIN, (millis() / 500) % 2);
}
