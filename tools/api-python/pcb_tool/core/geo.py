import requests

NOMINATIM_URL = "https://nominatim.openstreetmap.org/reverse"

def reverse_city(lat: float, lon: float) -> str:
    headers = {"User-Agent": "pcb-tool-uni/1.0"}
    params = {"format": "jsonv2", "lat": lat, "lon": lon, "zoom": 10}
    try:
        r = requests.get(NOMINATIM_URL, params=params, headers=headers, timeout=10)
        r.raise_for_status()
        addr = r.json().get("address", {})
        # Prende il primo nome valido tra città, paese o villaggio
        city = addr.get("city") or addr.get("town") or addr.get("village") or "Unknown"
        return city
    except Exception:
        return "NA"
