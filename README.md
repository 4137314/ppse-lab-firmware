# 🛰️ RP2040 GPS & Environment Logger

Firmware avanzato per **RP2040** dedicato al logging geospaziale, monitoraggio ambientale e telemetria in tempo reale.

---

## 🛠️ Core Features

* **GPS Integration:** Sincronizzazione oraria u-blox e logging coordinate NMEA.
* **Hybrid Storage:** Gestione dati su Flash interna via **LittleFS** e USB Mass Storage.
* **UI & UX:** Interfaccia grafica su OLED SSD1306, LED RGB adattivi e feedback sonoro.
* **Power Management:** Monitoraggio dei bus di alimentazione per operazioni standalone.

---

## 📖 Risorse e Documentazione

Tutte le informazioni dettagliate sono centralizzate nelle seguenti risorse esterne, aggiornate automaticamente tramite CI/CD:

* 📚 **[Project Wiki](../../wiki)** – **Guida Rapida**, comandi di build (`pio`, `make`), manuale utente e scorciatoie.
* 📄 **[Academic Report (PDF)](https://4137314.github.io/ppse-lab-firmware/Academic_Report_PPSE.pdf)** – Analisi tecnica, schemi elettrici e architettura del sistema.
* 🌐 **[API Reference](https://4137314.github.io/ppse-lab-firmware/api/)** – Documentazione tecnica Doxygen per classi e driver.

---

## 📁 Struttura del Repository

* `src/`: Logica applicativa e task core.
* `lib/`: Driver hardware (GPS, sensori, display).
* `report/`: Sorgenti LaTeX della documentazione accademica.
* `site/`: Dashboard statica per il portale web del progetto.

---

### 🤝 Contributi e Sviluppo
Se desideri contribuire o compilare il firmware localmente, consulta la sezione **Sviluppo** nella [Wiki del progetto](../../wiki). Per segnalazioni di bug, apri una **Issue** o una **Pull Request**.

<script type="text/javascript" src="doxygen-awesome-darkmode-toggle.js"></script>
<script type="text/javascript">
    DoxygenAwesomeDarkModeToggle.init()
</script>
