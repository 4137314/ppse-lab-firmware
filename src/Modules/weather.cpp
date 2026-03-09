#include "Filesystem.h"
#include "weather.h"
#include <math.h>
#include <stdio.h>
#include <string.h>


WeatherState wx;

void Weather_Init() {
    memset(&wx, 0, sizeof(wx));
    strcpy(wx.city, "NA");
    for (int d = 0; d < 7; d++) {
        for (int h = 0; h < 24; h++) {
            wx.tempC[d][h] = NAN;
            wx.wcode[d][h] = 255;
        }
    }
}

static bool splitField(const String &s, int &pos, String &out){
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
    if (line.startsWith("WXC,")) {
    // WXC,city,curTemp,curWind,curHum,curWcode,weekday0
    int pos = 4;
    String city, t, w, hum, code, wd;
    if (!splitField(line, pos, city)) return;
    if (!splitField(line, pos, t)) return;
    if (!splitField(line, pos, w)) return;
    if (!splitField(line, pos, hum)) return;
    if (!splitField(line, pos, code)) return;
    if (!splitField(line, pos, wd)) return;

    city.replace(",", "_");
    city.toCharArray(wx.city, sizeof(wx.city));
    wx.curTempC = t.toFloat();
    wx.curWindKmh = w.toFloat();
    wx.curHumidity = hum.toInt();
    wx.curWcode = code.toInt();
    wx.weekday0 = (uint8_t)wd.toInt();
    wx.curValid = true;
    wx.lastUpdateMs = millis();
    return;
  }

  if (line.startsWith("WXD,")) {
    // WXD,dayIndex,t0..t23,w0..w23  (48 valori dopo dayIndex)
    int pos = 4;
    String dStr;
    if (!splitField(line, pos, dStr)) return;
    int d = dStr.toInt();
    if (d < 0 || d > 6) return;

    // 24 temperature
    for (int h=0; h<24; h++) {
      String v;
      if (!splitField(line, pos, v)) return;
      wx.tempC[d][h] = v.toFloat();
    }
    // 24 weather_code
    for (int h=0; h<24; h++) {
      String v;
      if (!splitField(line, pos, v)) return;
      int code = v.toInt();
      wx.wcode[d][h] = (uint8_t)constrain(code, 0, 255);
    }

    wx.recvMask |= (1u << d);
    wx.fcValid = (wx.recvMask == 0x7F);  // 7 giorni ricevuti
    wx.lastUpdateMs = millis();
    return;
  }
}

const char* Weather_WeekdayName(uint8_t w) {
  static const char* names[7] = {"Lun","Mar","Mer","Gio","Ven","Sab","Dom"};
  return names[w % 7];
}
const char* Weather_CodeToShortText(int code)
{
  switch (code)
  {
    case 0: return "Clear";
    case 1: return "MainClr";
    case 2: return "Cloudy";
    case 3: return "Overcast";
  }

  if (code == 45 || code == 48) return "Fog";

  if (code >= 51 && code <= 57) return "Drizzle";

  if (code >= 61 && code <= 67) return "Rain";

  if (code >= 71 && code <= 77) return "Snow";

  if (code >= 80 && code <= 82) return "Showers";

  if (code >= 85 && code <= 86) return "SnowShw";

  if (code >= 95 && code <= 99) return "Storm";

  return "WX";
}



static void strip_crlf(char* s) {
  size_t n = strlen(s);
  while (n && (s[n-1] == '\n' || s[n-1] == '\r')) {
    s[n-1] = '\0';
    n--;
  }
}

bool Weather_LoadFromFile(const char* path) {
  // Non toccare FATFS mentre il PC ha montato la “chiavetta”
  if (inPrinting || driveConnected || updated) return false;  // :contentReference[oaicite:2]{index=2}

  inPrinting = true;

  FILE* f = fopen(path, "r");
  if (!f) { inPrinting = false; return false; }

  bool any = false;

  // Riga WXD può essere lunga: usa un buffer ampio
  char buf[1024];

  while (fgets(buf, sizeof(buf), f)) {
    strip_crlf(buf);
    if (buf[0] == '\0') continue;

    // Popola wx (WXC/WXD) usando il tuo parser
    Weather_HandleLine(String(buf));  // :contentReference[oaicite:3]{index=3}
    any = true;
  }

  fclose(f);
  inPrinting = false;

  return any;
}