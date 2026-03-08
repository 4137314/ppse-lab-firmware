WEB_DIR   := doc/web
WEB_SRC   := $(WEB_DIR)/src
WEB_B_DIR := $(WEB_DIR)/.web-build

.PHONY: web-build web-clean

web-build:
	@echo "--- [WEB] Building Landing Page (Version: $(VERSION)) ---"
	$(Q)mkdir -p $(WEB_B_DIR)
	$(Q)sed -e 's/@VERSION@/$(VERSION)/g' \
		 -e 's/@DATE@/$(DATE_ISO)/g' \
		 $(WEB_SRC)/index.html > $(WEB_B_DIR)/index.html
	$(Q)cp $(WEB_SRC)/style.css $(WEB_B_DIR)/style.css

web-clean:
	$(Q)rm -rf $(WEB_B_DIR)
