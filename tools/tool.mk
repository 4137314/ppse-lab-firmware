# --- TARGET METEO ---
.PHONY: sync-meteo
sync-meteo:
	@echo "🚀 Avvio sincronizzazione meteo..."
	@python3 tools/api-weather/pcb_bridge.py
