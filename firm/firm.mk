# --- PIO CONFIG ---
PIO_CMD  := $(shell command -v pio 2>/dev/null)
PIO      := $(if $(PIO_CMD),$(PIO_CMD),$(HOME)/.local/share/pipx/venvs/platformio/bin/python -m platformio)
FW_B_DIR := .firm-build

# Variabili di Build (sovrascrivibili da riga di comando)
DEBUG ?= 0
BEEP  ?= 1

# Export dei flag per PlatformIO - Inclusione automatica delle variabili di sistema
export PLATFORMIO_BUILD_FLAGS = -DDEBUG=$(DEBUG) -DBUZZER_INIT_BEEP=$(BEEP)

# Helper per i comandi (si assume che il Makefile sia nella root del progetto)
PIO_RUN = cd firm && $(PIO) run

.PHONY: fw-build fw-upload fw-lint fw-clean fw-format fw-monitor fw-debug

## fw-build: Compila il firmware in modalità standard
fw-build:
	@echo "--- [PIO] Building Firmware ---"
	$(Q)$(PIO_RUN)

## fw-debug: Build con flag DEBUG=1 e log verbosi
fw-debug:
	@printf -- "--- [PIO] Debug Mode: ON ---\n"
	$(Q)$(MAKE) fw-build DEBUG=1 --no-print-directory

## fw-upload: Compila e carica su RP2040 via picotool
fw-upload:
	@echo "--- [PIO] Flashing Board ---"
	$(Q)$(PIO_RUN) --target upload

## fw-monitor: Apre il monitor seriale
fw-monitor:
	$(Q)cd firm && $(PIO) device monitor

## fw-lint: Analisi statica del codice (src, include e lib/minmea)
fw-lint:
	@echo "--- [CHECK] Static Analysis ---"
	$(Q)cppcheck --enable=warning,performance,portability --inline-suppr --error-exitcode=1 \
		firm/src/ \
		firm/include/ \
		firm/lib/minmea/ \
		firm/main.cpp

## fw-format: Formattazione automatica con clang-format (include sottocartelle)
fw-format:
	@echo "--- [FORMAT] Formatting source code ---"
	$(Q)find firm/src firm/include -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format -i

## fw-clean: Pulisce i file di compilazione di PIO e la cartella di build custom
fw-clean:
	@echo "--- [PIO] Cleaning build artifacts ---"
	$(Q)$(PIO_RUN) --target clean
	$(Q)rm -rf firm/$(FW_B_DIR)
	$(Q)rm -rf firm/.pio
