#include "display_ui.h"

//enum ScreenStyle : uint8_t {
    //  STYLE_CLASSIC = 0,
    //  STYLE_ROUNDED = 1
    //};

static uint8_t settingsIndex = 0; // 0 = Menu Style (per ora)
uint8_t ScreenStyle = 1; // 1 = Simple, 0 = Highlight


// Definizioni REALI (non extern)
const char* menuItems[] = {
  "Settings",
  "Sensors",
  "GPS",
  "System",
  "Info"
};

const int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


bool displayInit(){
    Wire.setSDA(DISPLAY_SDA_I2C_PIN);
    Wire.setSCL(DISPLAY_SCL_I2C_PIN);
    Wire.begin();
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false ; // Exit if display initialization fails
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.display();
}

void drawHomeScreen(){
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Home Screen");
    display.display();
}

void drawMenu(int index){

}

void drawMenu_evi(int index) {
    display.clearDisplay();
    display.setTextSize(1);

    // Titolo
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Menu");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    // Layout
    const int firstY = 14;   // inizio lista
    const int lineH  = 10;   // altezza riga (8px font + spazio)
    const int boxH   = 9;    // altezza box evidenziato

    for (int i = 0; i < menuLength; i++) {
        int y = firstY + i * lineH;

        if (i == index) {
            // rettangolo evidenziato
            display.fillRect(0, y - 1, 128, boxH, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK); // testo nero sul bianco
        } else {
            display.setTextColor(SSD1306_WHITE);
        }

        display.setCursor(2, y);
        display.print(menuItems[i]);
    }

    display.display();
}




// display_ui.cpp

void drawSettingsScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.print("Settings");
  display.drawLine(0,10,127,10, SSD1306_WHITE);

  display.setCursor(0, 16);
  display.print(settingsIndex == 0 ? "> " : "  ");
  display.print("Menu style: ");
  display.print(ScreenStyle == 1 ? "Simple" : "Highlight");

  display.setCursor(0, 54);
  display.print("R=Change  L=Back");
  display.display();
}


void drawSensorsScreen() { drawSelected("Sensors"); }
void drawGPSScreen()     { drawSelected("GPS"); }
void drawSystemScreen()  { drawSelected("System"); }
void drawInfoScreen()    { drawSelected("Info"); }

void drawSelected(const char* title){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Selected:");
  display.setCursor(0,12);
  display.print(title);
  display.display();
}

void updateDisplayTimeout(){
    if(menuOpen || inSubmenu){
        if(millis()- lastActivity > DISPLAY_TIMEOUT_MS){
            menuOpen = false;
            inSubmenu = false;
            ambientDisplay = true;
            drawHomeScreen(); 
        }
    }
}

void setBrightness(uint8_t level){
    // Placeholder: Implement brightness control if hardware supports it
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(level);
    // level 0-255
}

