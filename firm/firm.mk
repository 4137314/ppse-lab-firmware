# --- RILEVAMENTO PIO ---
PIO_CMD := $(shell command -v pio 2>/dev/null)
PIO := $(if $(PIO_CMD),$(PIO_CMD),$(HOME)/.local/share/pipx/venvs/platformio/bin/python -m platformio)

# --- CONFIGURAZIONE ---
DEBUG ?= 0
BEEP  ?= 1
FLAGS = -Os -DDEBUG=$(DEBUG) -DBUZZER_INIT_BEEP=$(BEEP) -std=gnu++17

# Comando base PlatformIO (eseguito sempre dentro la cartella firmware)
PIO_RUN = cd firm && PLATFORMIO_BUILD_FLAGS="$(FLAGS)" $(PIO) run

.PHONY: fw-build fw-upload fw-lint fw-clean

fw-build:
	@echo "--- [PIO] Building Firmware ---"
	$(Q)$(PIO_RUN)

fw-upload:
	@echo "--- [PIO] Flashing Board ---"
	$(Q)$(PIO_RUN) --target upload

fw-lint:
	@echo "--- [CHECK] Static Analysis ---"
	$(Q)cppcheck --enable=warning,performance,portability \
		--inline-suppr --error-exitcode=1 \
		firm/main.cpp firm/src/ firm/include/

fw-clean:
	@echo "--- [PIO] Cleaning .firm-build ---"
	$(Q)$(PIO_RUN) --target clean
	$(Q)rm -rf firm/.firm-build/
