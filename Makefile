# ========================= #
# PPSE PROJECT ORCHESTRATOR #
# ========================= #

PROJ_NAME      := ppse-firmware
VERSION        := $(shell git describe --tags --always || echo "v0.0.0")
DATE_ISO       := $(shell date +%Y-%m-%d)
RELEASE_DIR    := .release
Q              := @

# Carica UI e Moduli (firmware, documentazione, tools)
include config/ui.mk
-include firm/firm.mk
-include doc/doc.mk
include tools/tool.mk

.PHONY: all build docs release clean help info check flash monitor test

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
	$(Q)$(MAKE) fw-format fw-lint tool-lint --no-print-directory
	@printf -- "$(OK_ICON) Codebase is clean and compliant.\n"

## test: Esegue i test dei tool
test: info
	@printf -- "$(WAIT_ICON) Running Tool Test Suite...\n"
	$(Q)$(MAKE) tool-test --no-print-directory || (printf "$(ERROR_ICON) Tool tests failed!\n"; exit 1)
	@printf -- "$(OK_ICON) All active tests passed.\n"

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

## release: Pipeline completa (Test -> Build -> Docs -> Tool -> Staging)
release: clean test build docs tool-dist
	@printf -- "$(WAIT_ICON) Assembling Distribution Bundle...\n"
	$(Q)mkdir -p $(RELEASE_DIR)/downloads
	
	# 1. Documentazione (Portale Web e PDF)
	$(Q)cp -r $(FINAL_OUT)/* $(RELEASE_DIR)/ 2>/dev/null || true
	
	# 2. Firmware (Ripristino completo: UF2, BIN, ELF + ZIP)
	$(Q)if [ -f firm/.firm-build/build/pico/firmware.uf2 ]; then \
		cp firm/.firm-build/build/pico/firmware.uf2 $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).uf2; \
		cp firm/.firm-build/build/pico/firmware.bin $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).bin; \
		cp firm/.firm-build/build/pico/firmware.elf $(RELEASE_DIR)/downloads/$(PROJ_NAME)_$(VERSION).elf; \
		(cd $(RELEASE_DIR)/downloads && \
			ln -sf $(PROJ_NAME)_$(VERSION).uf2 $(PROJ_NAME)_latest.uf2 && \
			ln -sf $(PROJ_NAME)_$(VERSION).bin $(PROJ_NAME)_latest.bin && \
			ln -sf $(PROJ_NAME)_$(VERSION).elf $(PROJ_NAME)_latest.elf && \
			zip -q $(PROJ_NAME)_$(VERSION)_bundle.zip $(PROJ_NAME)_$(VERSION).uf2 $(PROJ_NAME)_$(VERSION).bin $(PROJ_NAME)_$(VERSION).elf && \
			ln -sf $(PROJ_NAME)_$(VERSION)_bundle.zip $(PROJ_NAME)_latest_bundle.zip); \
		printf -- "$(OK_ICON) Firmware staged and ZIP bundle created.\n"; \
	fi

	# 3. Python Tool (Solo Linux, integrato nella release)
	$(Q)if [ -f tools/api-python/.tool-build/dist/pcb-sync-tool ]; then \
		cp tools/api-python/.tool-build/dist/pcb-sync-tool $(RELEASE_DIR)/downloads/$(PROJ_NAME)_tool_$(VERSION); \
		chmod +x $(RELEASE_DIR)/downloads/$(PROJ_NAME)_tool_$(VERSION); \
		(cd $(RELEASE_DIR)/downloads && \
			ln -sf $(PROJ_NAME)_tool_$(VERSION) $(PROJ_NAME)_tool_latest); \
		printf -- "$(OK_ICON) Linux tool staged and linked.\n"; \
	else \
		printf -- "$(ERROR_ICON) Tool binary not found!\n"; exit 1; \
	fi

	@printf -- "\n$(GREEN)$(BOLD)✅ RELEASE COMPLETE!$(RESET) -> $(RELEASE_DIR)/\n"

## clean: Rimuove tutte le cartelle di build, doc e tool Python
clean:
	@printf -- "$(WAIT_ICON) Deep cleaning workspace...\n"
	$(Q)rm -rf $(RELEASE_DIR) .doc-build
	$(Q)$(MAKE) fw-clean doc-clean tool-clean --no-print-directory
	$(Q)rm -rf firm/.firm-build
	$(Q)rm -rf tools/api-python/.tool-build
	@printf -- "$(OK_ICON) Workspace is clean.\n"

## help: Mostra questo menu
help:
	@printf -- "$(BOLD)$(CYAN)Available Commands:$(RESET)\n"
	@grep -hE "^## " $(MAKEFILE_LIST) | sed -e 's/## //' | column -t -s ':' | sed -e 's/^/  /'
