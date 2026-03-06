# --- CONFIGURAZIONE PERCORSI ---
DOC_DIR      := doc
FINAL_OUT    := $(DOC_DIR)/.doc-build

DATE_ISO     := $(shell date +%Y-%m-%d)
REP_USA_NAME := $(DATE_ISO)_ppse_academic-report.pdf
TEC_USA_NAME := $(DATE_ISO)_ppse_technical-reference.pdf

SITE_DIR     := $(DOC_DIR)/website
# Uniformiamo i nomi delle cartelle di build
SITE_B_DIR   := $(SITE_DIR)/.web-build
REP_DIR      := $(DOC_DIR)/report
REP_B_DIR    := .rep-build

.PHONY: doc-all doc-report doc-website doc-stage doc-clean

doc-all: doc-report doc-website doc-stage

# --- 1. REPORT ACCADEMICO (Double Pass per Indice) ---
doc-report:
	@echo "--- [LATEX] Preparing build environment in $(REP_DIR)/$(REP_B_DIR) ---"
	$(Q)mkdir -p $(REP_DIR)/$(REP_B_DIR)
	# Creazione sottocartelle per gestire \include{}
	$(Q)cd $(REP_DIR) && find . -maxdepth 1 -type d ! -name "$(REP_B_DIR)" ! -name "." -exec mkdir -p $(REP_B_DIR)/{} \;
	
	@echo "--- [LATEX] Compiling Academic Report (Pass 1/2) ---"
	$(Q)cd $(REP_DIR) && pdflatex -interaction=nonstopmode -output-directory=$(REP_B_DIR) main.tex > /dev/null
	
	@echo "--- [LATEX] Compiling Academic Report (Pass 2/2 - Generating TOC) ---"
	$(Q)cd $(REP_DIR) && pdflatex -interaction=nonstopmode -output-directory=$(REP_B_DIR) main.tex > /dev/null
	
	$(Q)cp $(REP_DIR)/$(REP_B_DIR)/main.pdf $(REP_DIR)/main.pdf

# --- 2. DOXYGEN API + MANUALE TECNICO PDF ---
doc-website:
	@echo "--- [DOXYGEN] API Documentation Extraction ---"
	$(Q)rm -rf $(SITE_B_DIR) && mkdir -p $(SITE_B_DIR)
	# Nota: OUTPUT_DIRECTORY ora punta correttamente a .web-build (SITE_B_DIR)
	$(Q)cd $(SITE_DIR) && (cat Doxyfile; \
		echo "OUTPUT_DIRECTORY = .web-build"; \
		echo "INPUT = ../../firm/src ../../README.md") | doxygen - > /dev/null
	
	@echo "--- [LATEX] Compiling Technical Reference (Double Pass) ---"
	$(Q)cd $(SITE_B_DIR)/latex && pdflatex -interaction=nonstopmode refman.tex > /dev/null
	$(Q)cd $(SITE_B_DIR)/latex && makeindex refman.idx > /dev/null 2>&1
	$(Q)cd $(SITE_B_DIR)/latex && pdflatex -interaction=nonstopmode refman.tex > /dev/null
	$(Q)cp $(SITE_B_DIR)/latex/refman.pdf $(SITE_DIR)/Technical_Reference.pdf

# --- 3. ASSEMBLAGGIO PORTALE STAGING ---
doc-stage:
	@echo "--- [STAGE] Assembling Portal in $(FINAL_OUT) ---"
	$(Q)rm -rf $(FINAL_OUT) && mkdir -p $(FINAL_OUT)/api
	$(Q)cp $(SITE_DIR)/landing/* $(FINAL_OUT)/
	$(Q)cp -r $(SITE_B_DIR)/html/* $(FINAL_OUT)/api/ 2>/dev/null || true
	$(Q)cp $(REP_DIR)/main.pdf $(FINAL_OUT)/$(REP_USA_NAME) 2>/dev/null || true
	$(Q)cp $(SITE_DIR)/Technical_Reference.pdf $(FINAL_OUT)/$(TEC_USA_NAME) 2>/dev/null || true
	$(Q)cd $(FINAL_OUT) && ln -sf $(REP_USA_NAME) Academic_Report.pdf
	$(Q)cd $(FINAL_OUT) && ln -sf $(TEC_USA_NAME) Technical_Reference.pdf

# --- 4. PULIZIA ---
doc-clean:
	@echo "--- [CLEAN] Removing doc build artifacts ---"
	$(Q)rm -rf $(FINAL_OUT)
	$(Q)rm -rf $(SITE_B_DIR)
	$(Q)rm -rf $(REP_DIR)/$(REP_B_DIR)
	$(Q)rm -f $(REP_DIR)/main.pdf
	$(Q)rm -f $(SITE_DIR)/Technical_Reference.pdf
