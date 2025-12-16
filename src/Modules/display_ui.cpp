#include "display_ui.h"

/*
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* menuItems[] = {
    "SETTINGS",
    "SENSORS",
    "GPS",
    "SYSTEM",
    "INFO"
};
const int menuLength = sizeof(menuItems)/sizeof(menuItems[0]);

unsigned long lastActivity = 0;
const unsigned long timeout = 60000; // 60 sec

// ----------------- Funzioni -----------------
// Inizializzazione OLED completa
bool displayInit() {
    bool status = true;

    // Inizializza I2C
    Wire.setSDA(DISPLAY_SDA_I2C_PIN); // Imposta GPIO0 come SDA
    Wire.setSCL(DISPLAY_SCL_I2C_PIN); // Imposta GPIO1 come SCL
    Wire.begin();
    Serial.println(F("I2C used for display initialized"));

    // Inizializzazione display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("Error: SSD1306 allocation failed"));
        status = false;
    } else {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.display();
        Serial.println(F("OLED initialized correctly"));
        drawMenu(0);
    }
    return status;
}

void drawMenu(int index) {
    display.clearDisplay();
    for(int i=0;i<menuLength;i++){
        if(i==index) display.setTextColor(SSD1306_BLACK,SSD1306_WHITE);
        else display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, i*10);
        display.println(menuItems[i]);
    }
    display.display();
}

void drawSelected(const char* title) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println(title);
    display.display();
}

// ----------------- SETTINGS SCREEN -----------------
void drawSettingsScreen() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("SETTINGS");

    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("Brightness: "); display.println(strip.getBrightness()); // da led strip
    display.setCursor(0, 35);
    display.println("Volume: 75%"); // se hai un modulo volume reale puoi sostituire
    display.setCursor(0, 45);
    display.println("Back: [LEFT]");

    display.display();
}

// ----------------- SENSORS SCREEN -----------------
void drawSensorsScreen() {
    float temp = readTemp();   // temperatura reale

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("SENSORS");

    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("Temp: "); display.print(temp, 1); display.println(" C");

    display.setCursor(0, 45);
    display.println("Back: [LEFT]");

    display.display();
}

void drawGPSScreen() {
    //TinyGPSPlus& gps = getGPS();

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("GPS Info");

    display.setTextSize(1);
    if (gps.location.isValid()) {
        display.setCursor(0, 25);
        display.print("Lat: "); display.println(gps.location.lat(), 6);
        display.setCursor(0, 35);
        display.print("Lng: "); display.println(gps.location.lng(), 6);
        display.setCursor(0, 45);
        display.print("Alt: "); display.println(gps.altitude.meters());
        display.setCursor(0, 55);
        display.print("Sat: "); display.println(gps.satellites.value());
    } else {
        display.setCursor(0, 25);
        display.println("Waiting for GPS fix...");
    }

    display.display();
}


// ----------------- SYSTEM SCREEN -----------------
void drawSystemScreen() {
    unsigned long uptimeMs = millis();
    unsigned long hours = uptimeMs / 3600000;
    unsigned long minutes = (uptimeMs / 60000) % 60;
    unsigned long seconds = (uptimeMs / 1000) % 60;

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("SYSTEM");

    display.setTextSize(1);
    display.setCursor(0, 25);
    display.println("CPU: RP2040");
    display.setCursor(0, 35);
    display.print("Uptime: "); 
    display.print(hours); display.print(":");
    if(minutes<10) display.print("0");
    display.print(minutes); display.print(":");
    if(seconds<10) display.print("0");
    display.print(seconds);
    display.setCursor(0, 45);
    display.println("Back: [LEFT]");

    display.display();
}

// ----------------- INFO SCREEN -----------------
void drawInfoScreen() {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("INFO");

    display.setTextSize(1);
    display.setCursor(0, 25);
    display.println("Firmware v1.0");   // puoi renderlo dinamico
    display.setCursor(0, 35);
    display.println("Author: Mattia");
    display.setCursor(0, 45);
    display.println("Back: [LEFT]");

    display.display();
}

// ----------------- Timeout -----------------
void updateDisplayTimeout() {
    if (!digitalRead(SW_UP) || !digitalRead(SW_DOWN) ||
        !digitalRead(SW_LEFT) || !digitalRead(SW_RIGHT)) {
        lastActivity = millis();
        display.ssd1306_command(SSD1306_DISPLAYON);
    }

    if (millis() - lastActivity > timeout) {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
    }
}
    */