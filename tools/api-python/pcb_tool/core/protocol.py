import datetime as dt

def format_wxc_line(city, temp, wind, hum, code, iso_time):
    """Genera la riga di header WXC."""
    wd0 = dt.datetime.fromisoformat(iso_time).weekday()
    city_clean = str(city).replace(",", "_").strip()[:23]
    return f"WXC,{city_clean},{temp:.1f},{wind:.1f},{int(hum)},{int(code)},{wd0}\n"

def format_wxd_lines(hourly_temps, hourly_codes):
    """Genera le 7 righe WXD (una per giorno)."""
    lines = []
    for d in range(7):
        start = d * 24
        parts = ["WXD", str(d)]
        parts += [f"{float(x):.1f}" for x in hourly_temps[start:start+24]]
        parts += [str(int(x)) for x in hourly_codes[start:start+24]]
        lines.append(",".join(parts) + "\n")
    return lines
