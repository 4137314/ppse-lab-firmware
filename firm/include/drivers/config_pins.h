/**
 * @file config_pins.h
 * @brief Mappatura hardware globale dei pin GPIO per RP2040 (PPSE Lab Edition).
 * @details Unico punto di verità derivato dallo schema elettrico 251127.
 */

#ifndef CONFIG_PINS_H
#define CONFIG_PINS_H

/** * @name Bus di Comunicazione (I2C & UART)
 * @{ */
// I2C0: Condiviso tra Display OLED, Accelerometro e Sensore Temp I2C
#define I2C0_SDA_PIN          12    
#define I2C0_SCL_PIN          13    

// UART1: Modulo GPS SAM-M8Q
#define GPS_TX_PIN            8     /**< UART1 TX -> RX SAM */
#define GPS_RX_PIN            9     /**< UART1 RX <- TX SAM */

// UART0: Modulo WiFi ESP-01
#define ESP_TX_PIN            0     /**< UART0 TX -> RX ESP */
#define ESP_RX_PIN            1     /**< UART0 RX <- TX ESP */
/** @} */

/** * @name Controllo Alimentazione (Power Management)
 * @{ */
#define SAM_EN_PIN            10    /**< Abilita modulo GPS (High = ON) */
#define SAM_RST_PIN           11    /**< Reset modulo GPS */
#define ESP_EN_PIN            2     /**< Abilita modulo WiFi ESP (High = ON) */
#define USB_EN_PIN            24    /**< OUTPUT_USB_EN: Gestione alimentazione USB */
#define BUCK_EN_PIN           5     /**< Abilita convertitore Buck principale */
/** @} */

/** * @name Input Utente (Pulsanti)
 * @note Dallo schema: Pull-up interni richiesti.
 * @{ */
#define BTN_UP_PIN            20    
#define BTN_DOWN_PIN          21    
#define BTN_OK_PIN            22    
// Nota: Nello schema sono indicati 3 pulsanti principali.
/** @} */

/** * @name Feedback e Periferiche
 * @{ */
#define BUZZER_PIN            23    /**< Pilotaggio Buzzer (PWM) */
#define RGB_LED_PIN           17    /**< WS2812B Data Line */
#define SAM_PPS_PIN           14    /**< Pulse Per Second dal GPS */
/** @} */

/** * @name Attuatori PWM (Servo)
 * @{ */
#define SERVO_1_PIN           18    
#define SERVO_2_PIN           19    
#define SERVO_3_PIN           25    
/** @} */

/** * @name Sensori Analogici e Interrupt
 * @{ */
#define ANALOG_BATT_PIN       26    /**< ADC0: Lettura tensione batteria */
#define TEMP_SENS_PIN         27    /**< ADC1: Sensore temperatura analogico */
#define ACC_INT1_PIN          15    /**< Interrupt 1 Accelerometro */
#define ACC_INT2_PIN          16    /**< Interrupt 2 Accelerometro */
/** @} */

#endif /* CONFIG_PINS_H */
