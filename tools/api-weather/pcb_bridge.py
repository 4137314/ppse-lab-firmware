#!/usr/bin/env python3
import sys
import serial
import time
import requests

# Configurazione Porta Seriale (cambia se necessario)
SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 115200

def fetch_weather(lat, lon):
    url = "https://api.open-meteo.com/v1/forecast"
    params = {
        "latitude": lat, "longitude": lon,
        "current": "temperature_2m,relative_humidity_2m,wind_speed_10m,weather_code",
    }
    r = requests.get(url, params=params, timeout=10)
    r.raise_for_status()
    return r.json()["current"]

def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=5)
        print(f"🔗 Connesso a {SERIAL_PORT}")
        time.sleep(2) # Attesa per stabilizzazione
        
        # 1. Richiedi GPS alla PCB
        ser.write(b"GET_GPS\n")
        line = ser.readline().decode('utf-8').strip()
        print(f"📍 Ricevuto da PCB: {line}")
        lat, lon = map(float, line.split(','))

        # 2. Ottieni meteo
        print("☁️ Interrogazione API Meteo...")
        cur = fetch_weather(lat, lon)
        
        # 3. Formatta pacchetto per PCB (WXC protocol)
        # WXC,City,Temp,Wind,Hum,Code
        wxc = f"WXC,Local,{cur['temperature_2m']},{cur['wind_speed_10m']},{cur['relative_humidity_2m']},{cur['weather_code']}\n"
        
        # 4. Invia al firmware
        ser.write(wxc.encode('utf-8'))
        print(f"✅ Inviato: {wxc.strip()}")
        
    except Exception as e:
        print(f"❌ Errore: {e}")
        sys.exit(1)
    finally:
        if 'ser' in locals(): ser.close()

if __name__ == "__main__":
    main()
