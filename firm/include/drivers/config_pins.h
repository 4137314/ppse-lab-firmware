/**
 * @file config_pins.h
 * @brief Mappatura hardware GLOBALE e REALE per RP2040 (PPSE Lab Edition).
 * @details Allineato allo schema: UART1(TX=5, RX=4). Include tutti i pin definiti.
 */

#ifndef CONFIG_PINS_H
#define CONFIG_PINS_H

/** * @name Bus di Comunicazione (I2C & UART)
 * @{ 
 */
#define I2C0_SDA_PIN        0
#define I2C0_SCL_PIN        1

// UART1: Modulo GPS SAM-M8Q (Corretti su schema: TX=5, RX=4)
#define GPS_TX_PIN          4  /**< Pin 7 RP2040 (DRX_SAM) */
#define GPS_RX_PIN          5  /**< Pin 6 RP2040 (KITX_SAM) */
#define GPSBAUD             9600

// UART0: Modulo WiFi ESP-01
#define ESP_TX_PIN          16
#define ESP_RX_PIN          17
/** @} */

/** * @name Controllo Alimentazione e Reset Moduli
 * @{ 
 */
#define SAM_EN_PIN          18
#define SAM_RST_PIN         12
#define ESP_EN_PIN          3
#define BUCK_5V_EN_PIN      11
#define OUTPUT_USB_EN_PIN   27
/** @} */

/** * @name Input Utente (Pulsanti e Interruttori)
 * @{ 
 */
#define BTN_UP_PIN          22
#define BTN_DOWN_PIN        23
#define BTN_RIGHT_PIN       24
#define BTN_LEFT_PIN        25
#define BTN_OK_PIN          BTN_RIGHT_PIN
#define BTN_BACK_PIN        BTN_LEFT_PIN
/** @} */

/** * @name Feedback, Stato e Linee di Interrupt
 * @{ 
 */
#define NUM_LEDS            8  /**< Numero di LED indirizzabili WS2812B */
#define RGB_LED_PIN         13
#define BUZZER_PIN          21
#define LED_ALIVE_PIN       20
#define ACC_INT1_PIN        14
#define ACC_INT2_PIN        15
#define SAM_PPS_PIN         19
/** @} */

/** * @name Attuatori PWM (Servo)
 * @{ 
 */
#define SERVO_1_PIN         8
#define SERVO_2_PIN         9
#define SERVO_3_PIN         7
/** @} */

/** * @name Canali Analogici (ADC Hardware)
 * @{ 
 */
#define TEMP_SENS_PIN       26
#define ANALOG_BATT_PIN     28
/** @} */

#endif /* CONFIG_PINS_H */
