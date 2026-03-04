# Makefile Principale

# Importazione moduli
include mk/firmware.mk
include mk/docs.mk
include mk/report.mk

.PHONY: all clean ci-test help

# Target di default: Compila e carica il firmware
all: 
	$(PIO) run --target upload

# Target per la CI: Esegue tutto in sequenza
ci-test: lint build report docs

# Pulizia globale
clean: clean-firmware clean-docs clean-report

help:
	@echo "Progetto: PPSE Lab Firmware"
	@echo "---------------------------"
	@echo "  make build         : Compila il firmware"
	@echo "  make report        : Genera il report PDF"
	@echo "  make open-report   : Compila e apre il PDF"
	@echo "  make docs          : Genera documentazione Doxygen"
	@echo "  make ci-test       : Esegue lint + build + report + docs"
	@echo "  make clean         : Rimuove tutti i file temporanei"
