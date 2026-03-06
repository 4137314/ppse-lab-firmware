# ==============================================================================
# PPSE PROJECT ORCHESTRATOR - VERSION 2.1
# ==============================================================================

PROJ_NAME    := ppse-firmware
VERSION      := $(shell git describe --tags --always || echo "v0.0.0")
DATE_ISO     := $(shell date +%Y-%m-%d)
RELEASE_DIR  := .release
Q            := @

# --- UI & COLORS ---
ifneq (,$(findstring xterm,$(TERM)))
    BOLD         := \033[1m
    GREEN        := \033[32m
    YELLOW       := \033[33m
    BLUE         := \033[34m
    CYAN         := \033[36m
    RED          := \033[31m
    RESET        := \033[0m
endif

OK_ICON      := $(GREEN)$(BOLD)[OK]$(RESET)
INFO_ICON    := $(BLUE)$(BOLD)[INFO]$(RESET)
WAIT_ICON    := $(YELLOW)$(BOLD)[WAIT]$(RESET)
ERROR_ICON   := $(RED)$(BOLD)[FAIL]$(RESET)

# Inclusione Moduli (con - per non fallire se mancano temporaneamente)
-include firm/firm.mk
-include doc/doc.mk

.PHONY: all build docs release clean help info docs-fix install

all: build

## info: Visualizza i metadati del progetto
info:
	@printf -- "$(BOLD)$(CYAN)PPSE PROJECT ORCHESTRATOR$(RESET)\n"
	@printf -- "$(BOLD)Target:$(RESET)   $(PROJ_NAME)\n"
	@printf -- "$(BOLD)Version:$(RESET)  $(VERSION)\n"
	@printf -- "$(BOLD)Date:$(RESET)     $(DATE_ISO)\n"
	@printf -- "------------------------------------------------\n"

## build: Compila il firmware per Raspberry Pi Pico (.bin, .uf2)
build: info
	@printf -- "$(WAIT_ICON) Compiling Firmware...\n"
	@$(MAKE) fw-build --no-print-directory
	@printf -- "$(OK_ICON) Firmware built successfully.\n"

## docs: Genera Doxygen (API) e LaTeX (Report) con fix per le macro
docs: info docs-fix
	@printf -- "$(OK_ICON) All documentation generated and fixed.\n"

docs-fix:
	@printf -- "$(WAIT_ICON) Injecting Macros & Running Doxygen...\n"
	$(Q)cd doc/website && (cat Doxyfile; \
		echo "ENABLE_PREPROCESSING = YES"; \
		echo "MACRO_EXPANSION = YES"; \
		echo "SEARCH_INCLUDES = YES"; \
		echo "INCLUDE_PATH = ../../firm/include ../../firm/src"; \
		echo "PREDEFINED = DEBUG=0 GPSBAUD=9600 GPS_SYNC_REQ=1 GPS_SYNC_ACK=1"; \
		) | doxygen - > /dev/null
	@$(MAKE) doc-all --no-print-directory

## release: Esegue la pipeline completa e assembla il pacchetto .release/
release: info fw-format fw-lint fw-build docs
	@printf -- "$(WAIT_ICON) Assembling Distribution Bundle...\n"
	$(Q)rm -rf $(RELEASE_DIR) && mkdir -p $(RELEASE_DIR)/downloads
	$(Q)cp -r doc/.doc-build/* $(RELEASE_DIR)/
	$(Q)cp firm/.firm-build/build/pico/firmware.bin $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).bin 2>/dev/null || true
	$(Q)cp firm/.firm-build/build/pico/firmware.uf2 $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).uf2 2>/dev/null || true
	$(Q)cp $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).bin $(RELEASE_DIR)/downloads/$(PROJ_NAME)_latest.bin 2>/dev/null || true
	$(Q)cp $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).uf2 $(RELEASE_DIR)/downloads/$(PROJ_NAME)_latest.uf2 2>/dev/null || true
	$(Q)cd $(RELEASE_DIR)/downloads && (ls *.bin *.uf2 >/dev/null 2>&1 && zip -q $(PROJ_NAME)_$(VERSION)_bundle.zip *.bin *.uf2 || printf -- "$(ERROR_ICON) No binaries!\n")
	@printf -- "\n$(GREEN)$(BOLD)✅ RELEASE COMPLETE!$(RESET)\n"
	@printf -- "------------------------------------------------\n"
	@printf -- "$(BOLD)Output:$(RESET) $(RELEASE_DIR)/index.html\n"

## clean: Rimuove tutte le cartelle di build e i file temporanei
clean:
	@printf -- "$(WAIT_ICON) Deep cleaning workspace...\n"
	$(Q)rm -rf $(RELEASE_DIR) .doc-build
	@$(MAKE) fw-clean doc-clean --no-print-directory
	@printf -- "$(OK_ICON) Workspace is clean.\n"

## help: Mostra questa lista di comandi
help:
	@printf -- "$(BOLD)$(CYAN)Available Commands:$(RESET)\n"
	@grep -hE "^## " $(MAKEFILE_LIST) | sed -e 's/## //' | column -t -s ':' | sed -e 's/^/  /'
