# --- CONFIGURAZIONE PERCORSI GLOBALI ---
DOC_DIR      := doc
FINAL_OUT    := $(DOC_DIR)/.doc-build

# Nomi dei file per la distribuzione finale
REP_USA_NAME := $(DATE_ISO)_ppse_academic-report.pdf
TEC_USA_NAME := $(DATE_ISO)_ppse_technical-reference.pdf

# Inclusione dei sottomoduli (i percorsi sono relativi alla ROOT del progetto)
include $(DOC_DIR)/report/report.mk
include $(DOC_DIR)/api/api.mk
include $(DOC_DIR)/web/web.mk

.PHONY: doc-all doc-stage doc-clean

# Target principale richiamato dal Makefile della Root
doc-all: report-build api-build tech-ref-build web-build doc-stage
	@echo "--- [DONE] Documentation bundle ready in $(FINAL_OUT) ---"

# --- ASSEMBLAGGIO PORTALE STAGING ---
# Questa fase raccoglie gli artefatti dalle varie cartelle .build e li unisce
doc-stage:
	@echo "--- [STAGE] Assembling Portal in $(FINAL_OUT) ---"
	$(Q)rm -rf $(FINAL_OUT) && mkdir -p $(FINAL_OUT)/api
	
	# 1. Landing Page (Sorgenti web processati)
	$(Q)if [ -d "$(WEB_B_DIR)" ]; then cp -r $(WEB_B_DIR)/* $(FINAL_OUT)/; fi
	
	# 2. API HTML (Doxygen)
	$(Q)if [ -d "$(API_B_DIR)/html" ]; then cp -r $(API_B_DIR)/html/* $(FINAL_OUT)/api/; fi
	
	# 3. PDF: Report Accademico
	$(Q)if [ -f "$(REP_DIR)/main.pdf" ]; then \
		cp $(REP_DIR)/main.pdf $(FINAL_OUT)/$(REP_USA_NAME); \
		cd $(FINAL_OUT) && ln -sf $(REP_USA_NAME) Academic_Report.pdf; \
	fi
	
	# 4. PDF: Manuale Tecnico
	$(Q)if [ -f "$(API_DIR)/Technical_Reference.pdf" ]; then \
		cp $(API_DIR)/Technical_Reference.pdf $(FINAL_OUT)/$(TEC_USA_NAME); \
		cd $(FINAL_OUT) && ln -sf $(TEC_USA_NAME) Technical_Reference.pdf; \
	fi

doc-clean: report-clean api-clean web-clean
	@echo "--- [CLEAN] Removing final distribution bundle ---"
	$(Q)rm -rf $(FINAL_OUT)
