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

.PHONY: all build docs release clean help info check flash monitor test flash-debug

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

## test: Verifica dello stato dei test (Attualmente disabilitato)
test: info
	@printf -- "$(WAIT_ICON) No test suite configured. Skipping.\n"

## build: Compila il firmware per Raspberry Pi Pico
build: info
	$(Q)$(MAKE) fw-build --no-print-directory

## flash: Compila e carica il firmware sulla scheda
flash: info
	$(Q)$(MAKE) fw-upload --no-print-directory

## flash-debug: Compila e carica forzando i flag di debug
flash-debug: info
	@printf -- "$(BOLD)$(RED)--- [PIO] Debug Mode & Silent Buzzer (Atomic Upload) ---$(RESET)\n"
	$(Q)cd firm && PLATFORMIO_BUILD_FLAGS="-DDEBUG=1 -DDEFAULT_BUZZER_OFF" $(PIO) run -e pico --target upload

## monitor: Apre il monitor seriale per il debug
monitor:
	$(Q)$(MAKE) fw-monitor --no-print-directory

## docs: Genera il portale della documentazione (.doc-build)
docs: info
	$(Q)$(MAKE) doc-all --no-print-directory

## release: Pipeline completa (Build -> Docs -> Tool -> Staging)
release: clean build docs tool-dist
	@printf -- "$(WAIT_ICON) Assembling Distribution Bundle...\n"
	$(Q)mkdir -p $(RELEASE_DIR)/downloads
	
	# 1. Documentazione (Portale Web e PDF)
	$(Q)rm -f $(RELEASE_DIR)/Academic_Report.pdf $(RELEASE_DIR)/Technical_Reference.pdf
	
	# Gestione Dinamica Academic Report (File Reale + Link Simbolico)
	$(Q)REAL_PDF=$$(ls doc/report/src/*academic-report.pdf 2>/dev/null | head -n 1); \
	if [ -f "$$REAL_PDF" ]; then \
		cp "$$REAL_PDF" $(RELEASE_DIR)/; \
		ln -sf $$(basename "$$REAL_PDF") $(RELEASE_DIR)/Academic_Report.pdf; \
		printf "$(OK_ICON) Academic Report staged (linked to $$(basename "$$REAL_PDF")).\n"; \
	elif [ -f doc/report/src/main.pdf ]; then \
		cp doc/report/src/main.pdf $(RELEASE_DIR)/Academic_Report.pdf; \
		printf "$(OK_ICON) Academic Report (main.pdf) staged.\n"; \
	fi
	
	# Gestione Technical Reference
	$(Q)if [ -f doc/.doc-build/technical_ref.pdf ]; then \
		cp doc/.doc-build/technical_ref.pdf $(RELEASE_DIR)/Technical_Reference.pdf; \
		printf "$(OK_ICON) Technical Reference staged.\n"; \
	fi
	
	# Copia il resto del portale (HTML/CSS)
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
		printf -- "$(OK_ICON) Tool binary not found, skipping tool staging.\n"; \
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
