#include "init.h"

#include "display_ui.h"
#include "buzzer.h"
#include "buttons.h"
#include "sensor.h"
#include "leds.h"
#include "gps_module.h"

// Restituisce true se tutto ok
bool initHardware() {
  bool status = true; // stato globale inizializzazione

  Serial.begin(9600);
  while (!Serial) delay(10); // attende che il monitor seriale si connetta

  Serial.println(F("=> Start init hardware"));

  // I2C
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin();
  Serial.println(F("I2C initialized"));

  // Pin output
  pinMode(GPS_EN_PIN, OUTPUT);
  pinMode(TEMP_PIN, INPUT); // sensore
  pinMode(LED_ALIVE, OUTPUT);
  Serial.println(F("Pin configured"));

  // GPS
  gpsInit();

  // SENSORS 
  sensorsInit();

  // LEDS
  ledsInit();

  // OLED
  displayInit();

  buttonsInit();

  // Buzzer
  buzzerInit(BUZZER);

  Serial.println(F("=> Init completed"));
  return status;
}

