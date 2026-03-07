#ifndef CONFIG_SCHEMA_H
#define CONFIG_SCHEMA_H

#include "arr.h"
#include "strvbuf.h"

typedef enum config_type_e {
	CONFIG_TYPE_UNKNOWN,
	CONFIG_TYPE_INT,
	CONFIG_TYPE_STR,
	CONFIG_TYPE_STR_LIST,
} config_type_t;

typedef enum config_scope_e {
	CONFIG_SCOPE_UNKNOWN,
	CONFIG_SCOPE_GLOBAL,
	CONFIG_SCOPE_PKG,
	CONFIG_SCOPE_TGT,
	__CONFIG_SCOPE_CNT,
} config_scope_t;

typedef struct config_schema_op_desc_s {
	config_type_t type;
	config_scope_t scope;
	strv_t name;
	void *priv;
} config_schema_op_desc_t;

typedef struct onfig_schema_op_s {
	config_type_t type;
	config_scope_t scope;
	size_t name;
	void *priv;
} config_schema_op_t;

typedef struct config_schema_s {
	arr_t ops;
	strvbuf_t strs;
} config_schema_t;

config_schema_t *config_schema_init(config_schema_t *schema, uint cap, alloc_t alloc);
void config_schema_free(config_schema_t *schema);

int config_schema_add_ops(config_schema_t *schema, config_schema_op_desc_t *descs, uint size);
config_schema_op_t *config_schema_get_op(const config_schema_t *schema, uint op);

#endif
