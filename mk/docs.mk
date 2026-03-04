# mk/docs.mk - Modulo per Doxygen

DOCS_DIR = docs

.PHONY: docs clean-docs

docs:
	@echo "--- [DOXYGEN] Generazione documentazione API ---"
	doxygen Doxyfile
	@echo "--- [DOXYGEN] Documentazione pronta in $(DOCS_DIR)/html/index.html ---"

clean-docs:
	@echo "--- [DOXYGEN] Rimozione cartelle generate ---"
	rm -rf $(DOCS_DIR)/html $(DOCS_DIR)/latex
