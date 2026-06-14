# --- PIO CONFIG ---
PIO_CMD  := $(shell command -v pio 2>/dev/null)
PIO      := $(if $(PIO_CMD),$(PIO_CMD),$(HOME)/.local/share/pipx/venvs/platformio/bin/python -m platformio)
FW_B_DIR := .firm-build

# Variabili di Build
DEBUG ?= 0
BEEP  ?= 1

# Export dei flag per PlatformIO
# export PLATFORMIO_BUILD_FLAGS = -DDEBUG=$(DEBUG) -DBUZZER_INIT_BEEP=$(BEEP)
# In firm/firm.mk
# Aggiungiamo -DFW_VERSION alla stringa esistente. 
# Nota: le virgolette interne devono essere scappate per il C++
export PLATFORMIO_BUILD_FLAGS = -DDEBUG=$(DEBUG) -DBUZZER_INIT_BEEP=$(BEEP) -DFW_VERSION=\"$(VERSION)\"

# Helper per i comandi
PIO_RUN  = cd firm && $(PIO) run

.PHONY: fw-build fw-upload fw-lint fw-clean fw-format fw-monitor fw-debug

## fw-build: Compila il firmware per Pico
fw-build:
	@echo "--- [PIO] Building Firmware (Pico) ---"
	$(Q)$(PIO_RUN) -e pico

## fw-debug: Build con flag DEBUG=1
fw-debug:
	@printf -- "--- [PIO] Debug Mode: ON ---\n"
	$(Q)$(MAKE) fw-build DEBUG=1 --no-print-directory

## fw-upload: Compila e carica su RP2040
fw-upload:
	@echo "--- [PIO] Flashing Board ---"
	$(Q)$(PIO_RUN) -e pico --target upload

## fw-monitor: Apre il monitor seriale
fw-monitor:
	$(Q)cd firm && $(PIO) device monitor

## fw-lint: Analisi statica
fw-lint:
	@echo "--- [CHECK] Static Analysis ---"
	$(Q)cppcheck --enable=warning,performance,portability --inline-suppr --error-exitcode=1 \
		firm/src/ \
		firm/include/ \
		firm/lib/minmea/ \
		firm/main.cpp

## fw-format: Formattazione automatica
fw-format:
	@echo "--- [FORMAT] Formatting source code ---"
	$(Q)find firm/src firm/include -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format -i

## fw-clean: Pulisce artefatti PIO
fw-clean:
	@echo "--- [PIO] Cleaning build artifacts ---"
	$(Q)$(PIO_RUN) -e pico --target clean
	$(Q)rm -rf firm/$(FW_B_DIR)
	$(Q)rm -rf firm/.pio
