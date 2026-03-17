/**
 * @file config_pins.h
 * @brief Mappatura hardware globale dei pin GPIO per RP2040 (PPSE Lab Edition).
 * @details Versione definitiva post-refactoring: risolti conflitti UART/RGB/SERVO.
 */

#ifndef CONFIG_PINS_H
#define CONFIG_PINS_H

/** * @name Bus di Comunicazione (I2C & UART)
 * @{ */
// I2C0: Display OLED, Accelerometro, Sensore Temp I2C
#define I2C0_SDA_PIN          0
#define I2C0_SCL_PIN          1 

// UART1: Modulo GPS SAM-M8Q
#define GPS_TX_PIN            4     /**< UART1 TX -> RX SAM */
#define GPS_RX_PIN            5     /**< UART1 RX <- TX SAM */

// UART0: Modulo WiFi ESP-01
// Spostati su 12/13 per liberare 16/17 per LED e Interrupt
#define ESP_TX_PIN            12    /**< UART0 TX -> RX ESP */
#define ESP_RX_PIN            13    /**< UART0 RX <- TX ESP */
/** @} */

/** * @name Controllo Alimentazione e Reset
 * @{ */
#define SAM_EN_PIN            18    /**< Abilita modulo GPS (High = ON) */
#define SAM_RST_PIN           11    /**< Reset modulo GPS */
#define ESP_EN_PIN            2     /**< Abilita modulo WiFi ESP (High = ON) */
#define BUCK_EN_PIN           3     /**< Spostato dal 5 (conflitto GPS) */
/** @} */

/** * @name Input Utente (Pulsanti)
 * @note Mappatura sequenziale: 22(UP), 23(DOWN), 24(RIGHT), 25(LEFT)
 * @{ */
#define BTN_UP_PIN            22    /**< SWUP */
#define BTN_DOWN_PIN          23    /**< SWDOWN */
#define BTN_RIGHT_PIN         24    /**< SWRIGHT / OK */
#define BTN_LEFT_PIN          25    /**< SWLEFT / BACK */

#define BTN_OK_PIN            BTN_RIGHT_PIN
#define BTN_BACK_PIN          BTN_LEFT_PIN
/** @} */

/** * @name Feedback e Periferiche
 * @{ */
#define NUM_LEDS              1     /**< Numero di LED WS2812B a bordo */
#define RGB_LED_PIN           17    /**< WS2812B Data Line */
#define BUZZER_PIN            10    /**< Assegnato a pin libero per feedback audio */
#define SAM_PPS_PIN           14    /**< Pulse Per Second dal GPS */
/** @} */

/** * @name Attuatori PWM (Servo)
 * @{ */
// Servo 1 spostato su 20 per evitare conflitto con SAM_EN_PIN (18)
#define SERVO_1_PIN           20    
#define SERVO_2_PIN           19    
/** @} */

/** * @name Sensori Analogici e Interrupt
 * @{ */
#define ANALOG_BATT_PIN       26    /**< ADC0: Lettura tensione batteria */
#define TEMP_SENS_PIN         27    /**< ADC1: Sensore temperatura analogico */
#define ACC_INT1_PIN          15    /**< Interrupt 1 Accelerometro */
#define ACC_INT2_PIN          16    /**< Interrupt 2 Accelerometro (libero da ESP) */
/** @} */

#endif /* CONFIG_PINS_H */
