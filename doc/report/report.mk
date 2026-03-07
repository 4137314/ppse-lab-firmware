REP_DIR   := doc/report
REP_SRC   := $(REP_DIR)/src
REP_B_DIR := $(REP_DIR)/.rep-build

.PHONY: report-build report-clean

report-build:
	@echo "--- [LATEX] Compiling Academic Report ---"
	$(Q)mkdir -p $(REP_B_DIR)
	# Fix find: confiniamo la creazione delle cartelle solo al primo livello di src
	$(Q)cd $(REP_SRC) && find . -maxdepth 1 -type d ! -name "." -exec mkdir -p ../.rep-build/{} \;
	# Compilazione
	$(Q)cd $(REP_SRC) && pdflatex -interaction=nonstopmode -output-directory=../.rep-build main.tex > /dev/null
	$(Q)cd $(REP_SRC) && pdflatex -interaction=nonstopmode -output-directory=../.rep-build main.tex > /dev/null
	$(Q)cp $(REP_B_DIR)/main.pdf $(REP_DIR)/main.pdf

report-clean:
	$(Q)rm -rf $(REP_B_DIR) $(REP_DIR)/main.pdf
