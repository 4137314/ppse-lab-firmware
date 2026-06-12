# Guida al Contributo

Segui queste linee guida per mantenere il codice pulito e la documentazione coerente.

# 🛠️ Setup Ambiente

Il progetto usa **Nix** per un ambiente di sviluppo identico su Windows (WSL2), macOS e Linux.

### 1. Prerequisiti
* **Installa Nix:** `sh <(curl -L https://nixos.org/nix/install) --daemon`
* **Abilita Flakes:** Aggiungi `experimental-features = nix-command flakes` in `~/.config/nix/nix.conf`.

### 2. Sviluppo
Nella root del progetto esegui:
```bash
nix develop
````

🚀 Comandi Makefile
Comando	Descrizione
make build	Compila il firmware
make flash	Carica sulla scheda
make docs	Genera documentazione
make monitor	Apre monitor seriale
make check	Linter e analisi statica
make release	Crea bundle di distribuzione

## 🚀 Workflow di Sviluppo
Il progetto usa un `Makefile` per orchestrare tutto.

## 🌿 Branching Strategy
- Crea un branch per ogni feature: `feature/nome-funzionalità`.
- Apri una Pull Request verso `main` usando il template predefinito.
- Assicurati che i workflow di CI siano verdi prima di richiedere il merge.

## 📝 Documentazione
Ogni funzione nel firmware deve essere documentata con stile Doxygen nel file `.cpp`. 
Le formule matematiche devono usare la sintassi LaTeX: `@f$ \frac{a}{b} @f$`.
