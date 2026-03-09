# ========================= #
# PPSE PROJECT ORCHESTRATOR #
# ========================= #

PROJ_NAME    := ppse-firmware
VERSION      := $(shell git describe --tags --always || echo "v0.0.0")
DATE_ISO     := $(shell date +%Y-%m-%d)
RELEASE_DIR  := .release
Q            := @

# Carica UI e Moduli
include build/ui.mk
-include firm/firm.mk
-include doc/doc.mk

.PHONY: all build docs release clean help info check flash monitor

all: build

## info: Mostra i metadati del progetto
info:
	@printf -- "$(BOLD)$(CYAN)PPSE PROJECT ORCHESTRATOR$(RESET)\n"
	@printf -- "$(BOLD)Target:$(RESET)    $(PROJ_NAME)\n"
	@printf -- "$(BOLD)Version:$(RESET)  $(VERSION)\n"
	@printf -- "$(BOLD)Date:$(RESET)     $(DATE_ISO)\n"
	@printf -- "$(LINE)\n"

## check: Esegue formattazione e analisi statica (Linter)
check: info
	$(Q)$(MAKE) fw-format fw-lint --no-print-directory
	@printf -- "$(OK_ICON) Codebase is clean and compliant.\n"

## build: Compila il firmware per Raspberry Pi Pico
build: info
	$(Q)$(MAKE) fw-build --no-print-directory

## flash: Compila e carica il firmware sulla scheda
flash: info
	$(Q)$(MAKE) fw-upload --no-print-directory

## monitor: Apre il monitor seriale per il debug
monitor:
	$(Q)$(MAKE) fw-monitor --no-print-directory

## docs: Genera il portale della documentazione (.doc-build)
docs: info
	$(Q)$(MAKE) doc-all --no-print-directory

## release: Pipeline completa con ZIP Bundle
release: clean build docs #tolto check ma da vedere se riaggiungere: il problema era .clang-format che su github-action usa una versione piu vecchia
	@printf -- "$(WAIT_ICON) Assembling Distribution Bundle...\n"
	$(Q)mkdir -p $(RELEASE_DIR)/downloads
	# 1. Copia il portale web e i PDF
	$(Q)cp -r $(FINAL_OUT)/* $(RELEASE_DIR)/ 2>/dev/null || true
	# 2. Preleva i binari (Nota: usiamo .firm-build come da tuoi log)
	$(Q)if [ -f firm/.firm-build/build/pico/firmware.uf2 ]; then \
		cp firm/.firm-build/build/pico/firmware.uf2 $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).uf2; \
		cp firm/.firm-build/build/pico/firmware.bin $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).bin; \
		cp firm/.firm-build/build/pico/firmware.elf $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).elf; \
		cd $(RELEASE_DIR)/downloads && \
			ln -sf $(PROJ_NAME)_$(VERSION).uf2 $(PROJ_NAME)_latest.uf2 && \
			ln -sf $(PROJ_NAME)_$(VERSION).bin $(PROJ_NAME)_latest.bin && \
			ln -sf $(PROJ_NAME)_$(VERSION).elf $(PROJ_NAME)_latest.elf && \
			zip -q $(PROJ_NAME)_$(VERSION)_bundle.zip $(PROJ_NAME)_$(VERSION).uf2 $(PROJ_NAME)_$(VERSION).bin $(PROJ_NAME)_$(VERSION).elf && \
			ln -sf $(PROJ_NAME)_$(VERSION)_bundle.zip $(PROJ_NAME)_latest_bundle.zip; \
		printf -- "$(OK_ICON) Binaries staged and ZIP bundle created.\n"; \
	else \
		printf -- "$(ERROR_ICON) Build artifacts not found in .firm-build!\n"; exit 1; \
	fi
	@printf -- "\n$(GREEN)$(BOLD)✅ RELEASE COMPLETE!$(RESET) -> $(RELEASE_DIR)/\n"

## clean: Rimuove tutte le cartelle di build
clean:
	@printf -- "$(WAIT_ICON) Deep cleaning workspace...\n"
	# Rimuove le cartelle di release e build documentazione
	$(Q)rm -rf $(RELEASE_DIR) .doc-build
	# Chiama i clean specifici dei moduli
	$(Q)$(MAKE) fw-clean doc-clean --no-print-directory
	# --- AGGIUNTA: Forza la rimozione della cartella nascosta del firmware ---
	$(Q)rm -rf firm/.firm-build
	@printf -- "$(OK_ICON) Workspace is clean.\n"

## help: Mostra questo menu
help:
	@printf -- "$(BOLD)$(CYAN)Available Commands:$(RESET)\n"
	@grep -hE "^## " $(MAKEFILE_LIST) | sed -e 's/## //' | column -t -s ':' | sed -e 's/^/  /'
