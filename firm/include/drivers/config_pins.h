/**
 * @file config_pins.h
 * @brief Mappatura hardware GLOBALE e REALE dei pin GPIO per RP2040 (PPSE Lab Edition).
 * @details Versione definitiva allineata al 100% con lo schema elettrico hardware.
 * Risolve i conflitti di boot, display, selettori e bus analogici.
 */

#ifndef CONFIG_PINS_H
#define CONFIG_PINS_H

/** * @name Bus di Comunicazione (I2C & UART)
 * @{ 
 */
// I2C0: Condiviso nativamente su PCB tra EEPROM AT24C256, Display OLED SSD1306 e Accelerometro
#define I2C0_SDA_PIN        0  /**< Pin 2 RP2040 (12C0_SDA) -> Linea dati condivisa */
#define I2C0_SCL_PIN        1  /**< Pin 3 RP2040 (12C0_SCL) -> Linea clock condivisa */

// UART1: Modulo GPS SAM-M8Q
#define GPS_TX_PIN          4  /**< Pin 6 RP2040 (DUART1_TX) -> RX Modulo SAM */
#define GPS_RX_PIN          5  /**< Pin 7 RP2040 (QUART1_RX) -> TX Modulo SAM */

// UART0: Modulo WiFi ESP-01
#define ESP_TX_PIN          16 /**< Pin 27 RP2040 (DUARTO_TX) -> RX Modulo ESP */
#define ESP_RX_PIN          17 /**< Pin 28 RP2040 (QUARTO_RX) -> TX Modulo ESP */
/** @} */

/** * @name Controllo Alimentazione e Reset Moduli
 * @{ 
 */
#define SAM_EN_PIN          18 /**< Pin 29 RP2040 (DGPS_EN) -> Abilita modulo GPS (High = ON) */
#define SAM_RST_PIN         12 /**< Pin 15 RP2040 (DGPS_RST) -> Reset logico del modulo GPS */
#define ESP_EN_PIN          3  /**< Pin 5 RP2040 (DESP-01_EN) -> Power Enable modulo WiFi */
#define BUCK_5V_EN_PIN      11 /**< Pin 14 RP2040 (D5V_EN) -> ATTENZIONE: Da portare HIGH per accendere i regolatori */
#define OUTPUT_USB_EN_PIN   27 /**< Pin 39 RP2040 (DOUTPUT_USB_EN) -> Abilita output linea di potenza USB */
/** @} */

/** * @name Input Utente (Pulsanti e Interruttori)
 * @{ 
 */
#define BTN_UP_PIN          22 /**< Pin 34 RP2040 (OSW_UP) -> Tasto Navigazione Su */
#define BTN_DOWN_PIN        23 /**< Pin 35 RP2040 (JSW_DOWN) -> Tasto Navigazione Giù */
#define BTN_RIGHT_PIN       24 /**< Pin 36 RP2040 (SW_RIGHT) -> Tasto Conferma / OK */
#define BTN_LEFT_PIN        25 /**< Pin 37 RP2040 (SW_LEFT) -> Tasto Annulla / BACK */

// Alias funzionali per il framework UI
#define BTN_OK_PIN          BTN_RIGHT_PIN
#define BTN_BACK_PIN        BTN_LEFT_PIN
/** @} */

/** * @name Feedback, Stato e Linee di Interrupt
 * @{ 
 */
#define NUM_LEDS            1  /**< Numero di LED indirizzabili WS2812B integrati sulla PCB */
#define RGB_LED_PIN         13 /**< Pin 16 RP2040 (DRGB_LED) -> Linea dati per LED RGB */
#define BUZZER_PIN          21 /**< Pin 32 RP2040 (DBUZZER_PWM) -> Piezo per alert acustici */
#define LED_ALIVE_PIN       20 /**< Pin 31 RP2040 (DLED_ALIVE) -> LED di heartbeat del sistema */

#define ACC_INT1_PIN        14 /**< Pin 17 RP2040 (DINT1_ACC) -> Interrupt primario accelerometro */
#define ACC_INT2_PIN        15 /**< Pin 18 RP2040 (DINT2_ACC) -> Interrupt secondario accelerometro */
#define SAM_PPS_PIN         19 /**< Pin 30 RP2040 (GPS_PPS) -> Pulse Per Second hardware per sincronizzazione temporale */
/** @} */

/** * @name Attuatori PWM (Servo)
 * @{ 
 */
#define SERVO_1_PIN         8  /**< Pin 11 RP2040 (DPWM_SERVO1) */
#define SERVO_2_PIN         9  /**< Pin 12 RP2040 (DPWM_SERVO2) */
#define SERVO_3_PIN         7  /**< Pin 9 RP2040 (DPWM_SERVO03) */
/** @} */

/** * @name Canali Analogici (ADC Hardware)
 * @{ 
 */
#define TEMP_SENS_PIN       26 /**< Pin 38 RP2040 (ADC0) -> Sensore di temperatura analogico di bordo */
#define ANALOG_BATT_PIN     28 /**< Pin 40 RP2040 (ADC2) -> Divisore di tensione per monitoraggio batteria LiPo */
/** @} */

#endif /* CONFIG_PINS_H */
