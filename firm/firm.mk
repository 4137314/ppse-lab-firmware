# --- PIO CONFIG ---
PIO_CMD := $(shell command -v pio 2>/dev/null)
PIO     := $(if $(PIO_CMD),$(PIO_CMD),$(HOME)/.local/share/pipx/venvs/platformio/bin/python -m platformio)
FW_B_DIR := .firm-build

# Variabili di Build (sovrascrivibili da riga di comando)
DEBUG ?= 0
BEEP  ?= 1

# Lasciamo che il Makefile gestisca solo le variabili che cambiano davvero
export PLATFORMIO_BUILD_FLAGS = -DDEBUG=$(DEBUG) -DBUZZER_INIT_BEEP=$(BEEP)

# Helper per i comandi
PIO_RUN = cd firm && $(PIO) run

.PHONY: fw-build fw-upload fw-lint fw-clean fw-format fw-monitor fw-debug

fw-build:
	@echo "--- [PIO] Building Firmware ---"
	$(Q)$(PIO_RUN)

## fw-debug: Build rapida con flag DEBUG attivi
fw-debug:
	@printf -- "$(INFO_ICON) Debug Mode: ON\n"
	$(Q)$(MAKE) fw-build DEBUG=1 --no-print-directory

fw-upload:
	@printf -- "$(WAIT_ICON) Flashing Board...\n"
	$(Q)$(PIO_RUN) --target upload

fw-monitor:
	$(Q)cd firm && $(PIO) device monitor

fw-lint:
	@echo "--- [CHECK] Static Analysis ---"
	$(Q)cppcheck --enable=warning,performance,portability --inline-suppr --error-exitcode=1 \
		firm/src/ firm/include/ firm/main.cpp

fw-format:
	@printf -- "$(WAIT_ICON) Formatting source code...\n"
	$(Q)find firm/src firm/include -name "*.?pp" -o -name "*.?h" | xargs clang-format -i

fw-clean:
	@echo "--- [PIO] Cleaning build artifacts ---"
	$(Q)$(PIO_RUN) --target clean
	# Cancella la cartella radice corretta del firmware
	$(Q)rm -rf firm/$(FW_B_DIR)
