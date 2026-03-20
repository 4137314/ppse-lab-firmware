# doc/doc.mk

# --- CONFIGURAZIONE PERCORSI GLOBALI ---
DOC_DIR      := doc
FINAL_OUT    := $(DOC_DIR)/.doc-build

# Nomi dei file per la distribuzione finale (basati su metadati del Makefile Root)
REP_USA_NAME := $(DATE_ISO)_ppse_academic-report.pdf
TEC_USA_NAME := $(DATE_ISO)_ppse_technical-reference.pdf

# Inclusione dei sottomoduli (i percorsi sono relativi alla ROOT del progetto)
include $(DOC_DIR)/report/report.mk
include $(DOC_DIR)/api/api.mk
include $(DOC_DIR)/web/web.mk

.PHONY: doc-all doc-stage doc-clean

# Target principale richiamato dal Makefile della Root
# Esegue le build dei singoli moduli e poi assembla il tutto
doc-all: report-build api-build tech-ref-build web-build doc-stage
	@echo "--- [DONE] Documentation bundle ready in $(FINAL_OUT) ---"

# --- ASSEMBLAGGIO PORTALE STAGING ---
# Raccoglie gli artefatti dalle varie cartelle di build e pulisce i sorgenti
doc-stage:
	@echo "--- [STAGE] Assembling Portal in $(FINAL_OUT) ---"
	$(Q)rm -rf $(FINAL_OUT) && mkdir -p $(FINAL_OUT)/api
	
	# 1. Landing Page (Sorgenti web processati)
	$(Q)if [ -d "$(WEB_B_DIR)" ]; then \
		cp -r $(WEB_B_DIR)/* $(FINAL_OUT)/; \
	fi
	
	# 2. API HTML (Doxygen/Sphinx)
	$(Q)if [ -d "$(API_B_DIR)/html" ]; then \
		cp -r $(API_B_DIR)/html/* $(FINAL_OUT)/api/; \
	fi
	
	# 3. PDF: Report Accademico
	# Sposta il PDF dal modulo report alla cartella build finale
	$(Q)if [ -f "$(REP_DIR)/main.pdf" ]; then \
		mv $(REP_DIR)/main.pdf $(FINAL_OUT)/$(REP_USA_NAME); \
		(cd $(FINAL_OUT) && ln -sf $(REP_USA_NAME) Academic_Report.pdf); \
		echo "--- [OK] Academic Report moved to staging ---"; \
	fi
	
	# 4. PDF: Manuale Tecnico (Technical Reference)
	# Spostiamo il file invece di copiarlo per evitare residui nei sorgenti (tools/api-python)
	$(Q)if [ -f "$(API_DIR)/Technical_Reference.pdf" ]; then \
		mv $(API_DIR)/Technical_Reference.pdf $(FINAL_OUT)/$(TEC_USA_NAME); \
		(cd $(FINAL_OUT) && ln -sf $(TEC_USA_NAME) Technical_Reference.pdf); \
		echo "--- [OK] Technical Reference moved and source cleaned ---"; \
	fi

# Pulizia profonda specifica per la documentazione
doc-clean: report-clean api-clean web-clean
	@echo "--- [CLEAN] Removing final documentation bundle ---"
	$(Q)rm -rf $(FINAL_OUT)
