/**
 * @file display_ui.cpp
 * @brief Implementazione dell'interfaccia utente (UI) su display OLED SSD1306.
 * * Gestisce il rendering delle schermate Home, Menu, Meteo e GPS. Include la
 * logica per il cambio di stile del menu (Simple vs Highlight) e il timeout del
 * display.
 */

#include "display_ui.h"

#include "gps.h"
#include "weather.h"

/** @name Stato Interno UI */
/**@{*/
extern volatile bool gpsDirty; /**< Flag che indica la presenza di nuovi dati
                                  GPS da processare. */
extern struct parsed_nmea
    gpsData; /**< Struttura globale contenente gli ultimi dati NMEA parsati. */

static uint32_t lastGpsUiMs  = 0; /**< Timestamp dell'ultimo aggiornamento della UI GPS. */
uint8_t gpsDayIndex          = 0; /**< Indice del giorno selezionato per le previsioni meteo. */
uint8_t gpsHourIndex         = 0; /**< Indice dell'ora selezionata per le previsioni meteo. */
static uint8_t settingsIndex = 0; /**< Indice della voce selezionata nel menu Settings. */
uint8_t ScreenStyle          = 1; /**< Stile del menu: 1 = Semplice, 0 = Evidenziato (Highlight). */
/**@}*/

/** * @brief Voci del menu principale.
 */
const char* menuItems[] = {"Settings", "Meteo", "GPS", "System", "Info"};

/** @brief Numero totale di voci nel menu. */
const int menuLength = sizeof(menuItems) / sizeof(menuItems[0]);

/** @brief Istanza del driver del display SSD1306. */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/**
 * @brief Inizializza l'hardware del display e il bus I2C.
 * @return true Se l'inizializzazione ha successo, false altrimenti.
 */
bool displayInit() {
    Wire.setSDA(DISPLAY_SDA_I2C_PIN);
    Wire.setSCL(DISPLAY_SCL_I2C_PIN);
    Wire.begin();
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;  // Exit if display initialization fails
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.display();
    return true;
}

/**
 * @brief Helper per stampare numeri a due cifre con zero iniziale (es. "09").
 * @param v Valore da stampare.
 */
static void print2d(uint8_t v) {
    if (v < 10)
        display.print("0");
    display.print(v);
}

/**
 * @brief Verifica se sono disponibili dati meteo per il giorno specificato.
 * @param d Indice del giorno.
 * @return true Se i dati sono presenti nel bitmask.
 */
static inline bool dayHasForecast(uint8_t d) {
    return (wx.recvMask & (1u << (d))) != 0;
}

/**
 * @brief Disegna la schermata principale (Ambient Display).
 * Mostra i dati meteo correnti, l'ora e la data recuperate dal GPS.
 */
void drawHomeScreen() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Weather:");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    display.setCursor(0, 14);
    display.print("City: ");
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
    if (!wx.curValid || isnan(wx.curHumidity))
        display.print("--");
    else {
        display.print((int)roundf(wx.curHumidity));
        display.print("%");
    }
    display.setCursor(0, 44);
    struct minmea_sentence_rmc rmc = gpsData.parsed_rmc;
    struct minmea_sentence_gga gga = gpsData.parsed_gga;
    display.print("Time: ");
    display.print(gga.time.hours);
    display.print(":");
    display.print(gga.time.minutes);
    display.print(":");
    display.print(gga.time.seconds);
    display.setCursor(0, 54);
    display.print("Date: ");
    display.print(rmc.date.day);
    display.print("/");
    display.print(rmc.date.month);
    display.print("/");
    display.print(rmc.date.year);

    display.display();
}

/**
 * @brief Renderizza il menu principale in base allo stile selezionato (@ref
 * ScreenStyle).
 * @param index Indice dell'elemento da evidenziare.
 */
void drawMenu(int index) {
    if (ScreenStyle == 1) {
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Menu:");
        for (int i = 0; i < menuLength; i++) {
            if (i == index) {
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

/**
 * @brief Renderizza il menu in modalità "Highlight" (barra piena e testo
 * invertito).
 * @param index Indice dell'elemento da evidenziare.
 */
void drawMenu_evi(int index) {
    display.clearDisplay();
    display.setTextSize(1);

    // Titolo
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Menu");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    // Layout
    const int firstY = 14;  // inizio lista
    const int lineH  = 10;  // altezza riga (8px font + spazio)
    const int boxH   = 9;   // altezza box evidenziato

    for (int i = 0; i < menuLength; i++) {
        int y = firstY + i * lineH;

        if (i == index) {
            // rettangolo evidenziato
            display.fillRect(0, y - 1, 128, boxH, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK);  // testo nero sul bianco
        } else {
            display.setTextColor(SSD1306_WHITE);
        }

        display.setCursor(2, y);
        display.print(menuItems[i]);
    }

    display.display();
}

/**
 * @brief Disegna la schermata dei dati GPS in tempo reale.
 * Mostra coordinate, qualità del fix e numero di satelliti.
 */
void drawSettingsScreen() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("Settings");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    display.setCursor(0, 16);
    display.print(settingsIndex == 0 ? "> " : "  ");
    display.print("Menu style: ");
    display.print(ScreenStyle == 1 ? "Simple" : "Highlight");

    display.setCursor(0, 54);
    display.print("R=Change  L=Back");
    display.display();
}

/**
 * @brief Renderizza la schermata dettagliata del Meteo.
 * * Visualizza la città, il giorno della settimana calcolato dinamicamente e le
 * previsioni (forecast) per l'ora e il giorno selezionati tramite i pulsanti.
 * Mostra inoltre le condizioni attuali (temperatura e umidità) recuperate dal
 * server.
 */
void drawMeteoScreen() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Titolo
    display.setCursor(0, 0);
    display.print("Meteo:");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
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
        float t      = wx.tempC[gpsDayIndex][gpsHourIndex];
        uint8_t code = wx.wcode[gpsDayIndex][gpsHourIndex];

        display.print("Fc: ");
        if (isnan(t))
            display.print("--");
        else {
            display.print(t, 1);
            display.print("C ");
        }

        if (code == 255)
            display.print("--");
        else
            display.print(Weather_CodeToShortText(code));
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
    if (!wx.curValid || isnan(wx.curHumidity))
        display.print("--");
    else {
        display.print((int)roundf(wx.curHumidity));
        display.print("%");
    }

    display.display();

    Serial.println(gpsDayIndex);
    Serial.println(gpsHourIndex);
}

/**
 * @brief Renderizza la schermata dei dati GPS e temporali.
 * * Estrae e formatta le coordinate geografiche (Latitudine/Longitudine),
 * la qualità del fix e le informazioni satellitari dalle sentenze NMEA GGA e
 * RMC. Include un'indicazione per il salvataggio dei log su SD/Flash.
 */
void drawGPSScreen() {
    // normIndices();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("GPS & Meteo");
    display.setCursor(50, 0);
    display.print("R = Save data to log");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    struct minmea_sentence_rmc rmc = gpsData.parsed_rmc;
    struct minmea_sentence_gga gga = gpsData.parsed_gga;

    bool fix = gga.fix_quality > 0;

    float lat = minmea_tocoord(&gga.latitude);
    float lon = minmea_tocoord(&gga.longitude);

    // Riga: Lat
    display.setCursor(0, 14);
    display.print("Lat:");
    if (fix)
        display.print(lat, 5);
    else
        display.print("NA");

    // Riga: Lon
    display.setCursor(0, 24);
    display.print("Lon:");
    if (fix)
        display.print(lon, 5);
    else
        display.print("NA");

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
    display.setCursor(0, 54);
    display.print("Date: ");
    display.print(rmc.date.day);
    display.print("/");
    display.print(rmc.date.month);
    display.print("/");
    display.print(rmc.date.year);
    display.display();
}

/**
 * @brief Schermata generica per le voci di menu non ancora implementate.
 * @param title Il titolo della sezione selezionata.
 */
void drawSelected(const char* title) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Selected:");
    display.setCursor(0, 12);
    display.print(title);
    display.display();
}

/** @brief Renderizza la schermata System. */
void drawSystemScreen() {
    drawSelected("System");
}
/** @brief Renderizza la schermata Info. */
void drawInfoScreen() {
    drawSelected("Info");
}

/**
 * @brief Gestisce il timeout del display per l'inattività utente.
 * * Se il menu è aperto o l'utente è in un sottomenu, verifica il tempo
 * trascorso dall'ultima interazione. Superato il limite @ref
 * DISPLAY_TIMEOUT_MS, riporta il sistema alla @ref drawHomeScreen.
 */
void updateDisplayTimeout() {
    if (menuOpen || inSubmenu) {
        if (millis() - lastActivity > DISPLAY_TIMEOUT_MS) {
            menuOpen       = false;
            inSubmenu      = false;
            ambientDisplay = true;
            drawHomeScreen();
        }
    }
}

/**
 * @brief Regola il contrasto (luminosità) del display OLED.
 * @param level Livello di contrasto da 0 a 255.
 */
void setBrightness(uint8_t level) {
    // Placeholder: Implement brightness control if hardware supports it
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(level);
    // level 0-255
}
