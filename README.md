# ppse-lab-firmware

## FEATURES da implementare METEO
Prende la posizione con il gps, e la trasmette all'API che restituisce dati meteo del posto in cui ci troviamo.
I dati sono salvati sulla memoria della scheda e danno un'indicazione dell'ultimo aggiornamento (funziona anche offline).
Lo schermo serve per vedere i dati meteo e consultarli in base alle ore/giornate precedenti e sccessive. 
I led seguono il meteo corrente visualizzato. Di giorno hanno luminositá alta e di notte bassa.
Comunicazione con pc come chiavetta (filesystem) e salvataggio dati in flash.

## LATO PCB
### Boot
- Buzzer e led per segnalare accensione
- Orologio non sincronizzato, parte da una data standard, fin quando non arriva il primo dato del gps valido
- usare timepulse quando valido per aggiornare il clock al secondo.
  
### DISPLAY MENU
- Homepage come ambient display: info correnti minime, qualitá aria, temperatura, ora, ultimo aggiornamento dati
- Alla pressione di un tasto schermata home con possibili scelte
   - Info meteo, viene visualizzato il giorno corrente ed é possibile swipare fra i giorni a dx e sx con i tasti. se premuto tasto giú viene scrollata la pagina e visualizzato il meteo per le ore       della giornata selezionata prima di premere tasto giú

- Shortcuts per uscire dai menu (doppio click sinistro)
