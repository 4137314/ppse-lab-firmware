# --- COLORS & STYLES ---
ifneq (,$(findstring xterm,$(TERM)))
    BOLD    := \033[1m
    GREEN   := \033[32m
    YELLOW  := \033[33m
    BLUE    := \033[34m
    CYAN    := \033[36m
    RED     := \033[31m
    RESET   := \033[0m
endif

# --- STATUS ICONS ---
OK_ICON    := $(GREEN)$(BOLD)[OK]$(RESET)
INFO_ICON  := $(BLUE)$(BOLD)[INFO]$(RESET)
WAIT_ICON  := $(YELLOW)$(BOLD)[WAIT]$(RESET)
ERROR_ICON := $(RED)$(BOLD)[FAIL]$(RESET)

# --- DECORATIONS ---
LINE       := ------------------------------------------------
