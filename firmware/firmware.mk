# --- firmware/firmware.mk ---

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

# Uniamo le flag: teniamo il tuo -std=gnu++17 che è vitale per le nuove librerie
FLAGS = -Os -Dtimegm=mktime -DDEBUG=$(DEBUG) -DBUZZER_INIT_BEEP=$(BEEP) -std=gnu++17

# Approccio basato su directory context (risolve l'errore del flex scanner)
# Entriamo in firmware/ prima di ogni comando PIO
PIO_RUN_CMD := cd firmware && PLATFORMIO_BUILD_FLAGS="$(FLAGS)" $(PIO) run

.PHONY: build debug run-silent fs lint clean-firmware clean-all

build:
	@echo "--- BUILD FIRMWARE (DEBUG=$(DEBUG) BEEP=$(BEEP)) ---"
	$(PIO_RUN_CMD)

debug: 
	$(MAKE) run-debug DEBUG=1 BEEP=1

run-silent:
	$(MAKE) run-debug DEBUG=1 BEEP=0

run-debug:
	$(PIO_RUN_CMD) --target upload
	cd firmware && $(PIO) device monitor

fs:
	$(PIO_RUN_CMD) --target uploadfs

lint:
	@echo "--- ANALISI STATICA (cppcheck) ---"
	# Analizziamo i path relativi dalla root del progetto
	cppcheck --enable=warning,performance,portability \
		--suppress=missingInclude \
		--suppress=missingIncludeSystem \
		--inline-suppr \
		--error-exitcode=1 \
		firmware/main.cpp firmware/src/ firmware/include/

clean-firmware:
	@echo "--- PULIZIA PROFONDA FIRMWARE ---"
	# Pulizia standard di PlatformIO (rimuove .o, .elf, .bin)
	$(PIO_RUN_CMD) --target clean
	# Rimozione fisica della cartella di build e della cache delle librerie
	rm -rf firmware/.build_pio/
	rm -rf firmware/.pio/

clean-all:
	$(PIO_RUN_CMD) --target clean
	rm -rf firmware/build_pio

# Analisi dell'occupazione di memoria
size:
	cd firmware && pio run -t sizedata

# Ispezione statica dei simboli (richiede che il build sia già stato fatto)
inspect:
	cd firmware && pio device inspect

format:
	find firmware/src firmware/include -type f \( -name "*.h" -o -name "*.cpp" \) | xargs /usr/bin/clang-format -i -style=file
