# PPSE-Lab Firmware 🚀

[![Milestone](https://img.shields.io/badge/Milestone-MVP-blue)](https://github.com/4137314/ppse-lab-firmware/milestones)
[![PlatformIO](https://img.shields.io/badge/Platform-ESP32-orange)](https://platformio.org/)

Firmware avanzato per il monitoraggio ambientale e meteo con integrazione GPS, gestione energetica e interfaccia UI dinamica. Il progetto punta a un'esperienza utente fluida con supporto offline e sincronizzazione dati intelligente.

---

## 🛠 Functional Features (MVP)

### 🌤 Meteo & Data Management
- **Smart Fetching:** Acquisizione posizione tramite GPS e sincronizzazione con API Meteo.
- **Offline First:** Archiviazione dei dati in Flash Memory per consultazione senza connettività; indicatore di timestamp "Last Update".
- **Interactive Visualization:** Consultazione storica e previsioni tramite display OLED (navigazione giornaliera/oraria).
- **Adaptive UI:** Luminosità automatica dei LED basata sul ciclo circadiano (Day/Night mode).
- **USB Interface:** Il dispositivo viene riconosciuto dal PC come memoria di massa (Mass Storage Class) per l'accesso diretto ai file di log.

### 🔌 Hardware & System Logic
- **Boot Sequence:** Feedback immediato all'accensione tramite pattern sonori (Buzzer) e visivi (LED).
- **Clock Sync:** Sistema RTC integrato. All'avvio utilizza un tempo standard, sincronizzandosi al secondo tramite segnale GPS `Timepulse` non appena disponibile.
- **Power & Resource Management:** Ottimizzazione dei cicli di scrittura su Flash per preservare l'hardware.

---

## 🖥 User Interface (UI) Structure

### 1. Ambient Display (Idle Mode)
Visualizzazione dei dati critici in tempo reale:
- Qualità dell'aria e temperatura.
- Timestamp dell'ultimo aggiornamento.
- Orologio di sistema e stato del fix GPS.

### 2. Main Menu & Navigation
- **Meteo Browser:** Navigazione orizzontale (Swipe giorni) e verticale (Dettaglio orario).
- **System Tools:** - `Erase Memory`: Formattazione sicura della partizione dati.
    - `Silent Mode`: Toggle rapido per feedback sonori e luminosi.
    - `Auto-Brightness`: Sensore di luce ambientale / Toggle manuale.
    - `Module Init`: Debug e inizializzazione singola dei driver (LED, Buzzer, GPS).

### 3. Smart Shortcuts
- **Quick Exit:** Doppio click sinistro per chiudere i menu.
- **Force Sync:** Pressione prolungata (1s) Destra + Sinistra per aggiornamento dati meteo.
- **Home Reset:** Shortcut dedicata per il ritorno immediato all'Ambient Display.

---

## 📅 Roadmap & Future Work
- [ ] Implementazione animazioni fluide per transizioni menu.
- [ ] Calcolo automatico del Fuso Orario basato sulle coordinate GPS.
- [ ] Ottimizzazione consumi in Deep Sleep.
