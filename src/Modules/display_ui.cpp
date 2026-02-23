#include "display_ui.h"
#include "weather.h"
#include "gps.h"

extern volatile bool gpsDirty;
extern struct parsed_nmea gpsData; // global struct with the last gps data parsed
// meteo
static uint32_t lastGpsUiMs = 0;
uint8_t gpsDayIndex = 0;
uint8_t gpsHourIndex = 0;

//struct parsed_nmea gpsData; // global struct with the last gps data parsed

static inline bool dayHasForecast(uint8_t d) {
    return (wx.recvMask & (1u << (d))) != 0;
}

static uint8_t settingsIndex = 0; // 0 = Menu Style (per ora)
uint8_t ScreenStyle = 1; // 1 = Simple, 0 = Highlight


// Definizioni REALI (non extern)
const char* menuItems[] = {
    "Settings",
    "Meteo",
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
static void print2d(uint8_t v){
    if (v < 10) display.print("0");
        display.print(v);
}

void drawHomeScreen(){
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Weather:");
    display.drawLine(0,10,127,10, SSD1306_WHITE);
    display.setCursor(0, 14);
    display.print(wx.curValid ? wx.city : "NA");
    display.setCursor(0, 24);
    if (wx.curValid) {
        display.print(wx.curTempC, 1);
        display.print("C ");
        display.print(Weather_CodeToShortText(wx.curWcode));

    } else {
        display.print("NA");
    }
    display.setCursor(0, 34);
    display.print("Hum: ");
    if (!wx.curValid || isnan(wx.curHumidity)) display.print("--");
    else { display.print((int)roundf(wx.curHumidity)); display.print("%"); }
    display.print("City:");


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


void drawMeteoScreen() { 
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);    

    // Titolo
    display.setCursor(0,0);
    display.print("Meteo:");
    display.drawLine(0,10,127,10, SSD1306_WHITE);
    display.setCursor(0, 14);

    // Citta+giorno
    display.print("City:");
    display.print(wx.curValid ? wx.city : "NA");
    display.print(" ");
    uint8_t wd = (wx.weekday0 + gpsDayIndex) % 7;
    display.print(Weather_WeekdayName(wd));

    // Riga: giorno selezionato + ora selezionata
    display.setCursor(0, 24);
    display.print("g");
    display.print(gpsDayIndex);
    display.print(" h");
    display.print(gpsHourIndex);

    display.setCursor(0, 34);

    if (dayHasForecast(gpsDayIndex)) {
    float t = wx.tempC[gpsDayIndex][gpsHourIndex];
    uint8_t code = wx.wcode[gpsDayIndex][gpsHourIndex];

    display.print("Fc: ");
    if (isnan(t)) display.print("--");
    else { display.print(t, 1); display.print("C "); }

    if (code == 255) display.print("--");
    else display.print(Weather_CodeToShortText(code));
  } else {
    display.print("Fc: no data");
  }

  // Riga 4: Now (corrente) + Humidita corrente
  display.setCursor(0, 46);
  display.print("Now: ");
  if (wx.curValid) {
    display.print(wx.curTempC, 1);
    display.print("C ");
    display.print(Weather_CodeToShortText(wx.curWcode));
  } else {
    display.print("NA");
  }

  display.setCursor(0, 56);
  display.print("Hum: ");
  // usa il NOME che hai realmente in WeatherState:
  // se lo hai chiamato curHumPct:
  if (!wx.curValid || isnan(wx.curHumidity)) display.print("--");
  else { display.print((int)roundf(wx.curHumidity)); display.print("%"); }

  display.display();


  Serial.println(gpsDayIndex);
  Serial.println(gpsHourIndex);
   
}


void drawGPSScreen() {
  //normIndices();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.print("GPS & Meteo");
  display.drawLine(0,10,127,10, SSD1306_WHITE);

  struct minmea_sentence_rmc rmc = gpsData.parsed_rmc;
  struct minmea_sentence_gga gga = gpsData.parsed_gga;

  bool fix = (gga.fix_quality > 0) && rmc.valid &&
             (gga.latitude.scale != 0) && (gga.longitude.scale != 0);

  float lat = minmea_tocoord(&gga.latitude);
  float lon = minmea_tocoord(&gga.longitude);

  // Riga: City + giorno selezionato
  // Riga: Lat
  display.setCursor(0, 14);
  display.print("Lat:");
  if (fix) display.print(lat, 5); else display.print("NA");

  // Riga: Lon
  display.setCursor(0, 24);
  display.print("Lon:");
  if (fix) display.print(lon, 5); else display.print("NA");

  // Riga: Fix + sats
  display.setCursor(0, 34);
  display.print("FixQ:");
  display.print(gga.fix_quality);
  display.print(" Sat:");
  display.print(gga.satellites_tracked);

  // Riga: Meteo (ora selezionata)
  display.setCursor(0, 44);
  display.print("Time: ");
  display.print(gga.time.hours + 1);
  display.print(":");
  display.print(gga.time.minutes);
  display.print(":");
  display.print(gga.time.seconds);
  display.display();
  display.setCursor(40, 14);
  display.print("Date: ");
  display.print(rmc.date.day);
  display.print("/");
  display.print(rmc.date.month);
  display.print("/");
  display.print(rmc.date.year);
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
