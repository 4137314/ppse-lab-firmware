#include "core/config.h"

// Questa riga alloca la memoria per la variabile. 
// Senza questa, il linker ti darà "undefined reference to global_cfg"
SystemConfig global_cfg = {150, true, true, 1};
