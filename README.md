# 🛰️ RP2040 GPS & Environment Logger

Firmware avanzato per **RP2040** dedicato al logging geospaziale, monitoraggio ambientale e telemetria in tempo reale.

---

## 🛠️ Core Features

* **GPS Integration:** Sincronizzazione oraria u-blox e logging coordinate NMEA.
* **Hybrid Storage:** Gestione dati su Flash interna via **[FatFS](https://elm-chan.org/fsw/ff/)** e USB Mass Storage.
* **UI & UX:** Interfaccia grafica su OLED SSD1306, LED RGB adattivi e feedback sonoro.
* **Power Management:** Monitoraggio dei bus di alimentazione per operazioni standalone.

---

## 📖 Risorse e Documentazione

Il progetto adotta un approccio **"Documentation as Code"**. Tutte le risorse tecniche sono centralizzate e aggiornate automaticamente ad ogni release tramite il portale ufficiale:

* 🌐 **[Project Documentation Portal](https://4137314.github.io/ppse-lab-firmware/)**
  *Punto di accesso unico per consultare il report accademico, l'architettura del sistema e lo stato dell'hardware.*

* 📚 **[Project Wiki](https://github.com/4137314/ppse-lab-firmware/wiki)**
  *Guida rapida all'installazione, comandi del Makefile e workflow di sviluppo per nuovi contributori.*

* 🛠️ **[API Reference (Doxygen)](https://4137314.github.io/ppse-lab-firmware/api/)**
  *Documentazione tecnica generata direttamente dal codice sorgente per classi, driver e task RTOS.*

---

## 📁 Struttura del Repository

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

## 🤝 Contributi e Sviluppo

Siamo aperti a contributi tecnici, segnalazioni di bug e suggerimenti per nuove feature. Per iniziare:

* 🚀 **Setup Locale:** Consulta la sezione [Sviluppo nella Wiki](https://github.com/4137314/ppse-lab-firmware/wiki) per configurare l'ambiente `PlatformIO` e i tool di build.
* 🐛 **Bug Report:** Hai trovato un errore? Apri una [Issue](https://github.com/4137314/ppse-lab-firmware/issues) descrivendo il problema e i passaggi per riprodurlo.
* 🛠️ **Pull Requests:** Prima di inviare codice, leggi le nostre [Linee Guida per i Contributori](./.github/CONTRIBUTING.md).

> **Nota:** Assicurati di eseguire `make check` localmente per verificare la conformità del codice (Linter & Formatter) prima di ogni commit.
