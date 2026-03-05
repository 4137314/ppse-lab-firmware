# --- Configurazione Moduli ---
include firmware/firmware.mk
include doc/website/docs.mk
include doc/academic-report/report.mk

.PHONY: all clean ci-test help

# Target di default: Compila e carica il firmware
# Notare che $(PIO) deve essere definito o ereditato dai moduli
all: 
	$(MAKE) build
	$(MAKE) upload

# Target per la CI: Esegue tutto in sequenza dai vari moduli
ci-test: lint build report docs

# Pulizia globale (delegata ai singoli moduli)
clean: clean-firmware clean-docs clean-report

help:
	@echo "Progetto: PPSE Lab Firmware (Struttura Modulare)"
	@echo "----------------------------------------------"
	@echo "  make build         : Compila il firmware (in firmware/)"
	@echo "  make report        : Genera il report LaTeX (in doc/academic-report/)"
	@echo "  make open-report   : Compila e apre il PDF del report"
	@echo "  make docs          : Genera documentazione Doxygen (in doc/website/)"
	@echo "  make ci-test       : Esegue lint + build + report + docs"
	@echo "  make clean         : Rimuove tutti i file temporanei"
