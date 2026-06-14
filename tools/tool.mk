# ========================= #
# PPSE TOOLS CONFIGURATION  #
# ========================= #

# Specifica i target di questo modulo
.PHONY: tool-clean tool-lint tool-test tool-dist sync-meteo

## tool-clean: Pulisce gli artefatti generati dai tool
tool-clean:
	@echo "--- [CLEAN] Cleaning API tools ---"
	rm -rf tools/api-weather/__pycache__
	rm -rf tools/api-weather/*.pyc
	rm -rf tools/api-python/.tool-build
	@echo "--- [OK] Tool artifacts removed ---"

## tool-lint: Esegue l'analisi statica sui tool Python
tool-lint:
	@echo "--- [LINT] Checking Python tools ---"
	flake8 tools/api-weather/pcb_bridge.py

## tool-dist: Prepara i tool per la distribuzione (placeholder)
tool-dist:
	@echo "--- [DIST] Packaging tools ---"
	# Logica di packaging futura

# --- TARGET METEO ---
## sync-meteo: Avvia il bridge di sincronizzazione meteo tra PC e PCB
sync-meteo:
	@echo "🚀 Avvio sincronizzazione meteo..."
	@python3 tools/api-weather/pcb_bridge.py
