import requests

OPEN_METEO_URL = "https://api.open-meteo.com/v1/forecast"

def fetch_weather(lat, lon):
    params = {
        "latitude": lat, "longitude": lon,
        "timezone": "auto", "forecast_days": 7,
        "current": "temperature_2m,relative_humidity_2m,wind_speed_10m,weather_code",
        "hourly": "temperature_2m,weather_code"
    }
    r = requests.get(OPEN_METEO_URL, params=params, timeout=15)
    r.raise_for_status()
    return r.json()
