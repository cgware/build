#include "config_schema.h"

#include "arr.h"
#include "log.h"
#include "proj.h"

config_schema_t *config_schema_init(config_schema_t *schema, uint cap, alloc_t alloc)
{
	if (schema == NULL) {
		return NULL;
	}

	if (arr_init(&schema->ops, cap, sizeof(config_schema_op_t), alloc) == NULL || strvbuf_init(&schema->strs, cap, 16, alloc) == NULL) {
		return NULL;
	}

	return schema;
}

void config_schema_free(config_schema_t *schema)
{
	if (schema == NULL) {
		return;
	}

	arr_free(&schema->ops);
	strvbuf_free(&schema->strs);
}

int config_schema_add_ops(config_schema_t *schema, config_schema_op_desc_t *descs, uint size)
{
	if (schema == NULL) {
		return 1;
	}

	for (uint i = 0; i < size / sizeof(config_schema_op_desc_t); i++) {
		size_t strs_cnt;
		if (strvbuf_add(&schema->strs, descs[i].name, &strs_cnt)) {
			log_error("cbuild", "config", NULL, "failed to add op: %.*s", descs[i].name.len, descs[i].name.data);
			return 1;
		}

		config_schema_op_t *op = arr_add(&schema->ops, NULL);
		if (op == NULL) {
			strvbuf_reset(&schema->strs, strs_cnt);
			return 1;
		}

		op->type  = descs[i].type;
		op->scope = descs[i].scope;
		op->name  = strs_cnt;
		op->priv  = descs[i].priv;
	}

	return 0;
}

config_schema_op_t *config_schema_get_op(const config_schema_t *schema, uint id)
{
	if (schema == NULL) {
		return NULL;
	}

	config_schema_op_t *op = arr_get(&schema->ops, id);
	if (op == NULL) {
		log_error("cbuild", "config_schema", NULL, "invalid op: %d", id);
		return NULL;
	}

	return op;
}
