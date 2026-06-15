/**
 * @file config_pins.h
 * @brief Mappatura hardware GLOBALE e REALE per RP2040 (PPSE Lab Edition).
 * @details Definizione univoca dei pin per il core RP2040. 
 * Include interfacce di comunicazione, controllo alimentazione e periferiche di feedback.
 * @defgroup HardwarePins Mappatura Pin Hardware
 * @{
 */

#ifndef CONFIG_PINS_H
#define CONFIG_PINS_H

/**
 * @name Bus di Comunicazione (I2C & UART)
 * @{
 */
#define I2C0_SDA_PIN        0  /**< Bus I2C: Dati */
#define I2C0_SCL_PIN        1  /**< Bus I2C: Clock */

/// UART1: Modulo GPS SAM-M8Q
#define GPS_TX_PIN          4  /**< Pin 7 RP2040 (DRX_SAM) */
#define GPS_RX_PIN          5  /**< Pin 6 RP2040 (KITX_SAM) */
#define GPSBAUD             9600 /**< Baud rate standard modulo SAM-M8Q */

/// UART0: Modulo WiFi ESP-01
#define ESP_TX_PIN          16 /**< ESP-01 TX (verso RP2040 RX) */
#define ESP_RX_PIN          17 /**< ESP-01 RX (verso RP2040 TX) */
/** @} */

/**
 * @name Controllo Alimentazione e Reset Moduli
 * @{
 */
#define SAM_EN_PIN          18 /**< Abilitazione modulo GPS */
#define SAM_RST_PIN         12 /**< Reset fisico modulo GPS */
#define ESP_EN_PIN          3  /**< Abilitazione modulo WiFi */
#define BUCK_5V_EN_PIN      11 /**< Controllo abilitazione step-down 5V */
#define OUTPUT_USB_EN_PIN   27 /**< Abilitazione porta USB di output */
/** @} */

/**
 * @name Input Utente (Pulsanti e Interruttori)
 * @{
 */
#define BTN_UP_PIN          22
#define BTN_DOWN_PIN        23
#define BTN_RIGHT_PIN       24
#define BTN_LEFT_PIN        25
#define BTN_OK_PIN          BTN_RIGHT_PIN /**< Aliasing per logica UI */
#define BTN_BACK_PIN        BTN_LEFT_PIN  /**< Aliasing per logica UI */
/** @} */

/**
 * @name Feedback, Stato e Linee di Interrupt
 * @{
 */
#define NUM_LEDS            8  /**< Numero di LED indirizzabili WS2812B */
#define RGB_LED_PIN         13 /**< Segnale data per strip LED */
#define BUZZER_PIN          21 /**< Uscita PWM per feedback sonoro */
#define LED_ALIVE_PIN       20 /**< LED di stato sistema (Heartbeat) */
#define ACC_INT1_PIN        14 /**< Interrupt 1 Accelerometro */
#define ACC_INT2_PIN        15 /**< Interrupt 2 Accelerometro */
#define SAM_PPS_PIN         19 /**< Pulse Per Second GPS (Time sync) */
/** @} */

/**
 * @name Attuatori PWM (Servo)
 * @{
 */
#define SERVO_1_PIN         8
#define SERVO_2_PIN         9
#define SERVO_3_PIN         7
/** @} */

/**
 * @name Canali Analogici (ADC Hardware)
 * @{
 */
#define TEMP_SENS_PIN       26 /**< Ingresso sensore temperatura */
#define ANALOG_BATT_PIN     28 /**< Monitoraggio tensione batteria (partitore) */
/** @} */

/** @} */ // fine del gruppo HardwarePins

#endif /* CONFIG_PINS_H */
