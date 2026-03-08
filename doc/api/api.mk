# doc/api/api.mk

API_DIR   := doc/api
API_SRC   := $(API_DIR)/src
API_B_DIR := $(API_DIR)/.api-build

.PHONY: api-build tech-ref-build api-clean

api-build:
	@echo "--- [DOXYGEN] Extracting API Reference ---"
	$(Q)mkdir -p $(API_B_DIR)
	$(Q)PROJECT_NUMBER=$(VERSION) doxygen $(API_SRC)/Doxyfile > /dev/null
	$(Q)$(MAKE) tech-ref-build

tech-ref-build:
	@echo "--- [LATEX] Building Technical Reference PDF ---"
	@test -f $(API_B_DIR)/latex/Makefile || (echo "❌ LaTeX source not found" && exit 1)
	@echo "Compiling..."
	-@cd $(API_B_DIR)/latex && pdflatex -interaction=nonstopmode refman.tex > /dev/null 2>&1
	-@cd $(API_B_DIR)/latex && pdflatex -interaction=nonstopmode refman.tex > /dev/null 2>&1
	@test -f $(API_B_DIR)/latex/refman.pdf || (echo "❌ PDF not generated" && exit 1)
	@cp $(API_B_DIR)/latex/refman.pdf $(API_DIR)/Technical_Reference.pdf
	@echo "✅ Technical Reference PDF Generated."

api-clean:
	@echo "--- [CLEAN] API Build Artifacts ---"
	$(Q)rm -rf $(API_B_DIR) $(API_DIR)/Technical_Reference.pdf
