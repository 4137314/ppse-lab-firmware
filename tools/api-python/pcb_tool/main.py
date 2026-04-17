import sys
import typer
from typing import Optional
from rich.console import Console
from rich.table import Table

# Import dei tuoi moduli esistenti
from pcb_tool.hardware.discovery import find_pcb_mount
from pcb_tool.hardware.storage import read_gps_coords, write_weather_data
from pcb_tool.core.weather import fetch_weather
from pcb_tool.core.geo import reverse_city
from pcb_tool.core.protocol import format_wxc_line, format_wxd_lines

app = typer.Typer(help="PCB Sync Tool - Gestione e Sincronizzazione Hardware")
console = Console()

@app.command()
def sync(
    force_city: Optional[str] = typer.Option(None, "--city", "-c", help="Forza una città specifica invece di usare il GPS"),
    verbose: bool = typer.Option(False, "--verbose", "-v", help="Mostra dettagli tecnici del protocollo")
):
    """
    Sincronizza i dati meteo sulla PCB basandosi sulla posizione GPS attuale.
    """
    console.print("[bold blue]🔄 Avvio Sincronizzazione...[/bold blue]")
    
    mount = find_pcb_mount()
    if not mount:
        console.print("[red]❌ Errore: PCB non trovata. Controlla il cavo USB.[/red]")
        raise typer.Exit(code=1)

    # Logica di posizionamento
    if force_city:
        console.print(f"📍 Modalità manuale: [yellow]{force_city}[/yellow]")
        # Qui servirebbe una funzione geo_city_to_coords(force_city)
        coords = (46.0667, 11.1167) # Placeholder
    else:
        coords = read_gps_coords(mount) or (46.0667, 11.1167)
        if not read_gps_coords(mount):
            console.print("[yellow]⚠️ GPS non disponibile, uso coordinate di default.[/yellow]")

    try:
        with console.status("[bold green]Scaricamento dati OpenMeteo...") as status:
            lat, lon = coords
            data = fetch_weather(lat, lon)
            city = force_city or reverse_city(lat, lon)
            
            # Generazione protocollo
            h = data["hourly"]
            wxc = format_wxc_line(city, data["current"]["temperature_2m"], 
                                 data["current"]["wind_speed_10m"],
                                 data["current"]["relative_humidity_2m"],
                                 data["current"]["weather_code"],
                                 h["time"][0])
            wxd = format_wxd_lines(h["temperature_2m"], h["weather_code"])
            
            if verbose:
                console.print(f"[dim]Protocol WXC: {wxc}[/dim]")

            write_weather_data(mount, [wxc] + wxd)
            
        console.print(f"[bold green]✅ Sync Completato per {city}![/bold green]")

    except Exception as e:
        console.print(f"[bold red]❌ Errore critico:[/bold red] {e}")

@app.command()
def info():
    """
    Mostra informazioni sullo stato della PCB connessa.
    """
    mount = find_pcb_mount()
    if not mount:
        console.print("[red]PCB Scollegata.[/red]")
        return

    table = Table(title="Stato PCB")
    table.add_column("Parametro", style="cyan")
    table.add_column("Valore", style="magenta")
    
    table.add_row("Punto di Mount", mount)
    coords = read_gps_coords(mount)
    table.add_row("Coordinate GPS", str(coords) if coords else "Nessun Fix")
    
    console.print(table)

if __name__ == "__main__":
    app()
