API_DIR   := doc/api
API_SRC   := $(API_DIR)/src
API_B_DIR := $(API_DIR)/.api-build

.PHONY: api-build tech-ref-build api-clean

api-build:
	@echo "--- [DOXYGEN] Extracting API Reference ---"
	$(Q)rm -rf $(API_B_DIR) && mkdir -p $(API_B_DIR)
	$(Q)cd $(API_SRC) && (cat Doxyfile; \
		echo "PROJECT_NUMBER = $(VERSION)"; \
		echo "OUTPUT_DIRECTORY = ../.api-build"; \
		) | doxygen - > /dev/null

tech-ref-build:
	@echo "--- [LATEX] Compiling Technical Reference PDF ---"
	$(Q)if [ -d "$(API_B_DIR)/latex" ]; then \
		cd $(API_B_DIR)/latex && \
		pdflatex -interaction=nonstopmode refman.tex > /dev/null && \
		makeindex refman.idx > /dev/null 2>&1 && \
		pdflatex -interaction=nonstopmode refman.tex > /dev/null && \
		cp refman.pdf ../../Technical_Reference.pdf; \
	fi

api-clean:
	$(Q)rm -rf $(API_B_DIR) $(API_DIR)/Technical_Reference.pdf
