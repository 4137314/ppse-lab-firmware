# --- RILEVAMENTO PIO (SOLUZIONE UNIVERSALE ARCH/GENTOO/CI) ---
# 1. Tenta di trovare 'pio' nel PATH (funziona su GitHub Actions e dopo pipx ensurepath)
PIO_CMD := $(shell command -v pio 2>/dev/null)

ifeq ($(PIO_CMD),)
    # 2. Fallback specifico per Arch/pipx se il PATH non è ancora aggiornato
    PIO_VENV_PYTHON = $(HOME)/.local/share/pipx/venvs/platformio/bin/python
    PIO := $(shell [ -f $(PIO_VENV_PYTHON) ] && echo $(PIO_VENV_PYTHON) -m platformio || echo pio)
else
    PIO := $(PIO_CMD)
endif

# --- CONFIGURAZIONE BUILD ---
# Valori di default (sovrascrivibili da riga di comando: es. make BEEP=0)
DEBUG ?= 0
BEEP ?= 1

# Flag passati al compilatore
FLAGS = -Os -Dtimegm=mktime -DDEBUG=$(DEBUG) -DBUZZER_INIT_BEEP=$(BEEP)
export PLATFORMIO_BUILD_FLAGS = $(FLAGS)

.PHONY: all build debug run-silent fs clean run-debug

# --- TARGETS ---

# Target per la CI e test veloci: Solo compilazione con log di verifica flag
build:
	@echo "--- VERIFICA ALLINEAMENTO FLAG ---"
	@echo "DEBUG impostato a: $(DEBUG)"
	@echo "BEEP impostato a: $(BEEP)"
	@echo "Inviando a PIO: $(PLATFORMIO_BUILD_FLAGS)"
	@echo "----------------------------------"
	$(PIO) run

# Target di default: Compila e Carica (Debug OFF, Beep ON)
all: 
	$(PIO) run --target upload

# Sviluppo: Compila, Carica, attiva i Log e apre il Monitor
debug: 
	$(MAKE) run-debug DEBUG=1 BEEP=1

# Modalità Silenziosa: Log attivi, ma Buzzer spento
run-silent:
	$(MAKE) run-debug DEBUG=1 BEEP=0

# Target interno per gestire il monitor seriale
run-debug:
	$(PIO) run --target upload
	$(PIO) device monitor

# Utility per il Filesystem (LittleFS/FatFS)
fs:
	$(PIO) run --target uploadfs

# --- ANALISI STATICA (Issue #29) ---
# Esegue cppcheck sulla cartella sorgente
lint:
	@echo "--- ESECUZIONE ANALISI STATICA (cppcheck) ---"
	cppcheck --enable=all --suppress=missingIncludeSystem --error-exitcode=1 src/
	@echo "---------------------------------------------"

# Pulizia completa dei file temporanei e della cache
clean:
	$(PIO) run --target clean
	rm -rf .pio
