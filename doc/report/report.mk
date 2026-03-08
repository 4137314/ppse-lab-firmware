REP_DIR   := doc/report
REP_SRC   := $(REP_DIR)/src
REP_B_DIR := $(REP_DIR)/.rep-build

.PHONY: report-build report-clean

report-build:
	@echo "--- [LATEX] Compiling Academic Report ---"
	$(Q)mkdir -p $(REP_B_DIR)
	$(Q)cp -r $(REP_SRC)/* $(REP_B_DIR)/
	$(Q)cd $(REP_B_DIR) && pdflatex -interaction=nonstopmode main.tex
	$(Q)cd $(REP_B_DIR) && bibtex main || true
	$(Q)cd $(REP_B_DIR) && pdflatex -interaction=nonstopmode main.tex
	$(Q)cd $(REP_B_DIR) && pdflatex -interaction=nonstopmode main.tex
	$(Q)cp $(REP_B_DIR)/main.pdf $(REP_DIR)/main.pdf

report-clean:
	$(Q)rm -rf $(REP_B_DIR) $(REP_DIR)/main.pdf
