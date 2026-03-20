# --- CONFIGURAZIONE PERCORSI ---
API_DIR      := tools/api-python
BUILD_DIR    := $(API_DIR)/.tool-build
VENV_DIR     := $(API_DIR)/.venv
VENV_BIN     := $(VENV_DIR)/bin
PYTHON       := $(VENV_BIN)/python3
PIP          := $(VENV_BIN)/pip
PYTEST       := $(VENV_BIN)/pytest

# Flag per pytest
PYTEST_FLAGS := -v --tb=short

.PHONY: tool-setup tool-test tool-dist tool-clean tool-lint

# 1. Setup Ambiente
tool-setup: $(API_DIR)/pyproject.toml
	@echo "🐍 [PYTHON] Preparazione ambiente virtuale..."
	@test -d $(VENV_DIR) || python3 -m venv $(VENV_DIR)
	@$(PIP) install --upgrade pip -q
	@$(PIP) install -e "$(API_DIR)[test]" -q
	@$(PIP) install pyinstaller -q
	@touch $(VENV_DIR)

# 2. Test
tool-test: tool-setup
	@echo "🧪 [TEST] Esecuzione suite di test Python..."
	@PYTHONPATH=$(API_DIR) $(PYTEST) $(PYTEST_FLAGS) $(API_DIR)/tests

## tool-dist: Genera l'eseguibile standalone per LINUX
tool-dist: tool-setup
	@echo "📦 [FREEZE] Generazione eseguibile Linux in $(BUILD_DIR)/dist..."
	@mkdir -p $(BUILD_DIR)
	$(Q)$(PYTHON) -m PyInstaller --onefile --clean \
		--name pcb-sync-tool \
		--workpath $(BUILD_DIR)/pyint-build \
		--specpath $(BUILD_DIR)/pyint-spec \
		--distpath $(BUILD_DIR)/dist \
		$(API_DIR)/pcb_tool/main.py
	@printf -- "$(OK_ICON) Linux tool ready.\n"

# 3. Pulizia
tool-clean:
	@echo "🧹 [CLEAN] Rimozione artefatti Python..."
	rm -rf $(VENV_DIR) $(BUILD_DIR)
	find $(API_DIR) -type d -name "__pycache__" -exec rm -rf {} +
	find $(API_DIR) -type d -name "*.egg-info" -exec rm -rf {} +
