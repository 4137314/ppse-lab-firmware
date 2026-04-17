# RP2040 GPS & Environment Logger

Firmware avanzato per **RP2040** dedicato al logging geospaziale, monitoraggio ambientale e telemetria in tempo reale.

---

## Core Features

### GPS & Telemetry Engine
*   **Precision Navigation:** Integrazione con moduli **u-blox** (SAM-M8Q) per il parsing di protocolli NMEA e UBX.
*   **Multi-Core Telemetry:** Gestione dell'acquisizione dati e del parsing sul **Core 1** per garantire l'integrità del flusso senza latenze nell'interfaccia utente.
*   **Real-time Snapshot:** Utilizzo della struttura atomica `SystemDataPacket` per tracciare coordinate, altitudine, velocità e stato dei satelliti.
*   **Fix Diagnostics:** Monitoraggio dello stato del segnale (IDLE, No Fix, 2D/3D) con calcolo dell'età dell'ultimo fix per la sicurezza del volo.

### Hybrid Storage System
*   **Dual-Mode Access:** Gestione dati su Flash interna tramite **FatFS** e interfaccia USB Mass Storage per l'estrazione rapida dei log su PC.
*   **Advanced Logging:** Sistema di append automatico per la telemetria in formato CSV (`telemetry_log.csv`) e salvataggio dei parametri di configurazione in formato binario.
*   **Safety Lock:** Protezione del filesystem con rilevamento automatico della connessione USB per prevenire corruzioni durante la scrittura.

### UI & UX Framework
*   **Agnostic Display Driver:** Interfaccia grafica su OLED **SSD1306** (128x64) con astrazione completa dalle librerie hardware per facilitare il porting.
*   **State Machine UI:** Gestione delle schermate tramite un orchestratore (`ui_manager`) che gestisce la navigazione tra Home, GPS, Meteo e Impostazioni.
*   **Multisensory Feedback:** 
    *   **Visual:** LED RGB (WS2812B) per indicazioni di stato (Success, Error, Neutral).
    *   **Acoustic:** Buzzer piezoelettrico per allarmi critici e feedback alla pressione dei tasti.
    *   **Input:** Gestione di 4 tasti fisici con debounce software integrato.

---

## Risorse e Documentazione

Il progetto adotta un approccio [**"Documentation as Code"**](https://en.wikipedia.org/wiki/Self-documenting_code). Tutte le risorse tecniche sono centralizzate e aggiornate automaticamente ad ogni release tramite il portale ufficiale:

* **[Project Documentation Portal](https://4137314.github.io/ppse-lab-firmware/)**
  *Punto di accesso unico per consultare il report accademico, l'architettura del sistema e lo stato dell'hardware.*

* **[Project Wiki](https://github.com/4137314/ppse-lab-firmware/wiki)**
  *Guida rapida all'installazione, comandi del Makefile e workflow di sviluppo per nuovi contributori.*

* **[API Reference (Doxygen)](https://4137314.github.io/ppse-lab-firmware/api/)**
  *Documentazione tecnica generata direttamente dal codice sorgente per classi, driver e task RTOS.*

---

## Struttura del Repository

```txt
.
├── config
│   └── ui.mk
├── doc
│   ├── api
│   ├── doc.mk
│   ├── README.md
│   ├── report
│   └── web
├── firm
│   ├── firm.mk
│   ├── include
│   ├── lib
│   ├── platformio.ini
│   └── src
├── Makefile
├── README.md
└── tools
    ├── api-python
    └── tool.mk
```

---

## Contributi e Sviluppo

Siamo aperti a contributi tecnici, segnalazioni di bug e suggerimenti per nuove feature. Per iniziare:

* **Setup Locale:** Consulta la sezione [Sviluppo nella Wiki](https://github.com/4137314/ppse-lab-firmware/wiki) per configurare l'ambiente `PlatformIO` e i tool di build.
* **Bug Report:** Hai trovato un errore? Apri una [Issue](https://github.com/4137314/ppse-lab-firmware/issues) descrivendo il problema e i passaggi per riprodurlo.
* 🛠️ **Pull Requests:** Prima di inviare codice, leggi le nostre [Linee Guida per i Contributori](./.github/CONTRIBUTING.md).

> **Nota:** Assicurati di eseguire `make check` localmente per verificare la conformità del codice (Linter & Formatter) prima di ogni commit.
