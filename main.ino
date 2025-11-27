


#define UART0_TX_PIN 16
#define UART0_RX_PIN 17

#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

//#define RXPIN 4 //pin RX tinyGPS
//#define TXPIN -1

#include <SPI.h>
#include <Wire.h> //I2C lib
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdint.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 //reset pin shared unset
#define SCREEN_ADDRESS 0x3c



Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  // put your setup code here, to run once:
  pinMode(18,OUTPUT); // enable GPS module
  digitalWrite (18, 0); //bring down GPS_EN signal to enable
  Serial2.setTX(4);
  Serial2.setRX(5);
  Serial2.begin(9600); // enable uart1 on 4 and 5



  
  pinMode(20,OUTPUT); // enable led alive

  Serial.begin(9600); // seriale virtuale over USB
  Wire.setSDA(0); // Imposta GP0 come SDA
  Wire.setSCL(1); // Imposta GP1 come SCL
  Wire.begin(); //

   
  // initialize the OLED object

  digitalWrite(12,0);

 if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {

   Serial.println(F("SSD1306 allocation failed"));

   for(;;); // Don't proceed, loop forever

 }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RP2040 I2C su GP0/GP1");
  display.display(); // Mostra il contenuto del buffer sul display
}


void loop() {
  //digitalWrite (20, !digitalRead(20));
  String str="";
  int Byte;
  if( Serial2.available() > 0) //check buffer of serial (max 64B)
  { 
    digitalWrite(20, 1);
    str=Serial2.readStringUntil('\n');
      digitalWrite(20, 0);
  }
  if (str!="")
  {
    Serial.print(str);
  }

}

