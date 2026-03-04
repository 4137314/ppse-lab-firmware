# mk/firmware.mk

# --- RILEVAMENTO PIO ---
PIO_CMD := $(shell command -v pio 2>/dev/null)
ifeq ($(PIO_CMD),)
    PIO_VENV_PYTHON = $(HOME)/.local/share/pipx/venvs/platformio/bin/python
    PIO := $(shell [ -f $(PIO_VENV_PYTHON) ] && echo $(PIO_VENV_PYTHON) -m platformio || echo pio)
else
    PIO := $(PIO_CMD)
endif

# --- CONFIGURAZIONE BUILD ---
DEBUG ?= 0
BEEP ?= 1
FLAGS = -Os -Dtimegm=mktime -DDEBUG=$(DEBUG) -DBUZZER_INIT_BEEP=$(BEEP)
export PLATFORMIO_BUILD_FLAGS = $(FLAGS)

.PHONY: build debug run-silent fs lint clean-firmware

build:
	@echo "--- BUILD FIRMWARE (DEBUG=$(DEBUG) BEEP=$(BEEP)) ---"
	$(PIO) run

debug: 
	$(MAKE) run-debug DEBUG=1 BEEP=1

run-silent:
	$(MAKE) run-debug DEBUG=1 BEEP=0

run-debug:
	$(PIO) run --target upload
	$(PIO) device monitor

fs:
	$(PIO) run --target uploadfs

lint:
	@echo "--- ANALISI STATICA (cppcheck) ---"
	cppcheck --enable=warning,performance,portability \
		--suppress=missingInclude \
		--suppress=missingIncludeSystem \
		--inline-suppr \
		--error-exitcode=1 src/

clean-firmware:
	$(PIO) run --target clean
	rm -rf .pio
