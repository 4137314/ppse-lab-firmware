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

## release: Pipeline completa: clean -> check -> build -> docs -> bundle
release: clean check build docs
	@printf -- "$(WAIT_ICON) Assembling Distribution Bundle...\n"
	$(Q)mkdir -p $(RELEASE_DIR)/downloads
	$(Q)cp -r doc/.doc-build/* $(RELEASE_DIR)/ 2>/dev/null || true
	$(Q)find firm -name "firmware.*" -exec cp {} $(RELEASE_DIR)/downloads/ \; 2>/dev/null || true
	@printf -- "\n$(GREEN)$(BOLD)✅ RELEASE COMPLETE!$(RESET) -> $(RELEASE_DIR)/\n"

## clean: Rimuove tutte le cartelle di build
clean:
	@printf -- "$(WAIT_ICON) Deep cleaning workspace...\n"
	$(Q)rm -rf $(RELEASE_DIR) .doc-build
	$(Q)$(MAKE) fw-clean doc-clean --no-print-directory
	@printf -- "$(OK_ICON) Workspace is clean.\n"

## help: Mostra questo menu
help:
	@printf -- "$(BOLD)$(CYAN)Available Commands:$(RESET)\n"
	@grep -hE "^## " $(MAKEFILE_LIST) | sed -e 's/## //' | column -t -s ':' | sed -e 's/^/  /'
