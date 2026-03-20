import os
import csv

def read_gps_coords(mount_point: str):
    """Legge lat/lon dall'ultimo fix GPS salvato dalla PCB."""
    path = os.path.join(mount_point, "gps_last.csv")
    try:
        with open(path, "r", encoding="utf-8") as f:
            reader = csv.reader(f)
            row = next(reader)
            return float(row[0]), float(row[1])
    except (OSError, ValueError, StopIteration):
        return None

def write_weather_data(mount_point: str, lines: list):
    """Scrive il file weather.csv in modo atomico."""
    path = os.path.join(mount_point, "weather.csv")
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.writelines(lines)
        f.flush()
        os.fsync(f.fileno()) # Forza la scrittura fisica su disco (USB)
