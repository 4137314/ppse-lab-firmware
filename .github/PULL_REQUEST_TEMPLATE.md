## 📝 Descrizione
Risolve # (issue)

## 🛠️ Tipo di modifica
- [ ] 🐛 Bug fix (modifica che risolve un problema nel firmware)
- [ ] ✨ Nuova feature (modifica che aggiunge funzionalità)
- [ ] 📚 Documentazione (modifica a report LaTeX o commenti Doxygen)
- [ ] ⚙️ Refactoring (modifica del codice che non cambia le funzionalità)

## 🧪 Checklist di Verifica
### Firmware & Qualità
- [ ] Il codice compila correttamente in locale (`make build`).
- [ ] Il linter non restituisce errori (`make lint`).
- [ ] Le modifiche sono state testate sull'hardware (RP2040).

### Documentazione & Report
- [ ] Il Report Accademico PDF compila senza errori (`make doc-report`).
- [ ] La documentazione API (Doxygen) è stata aggiornata con nuovi commenti.
- [ ] Le formule LaTeX nei commenti (`temp.cpp`, ecc.) sono formattate correttamente.

### Portale & Staging
- [ ] Il portale di staging viene generato correttamente (`make doc-stage`).
- [ ] I link ai nuovi download (firmware zip) funzionano.

## 📸 Screenshot / Log (opzionale)
---
*Assicurati che i workflow di CI siano passati prima di richiedere una revisione.*
