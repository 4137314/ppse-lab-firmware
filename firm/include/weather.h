/**
 * @file weather.h
 * @brief Definizioni della struttura dati e prototipi per il modulo Meteo.
 * * Questo header definisce la struttura @ref WeatherState, che funge da
 * database locale per le informazioni meteorologiche correnti e le previsioni a
 * breve termine (7 giorni).
 */

#ifndef WEATHER_H
#define WEATHER_H
#pragma once

#include <Arduino.h>

/**
 * @struct WeatherState
 * @brief Contenitore globale per lo stato meteorologico.
 * * Memorizza i dati correnti (temperatura, vento, umidità) e una matrice 7x24
 * per le previsioni orarie della settimana. Utilizza una maschera di bit
 * per tracciare la ricezione completa dei dati dal server.
 */
struct WeatherState {
    bool curValid = false; /**< True se i dati meteo correnti sono stati popolati
                              correttamente. */
    bool fcValid  = false; /**< True se tutte le previsioni (7 giorni) sono state ricevute. */
    char city[24] = "NA";  /**< Nome della città (max 23 caratteri + null terminator). */

    float curTempC   = NAN; /**< Temperatura attuale in gradi Celsius. */
    float curWindKmh = NAN; /**< Velocità del vento attuale in km/h. */
    int curHumidity  = -1;  /**< Percentuale di umidità attuale (0-100). */
    int curWcode     = -1;  /**< Codice WMO delle condizioni meteo attuali. */

    /** @name Matrici Forecast (7 giorni x 24 ore)
     * @{ */
    float tempC[7][24];   /**< Matrice delle temperature orarie previste. */
    uint8_t wcode[7][24]; /**< Matrice dei codici meteo orari previsti. */
    /** @} */

    uint8_t weekday0 = 0; /**< Indice del giorno della settimana corrente (0=Lun, 6=Dom). */
    uint8_t recvMask = 0; /**< Maschera bitwise: il bit i-esimo è 1 se il giorno i
                             è stato ricevuto. */

    uint32_t lastUpdateMs = 0; /**< Timestamp (millis) dell'ultimo aggiornamento ricevuto. */
};

/** @brief Istanza esterna della struttura meteo, accessibile da altri moduli
 * (es. UI). */
extern WeatherState wx;

/* --- Funzioni di Gestione --- */

/**
 * @brief Inizializza la struttura @ref WeatherState con valori nulli o non
 * validi.
 */
void Weather_Init();

/**
 * @brief Elabora una riga di protocollo WX (WXC o WXD) per aggiornare lo stato.
 * @param line Stringa formattata CSV proveniente dal modulo di comunicazione.
 */
void Weather_HandleLine(const String& line);

/* --- Funzioni di Supporto per UI --- */

/**
 * @brief Converte un indice (0-6) nel nome abbreviato del giorno (es. "Lun").
 * @param weekday Indice del giorno.
 * @return const char* Stringa del nome del giorno.
 */
const char* Weather_WeekdayName(uint8_t weekday);

/**
 * @brief Traduce un codice numerico WMO in una stringa breve leggibile (es.
 * "Rain").
 * @param code Codice meteorologico standard.
 * @return const char* Descrizione testuale breve.
 */
const char* Weather_CodeToShortText(int code);

#endif  // WEATHER_H
