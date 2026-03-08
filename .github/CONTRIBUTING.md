# Guida al Contributo

Grazie per l'interesse nel progetto! Segui queste linee guida per mantenere il codice pulito e la documentazione coerente.

## 🛠️ Setup Ambiente
1. Installa **PlatformIO Core**.
2. Installa una distribuzione **LaTeX** (es. TeX Live).
3. Assicurati di avere `doxygen` e `graphviz` nel PATH.

## 🚀 Workflow di Sviluppo
Il progetto usa un `Makefile` per orchestrare tutto:

- **Build Firmware**: `make build`
- **Linting**: `make lint`
- **Genera Report PDF**: `make doc-report`
- **Genera API Docs**: `make doc-website`
- **Tutto insieme**: `make doc-all`

## 🌿 Branching Strategy
- Crea un branch per ogni feature: `feature/nome-funzionalità`.
- Apri una Pull Request verso `main` usando il template predefinito.
- Assicurati che i workflow di CI siano verdi prima di richiedere il merge.

## 📝 Documentazione
Ogni funzione nel firmware deve essere documentata con stile Doxygen nel file `.cpp`. 
Le formule matematiche devono usare la sintassi LaTeX: `@f$ \frac{a}{b} @f$`.
