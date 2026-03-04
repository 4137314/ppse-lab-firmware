# mk/report.mk - Modulo per il Report Accademico (PRO Version)

REPORT_DIR  = report
REPORT_NAME = main
BUILD_DIR   = .build_latex
LATEX_CMD   = pdflatex

# Flags migliorati: 
# -output-directory sposta i file temporanei nella cartella specificata
LATEX_FLAGS = -halt-on-error -interaction=nonstopmode -file-line-error -output-directory=$(BUILD_DIR)

.PHONY: report clean-report open-report setup-report-dir

# Crea la directory temporanea se non esiste
setup-report-dir:
	@mkdir -p $(REPORT_DIR)/$(BUILD_DIR)

# Target principale per la compilazione
report: setup-report-dir
	@echo "--- [LATEX] Controllo sintassi e caratteri speciali ---"
	# Fix automatico riga 75 (il famoso cancelletto #)
	@sed -i '75s/\([^\]\)#/\1\\#/g' $(REPORT_DIR)/section/src/hardware.tex 2>/dev/null || true
	
	@echo "--- [LATEX] Passaggio 1: Generazione file ausiliari ---"
	cd $(REPORT_DIR) && $(LATEX_CMD) $(LATEX_FLAGS) $(REPORT_NAME).tex
	
	@echo "--- [LATEX] Passaggio 2: Risoluzione riferimenti e Indice ---"
	cd $(REPORT_DIR) && $(LATEX_CMD) $(LATEX_FLAGS) $(REPORT_NAME).tex
	
	# Sposta il PDF finale dalla cartella build alla root del report
	@mv $(REPORT_DIR)/$(BUILD_DIR)/$(REPORT_NAME).pdf $(REPORT_DIR)/$(REPORT_NAME).pdf
	@echo "--- [LATEX] Report generato: $(REPORT_DIR)/$(REPORT_NAME).pdf ---"

# Debug veloce: stampa solo le righe con errori dal file di log
report-error:
	@grep -A 5 "!" $(REPORT_DIR)/$(BUILD_DIR)/$(REPORT_NAME).log || echo "Nessun errore critico trovato nel log."

# Utility per aprire il PDF (Linux)
open-report: report
	@xdg-open $(REPORT_DIR)/$(REPORT_NAME).pdf &

clean-report:
	@echo "--- [LATEX] Rimozione file temporanei e PDF ---"
	rm -rf $(REPORT_DIR)/$(BUILD_DIR)
	rm -f $(REPORT_DIR)/$(REPORT_NAME).pdf
