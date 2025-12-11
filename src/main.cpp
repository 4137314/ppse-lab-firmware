//RELAZIONE 1 15-30pg, descrivere circuiti piú che 
// componenti. Anno mese giorno come data

#define UART0_TX_PIN 16
#define UART0_RX_PIN 17

#define DISPLAY_SDA_I2C_PIN 0
#define DISPLAY_SCL_I2C_PIN 1

#define TEMP_PIN 26 //GPIO temperature sensor

#define ADC_BITS 12
#define ADC_MAX_VAL 4095 // =2^(ADC_BITS) -1

#define GPS_EN_PIN 18   // Enable GPS module
//#define GPS_RX_PIN 19  // GPIO connected to GPS' tx pin
#define LED_ALIVE 20    // Led alive GPIO pin
//UART1 for GPS comm
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

#define SCREEN_WIDTH 128 // OLED display width in pixels
#define SCREEN_HEIGHT 64 // OLED display height in pixels
#define OLED_RESET -1 // reset pin to unset (not available)
#define SCREEN_ADDRESS 0x3C //screen address for Adafruit library

#include "Functions.cpp"

void main(){

  free(STR);
}
