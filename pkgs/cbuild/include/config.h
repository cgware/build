#ifndef CONFIG_H
#define CONFIG_H

#include "config_schema.h"
#include "list.h"
#include "registry.h"

typedef enum config_action_e {
	CONFIG_ACT_UNKNOWN,
	CONFIG_ACT_SET,
	CONFIG_ACT_APP,
	CONFIG_ACT_EN,
	__CONFIG_ACT_CNT,
} config_act_t;

typedef struct config_val_s {
	uint op;
	uint pkg;
	uint tgt;
	config_act_t act;

	union {
		size_t s;
		int i;
		list_node_t l;
	} args;
} config_val_t;

typedef struct config_s {
	arr_t vals;
	list_t lists;
	strvbuf_t strs;
	int prio;
} config_t;

typedef struct config_state_s {
	uint vals_cnt;
	uint lists_cnt;
	size_t strs_used;
} config_state_t;

config_t *config_init(config_t *config, uint cap, alloc_t alloc);
void config_free(config_t *config);

int config_get_state(const config_t *config, config_state_t *state);
int config_set_state(config_t *config, config_state_t state);

int config_int(config_t *config, uint op, uint pkg, uint tgt, config_act_t act, int val);
int config_str(config_t *config, uint op, uint pkg, uint tgt, config_act_t act, strv_t str);
int config_str_list(config_t *config, uint op, uint pkg, uint tgt, config_act_t act, strv_t str, uint *id);

int config_merge(config_t *config, const config_t *other, config_state_t start, const config_schema_t *schema, const registry_t *registry);

size_t config_print(const config_t *config, const config_schema_t *schema, const registry_t *registry, dst_t dst);

#define CONFIG_STATE_NULL ((config_state_t){0})

#endif
