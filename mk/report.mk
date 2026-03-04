# mk/report.mk - Modulo per il Report Accademico (CI-Ready - Safe Version)

REPORT_DIR  = report
REPORT_NAME = main
BUILD_DIR   = .build_latex
LATEX_CMD   = pdflatex
BIB_CMD     = bibtex

LATEX_FLAGS = -halt-on-error -interaction=nonstopmode -file-line-error -output-directory=$(BUILD_DIR)

.PHONY: report clean-report setup-report-dir check-syntax report-error

setup-report-dir:
	@mkdir -p $(REPORT_DIR)/$(BUILD_DIR)

check-syntax:
	@echo "--- [FIX] Sanificazione caratteri speciali (&#) ---"
	# Protegge & nei file .bib solo se NON sono già protette da \
	@find $(REPORT_DIR)/bibliography/src/ -name "*.bib" -exec sed -i 's/\([^\\]\)&/\1\\\&/g' {} + 2>/dev/null || true
	# Protegge # a riga 75 in hardware.tex solo se non è già \#
	@sed -i '75s/\([^\\]\)#/\1\\#/g' $(REPORT_DIR)/section/src/hardware.tex 2>/dev/null || true
	# NOTA: Il fix per riga 136 è stato rimosso. Correggi a mano nel .tex per evitare cicli infiniti.

report: clean-report setup-report-dir check-syntax
	@echo "--- [LATEX] Passaggio 1: Generazione file ausiliari ---"
	cd $(REPORT_DIR) && $(LATEX_CMD) $(LATEX_FLAGS) $(REPORT_NAME).tex
	
	@echo "--- [BIBTEX] Elaborazione Bibliografia ---"
	# Bibtex deve lavorare sui file generati nella cartella build
	-cd $(REPORT_DIR)/$(BUILD_DIR) && $(BIB_CMD) $(REPORT_NAME)
	
	@echo "--- [LATEX] Passaggio 2: Risoluzione citazioni e riferimenti ---"
	cd $(REPORT_DIR) && $(LATEX_CMD) $(LATEX_FLAGS) $(REPORT_NAME).tex
	
	@echo "--- [LATEX] Passaggio 3: Finalizzazione Indice e Hyperlinks ---"
	cd $(REPORT_DIR) && $(LATEX_CMD) $(LATEX_FLAGS) $(REPORT_NAME).tex
	
	@if [ -f $(REPORT_DIR)/$(BUILD_DIR)/$(REPORT_NAME).pdf ]; then \
		mv $(REPORT_DIR)/$(BUILD_DIR)/$(REPORT_NAME).pdf $(REPORT_DIR)/$(REPORT_NAME).pdf; \
		echo "--- [SUCCESS] Report generato: $(REPORT_DIR)/$(REPORT_NAME).pdf ---"; \
	else \
		echo "--- [ERROR] Compilazione fallita. Controlla i log con 'make report-error' ---"; \
		exit 1; \
	fi

report-error:
	@echo "--- [DEBUG] Analisi errori nel log ---"
	@grep -A 3 "!" $(REPORT_DIR)/$(BUILD_DIR)/$(REPORT_NAME).log || echo "Nessun errore fatale trovato."

clean-report:
	@echo "--- [LATEX] Pulizia profonda file temporanei ---"
	rm -rf $(REPORT_DIR)/$(BUILD_DIR)
	rm -f $(REPORT_DIR)/$(REPORT_NAME).pdf
