#ifndef GLOBAL_CONFIG
#define GLOBAL_CONFIG

#include "config_schema.h"
#include "mod.h"

enum {
	CONFIG_PKGS,
	CONFIG_PKG_PATH,
	CONFIG_PKG_DEPS,
	CONFIG_TGTS,
	CONFIG_TGT_TYPE,
	CONFIG_TGT_SRC,
	CONFIG_TGT_INC,
	CONFIG_TGT_INCS_PRIV,
	CONFIG_TGT_DEPS,
};

int mod_base_init(uint cap, config_schema_t *config_schema, alloc_t alloc);

mod_t *mod_base_get();

#endif
