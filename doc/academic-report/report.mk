# mk/report.mk - Modulo per il Report Accademico (Blindato)

# 1. PERCORSI (Relativi alla root del progetto dove lanci 'make')
REP_DIR      = doc/academic-report
REP_NAME     = main
# Nome cartella build (solo il nome, senza path)
B_NAME       = .build_report
# Path completo per i comandi shell (mkdir, rm)
B_PATH       = $(REP_DIR)/$(B_NAME)

# 2. COMANDI
LATEX_CMD    = pdflatex
BIB_CMD      = bibtex
# Nota: -output-directory riceve solo B_NAME perché il 'cd' ci sposta già dentro REP_DIR
LATEX_FLAGS  = -halt-on-error -interaction=nonstopmode -file-line-error -output-directory=$(B_NAME)

.PHONY: report clean-report setup-report-dir check-syntax report-error

# Crea la cartella nascosta dentro doc/academic-report/
setup-report-dir:
	@mkdir -p $(B_PATH)

check-syntax:
	@echo "--- [FIX] Sanificazione caratteri speciali (&#) ---"
	@find $(REP_DIR)/bibliography/src/ -name "*.bib" -exec sed -i 's/\([^\\]\)&/\1\\\&/g' {} + 2>/dev/null || true
	@sed -i '75s/\([^\\]\)#/\1\\#/g' $(REP_DIR)/section/src/hardware.tex 2>/dev/null || true

report: clean-report setup-report-dir check-syntax
	@echo "--- [LATEX] Passaggio 1: Generazione file ausiliari ---"
	cd $(REP_DIR) && $(LATEX_CMD) $(LATEX_FLAGS) $(REP_NAME).tex
	
	@echo "--- [BIBTEX] Elaborazione Bibliografia ---"
	# Bibtex deve lavorare dentro la cartella build dove stanno i file .aux
	-cd $(B_PATH) && $(BIB_CMD) $(REP_NAME)
	
	@echo "--- [LATEX] Passaggi finali: Citazioni e Indice ---"
	cd $(REP_DIR) && $(LATEX_CMD) $(LATEX_FLAGS) $(REP_NAME).tex
	cd $(REP_DIR) && $(LATEX_CMD) $(LATEX_FLAGS) $(REP_NAME).tex
	
	@if [ -f $(B_PATH)/$(REP_NAME).pdf ]; then \
		mv $(B_PATH)/$(REP_NAME).pdf $(REP_DIR)/$(REP_NAME).pdf; \
		echo "--- [SUCCESS] Report generato in: $(REP_DIR)/$(REP_NAME).pdf ---"; \
	else \
		echo "--- [ERROR] PDF non trovato in $(B_PATH). Controlla i log. ---"; \
		exit 1; \
	fi

report-error:
	@echo "--- [DEBUG] Analisi errori nel log ---"
	@grep -A 3 "!" $(B_PATH)/$(REP_NAME).log || echo "Nessun errore fatale trovato."

clean-report:
	@echo "--- [LATEX] Pulizia sicura (Solo cartella di build) ---"
	# Rimuove solo la sottocartella .build_report e il pdf finale
	rm -rf $(REP_DIR)/$(B_NAME)
	rm -f $(REP_DIR)/$(REP_NAME).pdf
