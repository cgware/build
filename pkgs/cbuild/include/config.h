#ifndef CONFIG_H
#define CONFIG_H

#include "list.h"
#include "registry.h"

typedef enum config_mode_e {
	CONFIG_MODE_UNKNOWN,
	CONFIG_MODE_SET,
	CONFIG_MODE_EN,
	CONFIG_MODE_APP,
	__CONFIG_MODE_CNT,
} config_mode_t;

typedef enum config_op_type_s {
	CONFIG_OP_TYPE_UNKNOWN,
	CONFIG_OP_TYPE_PKG,
	CONFIG_OP_TYPE_PKG_PATH,
	CONFIG_OP_TYPE_PKG_INC,
	CONFIG_OP_TYPE_PKG_URI,
	CONFIG_OP_TYPE_PKG_DEPS,
	CONFIG_OP_TYPE_TGT,
	CONFIG_OP_TYPE_TGT_TYPE,
	CONFIG_OP_TYPE_TGT_SRC,
	CONFIG_OP_TYPE_TGT_INC,
	CONFIG_OP_TYPE_TGT_INCS_PRIV,
	CONFIG_OP_TYPE_TGT_DEPS,
	CONFIG_OP_TYPE_TGT_PREP,
	CONFIG_OP_TYPE_TGT_CONF,
	CONFIG_OP_TYPE_TGT_COMP,
	CONFIG_OP_TYPE_TGT_INST,
	CONFIG_OP_TYPE_TGT_OUT,
	CONFIG_OP_TYPE_TGT_LIB,
	CONFIG_OP_TYPE_TGT_EXE,
	__CONFIG_OP_TYPE_CNT,
} config_op_type_t;

typedef struct config_op_s {
	config_op_type_t type;
	uint pkg;
	uint tgt;
	config_mode_t mode;

	union {
		size_t s;
		int i;
		list_node_t l;
	} args;
} config_op_t;

typedef struct config_s {
	arr_t ops;
	list_t lists;
	strvbuf_t strs;
	int prio;
} config_t;

typedef struct config_state_s {
	uint ops_cnt;
	uint lists_cnt;
	size_t strs_used;
} config_state_t;

config_t *config_init(config_t *config, uint cap, alloc_t alloc);
void config_free(config_t *config);

int config_get_state(const config_t *config, config_state_t *state);
int config_set_state(config_t *config, config_state_t state);

int config_str(config_t *config, config_op_type_t type, uint pkg, uint tgt, int mode, strv_t str);
int config_str_list(config_t *config, config_op_type_t type, uint pkg, uint tgt, int mode, strv_t str, uint *id);
int config_str_list_add(config_t *config, uint id, strv_t str);

int config_pkg(config_t *config, uint pkg, config_mode_t mode);

int config_tgt(config_t *config, uint pkg, uint tgt, config_mode_t mode);
int config_tgt_type(config_t *config, uint pkg, uint tgt, config_mode_t mode, int type);

int config_merge(config_t *config, const config_t *other, config_state_t start, const registry_t *registry);

size_t config_print(const config_t *config, const registry_t *registry, dst_t dst);

#define CONFIG_STATE_NULL ((config_state_t){0})

#endif
