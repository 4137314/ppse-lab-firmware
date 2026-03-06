# --- CONFIGURAZIONE PROGETTO ---
PROJ_NAME    := ppse-firmware
VERSION      := $(shell git describe --tags --always || echo "v0.0.0")
DATE_ISO     := $(shell date +%Y-%m-%d)

# Inclusione Moduli (Verificare che i file .mk esistano)
include firm/firm.mk
include doc/doc.mk

# Cartella di output finale (Staging per GitHub Pages)
RELEASE_DIR  := .release

.PHONY: all build docs release clean help

# Default: solo build firmware
all: fw-build

# --- TARGET RELEASE (Fix Percorsi & ZIP Error 12) ---
release: fw-lint fw-build doc-all
	@echo "--- [RELEASE] Assembling Distribution Bundle ($(VERSION)) ---"
	# 1. Pulizia e preparazione struttura
	$(Q)rm -rf $(RELEASE_DIR) && mkdir -p $(RELEASE_DIR)/downloads
	
	# 2. Copia del portale web generato (Documentazione & Landing)
	$(Q)cp -r doc/.doc-build/* $(RELEASE_DIR)/
	
	@echo "--- [RELEASE] Copying Binaries from .firm-build ---"
	# Utilizziamo percorsi espliciti basati sul tuo tree reale
	$(Q)cp firm/.firm-build/build/pico/firmware.bin $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).bin 2>/dev/null || true
	$(Q)cp firm/.firm-build/build/pico/firmware.uf2 $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).uf2 2>/dev/null || true
	
	# Creazione link/copie 'latest' per i bottoni fissi dell'index.html
	$(Q)[ -f $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).bin ] && \
		cp $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).bin $(RELEASE_DIR)/downloads/$(PROJ_NAME)_latest.bin || true
	$(Q)[ -f $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).uf2 ] && \
		cp $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).uf2 $(RELEASE_DIR)/downloads/$(PROJ_NAME)_latest.uf2 || true
	
	@echo "--- [RELEASE] Creating ZIP bundle ---"
	# Cambiamo directory e zippiamo solo se esistono file per evitare l'Error 12
	$(Q)cd $(RELEASE_DIR)/downloads && \
		(ls *.bin *.uf2 >/dev/null 2>&1 && zip $(PROJ_NAME)_$(VERSION)_bundle.zip *.bin *.uf2 > /dev/null \
		|| echo "⚠️ Warning: No binaries found to zip")
	
	@echo "------------------------------------------------"
	@echo "🚀 RELEASE BUNDLE READY IN: $(RELEASE_DIR)/"
	@echo "📦 Version: $(VERSION) | Date: $(DATE_ISO)"
	@echo "📂 Downloads: .bin, .uf2, .zip (latest & versioned)"
	@echo "------------------------------------------------"

# --- BRIDGE TARGETS ---
build: fw-build
docs: doc-all
upload: fw-upload
lint: fw-lint

# Pulizia globale
clean: fw-clean doc-clean
	$(Q)rm -rf $(RELEASE_DIR)
	$(Q)rm -rf .doc-build

help:
	@echo "PPSE Project Orchestrator"
	@echo "-------------------------"
	@echo "  make build    : Compila il firmware"
	@echo "  make docs     : Genera Doxygen + LaTeX + Portal"
	@echo "  make release  : Pipeline completa CI/CD -> crea $(RELEASE_DIR)/"
	@echo "  make clean    : Rimuove ogni file generato"
