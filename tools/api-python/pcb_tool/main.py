import time
import sys
from rich.console import Console
from pcb_tool.hardware.discovery import find_pcb_mount
from pcb_tool.hardware.storage import read_gps_coords, write_weather_data
from pcb_tool.core.weather import fetch_weather
from pcb_tool.core.geo import reverse_city
from pcb_tool.core.protocol import format_wxc_line, format_wxd_lines

console = Console()

def main():
    console.print("[bold blue]PCB Sync Tool v1.0[/bold blue]")
    
    mount = find_pcb_mount()
    if not mount:
        console.print("[red]❌ PCB non trovata. Verifica la connessione USB.[/red]")
        sys.exit(1)
        
    coords = read_gps_coords(mount)
    if not coords:
        console.print("[yellow]⚠️ Nessun fix GPS trovato sulla PCB. Uso coordinate di default (Trento).[/yellow]")
        coords = (46.0667, 11.1167)

    try:
        with console.status("[bold green]Sincronizzazione in corso...") as status:
            lat, lon = coords
            data = fetch_weather(lat, lon)
            city = reverse_city(lat, lon)
            
            # Generazione protocollo
            h = data["hourly"]
            wxc = format_wxc_line(city, data["current"]["temperature_2m"], 
                                 data["current"]["wind_speed_10m"],
                                 data["current"]["relative_humidity_2m"],
                                 data["current"]["weather_code"],
                                 h["time"][0])
            
            wxd = format_wxd_lines(h["temperature_2m"], h["weather_code"])
            
            # Scrittura
            write_weather_data(mount, [wxc] + wxd)
            
        console.print(f"[bold green]✅ Successo![/bold green] Dati aggiornati per: [cyan]{city}[/cyan]")
        
    except Exception as e:
        console.print(f"[bold red]❌ Errore durante il sync:[/bold red] {e}")

if __name__ == "__main__":
    main()
