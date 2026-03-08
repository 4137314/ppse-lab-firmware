#include "weather.h"

#include <math.h>
#include <string.h>

WeatherState wx;

void Weather_Init() {
    wx = {};
    strcpy(wx.city, "NA");
    for (int d = 0; d < 7; d++) {
        for (int h = 0; h < 24; h++) {
            wx.tempC[d][h] = NAN;
            wx.wcode[d][h] = 255;
        }
    }
}

static bool splitField(const String& s, int& pos, String& out) {
    int comma = s.indexOf(',', pos);
    if (comma < 0) {
        out = s.substring(pos);
        pos = s.length();
        return out.length() > 0;
    }
    out = s.substring(pos, comma);
    pos = comma + 1;
    return true;
}

void Weather_HandleLine(const String& line) {
    // Parsing dati correnti
    if (line.startsWith("WXC,")) {
        int pos = 4;
        String city, t, w, hum, code, wd;
        if (!splitField(line, pos, city))
            return;
        if (!splitField(line, pos, t))
            return;
        if (!splitField(line, pos, w))
            return;
        if (!splitField(line, pos, hum))
            return;
        if (!splitField(line, pos, code))
            return;
        if (!splitField(line, pos, wd))
            return;

        city.replace(",", "_");
        city.toCharArray(wx.city, sizeof(wx.city));
        wx.curTempC     = t.toFloat();
        wx.curWindKmh   = w.toFloat();
        wx.curHumidity  = hum.toInt();
        wx.curWcode     = code.toInt();
        wx.weekday0     = (uint8_t)wd.toInt();
        wx.curValid     = true;
        wx.lastUpdateMs = millis();
        return;
    }

    // Parsing previsioni giornaliere
    if (line.startsWith("WXD,")) {
        int pos = 4;
        String dStr;
        if (!splitField(line, pos, dStr))
            return;
        int d = dStr.toInt();
        if (d < 0 || d > 6)
            return;

        // Estrazione 24 temperature orarie
        for (int h = 0; h < 24; h++) {
            String v;
            if (!splitField(line, pos, v))
                return;
            wx.tempC[d][h] = v.toFloat();
        }
        // Estrazione 24 codici meteo orari (WMO codes)
        for (int h = 0; h < 24; h++) {
            String v;
            if (!splitField(line, pos, v))
                return;
            int code       = v.toInt();
            wx.wcode[d][h] = (uint8_t)constrain(code, 0, 255);
        }

        // Aggiorna maschera bitwise per verificare se tutti i 7 giorni sono stati
        // ricevuti
        wx.recvMask |= (1u << d);
        wx.fcValid      = (wx.recvMask == 0x7F);
        wx.lastUpdateMs = millis();
        return;
    }
}

const char* Weather_WeekdayName(uint8_t w) {
    static const char* names[7] = {"Lun", "Mar", "Mer", "Gio", "Ven", "Sab", "Dom"};
    return names[w % 7];
}

const char* Weather_CodeToShortText(int code) {
    if (code == 0)
        return "Clear";
    if (code >= 1 && code <= 3)
        return "Cloud";
    if (code == 45 || code == 48)
        return "Fog";
    if (code >= 51 && code <= 57)
        return "Drizz";
    if (code >= 61 && code <= 67)
        return "Rain";
    if (code >= 71 && code <= 77)
        return "Snow";
    if (code >= 80 && code <= 82)
        return "Shower";
    if (code >= 95)
        return "Storm";
    return "WX";
}
