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
	$(Q)if [ -f "$(API_B_DIR)/latex/Makefile" ]; then \
		$(MAKE) -i -C $(API_B_DIR)/latex > /dev/null 2>&1 || true; \
		if [ -f "$(API_B_DIR)/latex/refman.pdf" ]; then \
			cp $(API_B_DIR)/latex/refman.pdf $(API_DIR)/Technical_Reference.pdf && \
			echo "✅ PDF Generated with Index: $(API_DIR)/Technical_Reference.pdf"; \
		else \
			echo "❌ Error: refman.pdf not found."; exit 1; \
		fi \
	else \
		echo "❌ Error: LaTeX source not found."; exit 1; \
	fi

api-clean:
	@echo "--- [CLEAN] API Build Artifacts ---"
	$(Q)rm -rf $(API_B_DIR) $(API_DIR)/Technical_Reference.pdf
