PIO = pio
# Valori di default (sovrascrivibili da riga di comando: make BEEP=0)
DEBUG ?= 0
BEEP ?= 1

# Flag comuni
FLAGS = -Os -Dtimegm=mktime -DDEBUG=$(DEBUG) -DBUZZER_INIT_BEEP=$(BEEP)
# Esporta la variabile per tutti i comandi pio
export PLATFORMIO_BUILD_FLAGS = $(FLAGS)

.PHONY: all run-silent fs clean

# Target di default: Compila e Carica (Debug OFF, Beep ON)
all: 
	$(PIO) run --target upload

# Sviluppo: Log attivi e Monitor (es: make debug)
debug: 
	$(MAKE) run-debug DEBUG=1 BEEP=1

# Biblioteca: Log attivi, ma Beep spento
run-silent:
	$(MAKE) run-debug DEBUG=1 BEEP=0

# Target "motore" per evitare di scrivere Serial Monitor ovunque
run-debug:
	$(PIO) run --target upload
	$(PIO) device monitor

# Utility
fs:
	$(PIO) run --target uploadfs

clean:
	$(PIO) run --target clean
	rm -rf .pio
