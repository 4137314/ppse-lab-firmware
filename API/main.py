#!/usr/bin/env python3
import os
import time
import csv
import datetime as dt
import requests

GPS_FILE = r"D:\gps_last.csv"
WX_FILE  = r"D:\wx.txt"

OPEN_METEO_URL = "https://api.open-meteo.com/v1/forecast"
NOMINATIM_URL = "https://nominatim.openstreetmap.org/reverse"


def gps_valid(path):
    try:
        with open(path, "r", encoding="utf-8", newline="") as f:
            reader = csv.reader(f)
            row = next(reader)  # può lanciare StopIteration se file vuoto

        if not row or len(row) < 2:
            return None

        lat = float(row[0])
        lon = float(row[1])

        if -90.0 <= lat <= 90.0 and -180.0 <= lon <= 180.0:
            return lat, lon

    except (StopIteration, ValueError, OSError):
        pass

    return None


def wait_for_valid_gps(path):
    print(f"Waiting for valid GPS file: {path}")
    while True:
        if os.path.exists(path):
            pos = gps_valid(path)
            if pos:
                print("Valid GPS found:", pos)
                return pos
            else:
                print("gps_last exists but not valid yet")
        time.sleep(2)


def fetch_weather(lat, lon):
    params = {
        "latitude": lat,
        "longitude": lon,
        "timezone": "auto",
        "forecast_days": 7,
        "current": "temperature_2m,relative_humidity_2m,wind_speed_10m,weather_code",
        "hourly": "temperature_2m,weather_code"
    }
    r = requests.get(OPEN_METEO_URL, params=params, timeout=15)
    r.raise_for_status()
    return r.json()


def weekday0(first_time):
    return dt.datetime.fromisoformat(first_time).weekday()  # 0=Lun .. 6=Dom


def build_lines(data, city):
    cur = data["current"]
    hourly = data["hourly"]

    temps = hourly["temperature_2m"]
    codes = hourly["weather_code"]
    times = hourly["time"]

    # serve almeno 168 ore (7*24)
    if len(temps) < 168 or len(codes) < 168 or len(times) < 168:
        raise RuntimeError("Open-Meteo did not return 7*24 hourly samples")

    wd0 = weekday0(times[0])

    lines = []
    # WXC,city,curTemp,curWind,curHum,curWcode,weekday0  (city=NA per ora)
    lines.append(
        f"WXC,{city},{cur['temperature_2m']:.1f},{cur['wind_speed_10m']:.1f},{int(cur['relative_humidity_2m'])},{int(cur['weather_code'])},{wd0}\n"
    )

    # 7 righe WXD: dayIndex + 24 temp + 24 code
    for d in range(7):
        start = d * 24
        end = start + 24
        parts = ["WXD", str(d)]
        parts += [f"{float(t):.1f}" for t in temps[start:end]]
        parts += [str(int(c)) for c in codes[start:end]]
        lines.append(",".join(parts) + "\n")

    return lines



def reverse_city(lat, lon):
    headers = {"User-Agent": "ppse-weather/1.0 (personal project)"}
    params = {
        "format": "jsonv2",
        "lat": f"{lat:.6f}",
        "lon": f"{lon:.6f}",
        "zoom": "10",
        "addressdetails": "1",
    }
    try:
        r = requests.get(NOMINATIM_URL, params=params, headers=headers, timeout=10)
        r.raise_for_status()
        js = r.json()
        addr = js.get("address", {}) if isinstance(js, dict) else {}
        city = (addr.get("city") or addr.get("town") or addr.get("village")
                or addr.get("municipality") or addr.get("county") or "NA")
        city = str(city).replace(",", "_").strip()
        return city[:23] if city else "NA"   # nel firmware city[24] :contentReference[oaicite:0]{index=0}
    except Exception:
        return "NA"
    
def write_atomic(path, lines):
    # Su alcuni FATFS/USB MSC il rename o l'estensione .tmp può fallire.
    # Scriviamo direttamente.
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.writelines(lines)
        f.flush()
        os.fsync(f.fileno())


def main():
    lat, lon = wait_for_valid_gps(GPS_FILE)

    print("Downloading weather...")
    data = fetch_weather(lat, lon)
    city = reverse_city(lat, lon)
    lines = build_lines(data, city)
    write_atomic(WX_FILE, lines)

    print("Weather file written:", WX_FILE)


if __name__ == "__main__":
    main()