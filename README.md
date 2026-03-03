# PPSE-Lab Firmware 🚀

Firmware per **RP2040** dedicato al logging GPS, monitoraggio ambientale e visualizzazione dati meteo. Sviluppato con **PlatformIO**.

---

## 🛠 Core Features

- **GPS Integration:** Sincronizzazione oraria tramite segnale `Timepulse` e logging coordinate.
- **Storage:** Gestione dati su Flash interna (LittleFS) con supporto **USB Mass Storage** (il dispositivo viene montato come disco dal PC).
- **UI & Haptics:** Interfaccia grafica su display OLED, gestione LED adattiva (Day/Night) e feedback sonoro via Buzzer.
- **Data Bridge:** Comunicazione seriale ottimizzata per l'integrazione con script Python esterni.

---

## 📖 Documentazione

Tutte le specifiche tecniche, i menu e gli shortcut sono disponibili nella **Wiki** del progetto:

👉 **[Vai alla Wiki del Progetto](../../wiki)**

*Per i dettagli sulla struttura delle cartelle e le API dei sensori, consulta la cartella [`/docs`](./docs).*

---

## 🚀 Sviluppo

```bash
# Build del firmware
pio run

# Caricamento su hardware
pio run --target upload
```

