from rich.console import Console
from rich.table import Table

console = Console()

def print_weather_report(city, data):
    table = Table(title=f"Meteo per {city}")
    table.add_column("Parametro", style="cyan")
    table.add_column("Valore", style="magenta")
    
    cur = data["current"]
    table.add_row("Temperatura", f"{cur['temperature_2m']}°C")
    table.add_row("Vento", f"{cur['wind_speed_10m']} km/h")
    table.add_row("Umidità", f"{cur['relative_humidity_2m']}%")
    
    console.print(table)
