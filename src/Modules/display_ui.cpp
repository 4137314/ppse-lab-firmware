#include "display_ui.h"
#include "weather.h"
#include "gps.h"

// meteo
static uint32_t lastGpsUiMs = 0;
uint8_t gpsDayIndex = 0;
uint8_t gpsHourIndex = 0;
static void normIndices() {
    gpsDayIndex  %= 7;
    gpsHourIndex %= 24;
}


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
    return true;
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

    if(ScreenStyle == 1){
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0,0);
        display.println("Menu:");
        for(int i=0; i<menuLength; i++){
        if(i==index){
            display.print("> ");
        } else {
            display.print("  ");
        }
            display.println(menuItems[i]);
        }
        display.display();
        return;
    } else {
        drawMenu_evi(index);
    }
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


void drawGPSScreen() {
  //normIndices();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.print("GPS & Meteo");
  display.drawLine(0,10,127,10, SSD1306_WHITE);

  auto &gga = global_parsed_nmea.parsed_gga;
  auto &rmc = global_parsed_nmea.parsed_rmc;

  bool fix = (gga.fix_quality > 0) && rmc.valid &&
             (gga.latitude.scale != 0) && (gga.longitude.scale != 0);

  float lat = fix ? minmea_tocoord(&gga.latitude) : NAN;
  float lon = fix ? minmea_tocoord(&gga.longitude) : NAN;

  // Riga: City + giorno selezionato
  display.setCursor(0, 14);
  display.print("City:");
  display.print(wx.curValid ? wx.city : "NA");
  display.print(" ");
  uint8_t wd = (wx.weekday0 + gpsDayIndex) % 7;
  display.print(Weather_WeekdayName(wd));

  // Riga: Lat
  display.setCursor(0, 24);
  display.print("Lat:");
  if (fix) display.print(lat, 5); else display.print("NA");

  // Riga: Lon
  display.setCursor(0, 34);
  display.print("Lon:");
  if (fix) display.print(lon, 5); else display.print("NA");

  // Riga: Fix + sats
  display.setCursor(0, 44);
  display.print("FixQ:");
  display.print(gga.fix_quality);
  display.print(" Sat:");
  display.print(gga.satellites_tracked);

  // Riga: Meteo (ora selezionata)
  display.setCursor(0, 54);
  display.print("H");
  if (gpsHourIndex < 10) display.print("0");
  display.print(gpsHourIndex);
  display.print(" ");

  if (wx.fcValid) {
    float t = wx.tempC[gpsDayIndex][gpsHourIndex];
    int code = wx.wcode[gpsDayIndex][gpsHourIndex];
    display.print(t, 1);
    display.print("C ");
    display.print(Weather_CodeToShortText(code));
  } else if (wx.curValid) {
    display.print(wx.curTempC, 1);
    display.print("C ");
    display.print(Weather_CodeToShortText(wx.curWcode));
  } else {
    display.print("WX:NA");
  }

  display.display();
}

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

void GPSScreen_Tick() {
  // Aggiorna solo se stai davvero guardando la schermata GPS
  // Meglio: usa currentSubmenu == SUB_GPS (vedi sezione B)
  // Qui uso un flag semplice: inSubmenu + schermata GPS selezionata da te.
  
  uint32_t now = millis();
  if (now - lastGpsUiMs < 1000) return;
  lastGpsUiMs = now;

  // forza lettura gps
  GPS_poolOnce(100);

  // ridisegna
  drawGPSScreen();
}
