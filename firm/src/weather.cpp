/**
 * @file weather.cpp
 * @brief Implementazione della gestione dati Meteo e parsing protocollo WX.
 * * Questo modulo gestisce la ricezione e l'organizzazione dei dati meteo
 * correnti e delle previsioni orarie (forecast) ricevute tramite stringhe
 * formattate CSV.
 */

#include "weather.h"

#include <math.h>
#include <string.h>

/** * @brief Istanza globale dello stato meteo.
 */
WeatherState wx;

/**
 * @brief Inizializza la struttura dati Meteo con valori predefiniti.
 * * Imposta la città su "NA", invalida i flag di ricezione e riempie le matrici
 * delle previsioni con NAN (Not A Number) per le temperature e 255 per i codici
 * meteo.
 */
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

/**
 * @brief Helper interno per separare i campi di una stringa CSV.
 * * @param s Stringa sorgente.
 * @param pos Posizione corrente nell'indice della stringa (aggiornata dopo la
 * chiamata).
 * @param out Stringa in uscita contenente il valore estratto.
 * @return true se il campo è stato estratto correttamente, false altrimenti.
 */
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

/**
 * @brief Parser principale per i messaggi meteo in arrivo.
 * * Gestisce due tipi di prefissi:
 * - **WXC**: (Weather Current) Dati meteo attuali (Città, Temp, Vento, Umidità,
 * Codice).
 * - **WXD**: (Weather Daily) Dati forecast per un giorno specifico (Indice, 24
 * Temp, 24 Codici).
 * * @param line Stringa grezza ricevuta dal collegamento seriale/WiFi.
 */
void Weather_HandleLine(const String& line) {
  // Parsing dati correnti
  if (line.startsWith("WXC,")) {
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

  // Parsing previsioni giornaliere
  if (line.startsWith("WXD,")) {
    int pos = 4;
    String dStr;
    if (!splitField(line, pos, dStr)) return;
    int d = dStr.toInt();
    if (d < 0 || d > 6) return;

    // Estrazione 24 temperature orarie
    for (int h = 0; h < 24; h++) {
      String v;
      if (!splitField(line, pos, v)) return;
      wx.tempC[d][h] = v.toFloat();
    }
    // Estrazione 24 codici meteo orari (WMO codes)
    for (int h = 0; h < 24; h++) {
      String v;
      if (!splitField(line, pos, v)) return;
      int code = v.toInt();
      wx.wcode[d][h] = (uint8_t)constrain(code, 0, 255);
    }

    // Aggiorna maschera bitwise per verificare se tutti i 7 giorni sono stati
    // ricevuti
    wx.recvMask |= (1u << d);
    wx.fcValid = (wx.recvMask == 0x7F);
    wx.lastUpdateMs = millis();
    return;
  }
}

/**
 * @brief Restituisce il nome abbreviato del giorno della settimana.
 * @param w Indice del giorno (0=Lun, 6=Dom).
 * @return const char* Stringa di 3 caratteri del giorno.
 */
const char* Weather_WeekdayName(uint8_t w) {
  static const char* names[7] = {"Lun", "Mar", "Mer", "Gio",
                                 "Ven", "Sab", "Dom"};
  return names[w % 7];
}

/**
 * @brief Converte il codice WMO in una descrizione testuale breve.
 * * @param code Codice meteo (0-95+).
 * @return const char* Descrizione in inglese (es. "Clear", "Rain", "Storm").
 */
const char* Weather_CodeToShortText(int code) {
  if (code == 0) return "Clear";
  if (code >= 1 && code <= 3) return "Cloud";
  if (code == 45 || code == 48) return "Fog";
  if (code >= 51 && code <= 57) return "Drizz";
  if (code >= 61 && code <= 67) return "Rain";
  if (code >= 71 && code <= 77) return "Snow";
  if (code >= 80 && code <= 82) return "Shower";
  if (code >= 95) return "Storm";
  return "WX";
}
