/**
 * @file config_pins.h
 * @brief Mappatura hardware globale dei pin GPIO per RP2040.
 * @details Questo file è l'unico punto di verità per i collegamenti fisici.
 * Definisce i pin per sensori, display, input e feedback.
 */

#ifndef CONFIG_PINS_H
#define CONFIG_PINS_H

/** * @name Interfacce di Comunicazione
 * @{ */
#define DISPLAY_SDA_PIN      0     /**< I2C0 SDA: Display OLED SSD1306. */
#define DISPLAY_SCL_PIN      1     /**< I2C0 SCL: Display OLED SSD1306. */

#define GPS_TX_PIN          16     /**< UART0 TX: Modulo GPS (evita conflitti su UART1). */
#define GPS_RX_PIN          17     /**< UART0 RX: Modulo GPS. */
/** @} */

/** * @name Input Utente (Pulsanti)
 * @{ */
#define BTN_UP_PIN          22     /**< GPIO per pulsante Navigazione SU. */
#define BTN_DOWN_PIN        23     /**< GPIO per pulsante Navigazione GIÙ. */
#define BTN_OK_PIN          24     /**< GPIO per pulsante OK / Conferma. */
#define BTN_BACK_PIN        25     /**< GPIO per pulsante BACK / Indietro. */
/** @} */

/** * @name Feedback e Periferiche
 * @{ */
#define BUZZER_PIN          21     /**< GPIO per Buzzer piezoelettrico (PWM). */
#define LED_STRIP_PIN       13     /**< GPIO per trasmissione dati striscia LED RGB. */
/** @} */

/** * @name Sensori Analogici
 * @{ */
#define TEMP_ADC_PIN        26     /**< ADC0: Sensore di temperatura analogico. */
/** @} */

#endif /* CONFIG_PINS_H */
