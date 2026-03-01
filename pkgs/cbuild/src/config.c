#include "config.h"

#include "log.h"

config_t *config_init(config_t *config, uint cap, alloc_t alloc)
{
	if (config == NULL) {
		return NULL;
	}

	if (arr_init(&config->ops, cap, sizeof(config_op_t), alloc) == NULL ||
	    list_init(&config->lists, cap, sizeof(size_t), alloc) == NULL || strvbuf_init(&config->strs, cap, 16, alloc) == NULL) {
		return NULL;
	}

	config->prio = 0;

	return config;
}

void config_free(config_t *config)
{
	if (config == NULL) {
		return;
	}

	arr_free(&config->ops);
	list_free(&config->lists);
	strvbuf_free(&config->strs);
}

int config_get_state(const config_t *config, config_state_t *state)
{
	if (config == NULL || state == NULL) {
		return 1;
	}

	state->ops_cnt	 = config->ops.cnt;
	state->lists_cnt = config->lists.cnt;
	state->strs_used = config->strs.used;

	return 0;
}

int config_set_state(config_t *config, config_state_t state)
{
	if (config == NULL) {
		return 1;
	}

	arr_reset(&config->ops, state.ops_cnt);
	list_reset(&config->lists, state.lists_cnt);
	strvbuf_reset(&config->strs, state.strs_used);

	return 0;
}

static const char *s_op_names[] = {
	[CONFIG_OP_TYPE_UNKNOWN]       = "unknown",
	[CONFIG_OP_TYPE_PKG]	       = "pkgs",
	[CONFIG_OP_TYPE_PKG_PATH]      = "path",
	[CONFIG_OP_TYPE_PKG_INC]       = "inc",
	[CONFIG_OP_TYPE_PKG_URI]       = "uri",
	[CONFIG_OP_TYPE_PKG_DEPS]      = "deps",
	[CONFIG_OP_TYPE_TGT]	       = "tgts",
	[CONFIG_OP_TYPE_TGT_TYPE]      = "type",
	[CONFIG_OP_TYPE_TGT_SRC]       = "src",
	[CONFIG_OP_TYPE_TGT_INC]       = "inc",
	[CONFIG_OP_TYPE_TGT_INCS_PRIV] = "incs_priv",
	[CONFIG_OP_TYPE_TGT_DEPS]      = "deps",
	[CONFIG_OP_TYPE_TGT_PREP]      = "prep",
	[CONFIG_OP_TYPE_TGT_CONF]      = "conf",
	[CONFIG_OP_TYPE_TGT_COMP]      = "comp",
	[CONFIG_OP_TYPE_TGT_INST]      = "inst",
	[CONFIG_OP_TYPE_TGT_OUT]       = "out",
	[CONFIG_OP_TYPE_TGT_LIB]       = "lib",
	[CONFIG_OP_TYPE_TGT_EXE]       = "exe",
};

static const char *op_type_str(config_op_type_t type)
{
	if (type < CONFIG_OP_TYPE_UNKNOWN || type >= __CONFIG_OP_TYPE_CNT) {
		log_error("cbuild", "config", NULL, "invalid type: %d", type);
		type = CONFIG_OP_TYPE_UNKNOWN;
	}

	return s_op_names[type];
}

static config_op_t *config_add_op(config_t *config, config_op_type_t type, uint pkg, uint tgt, int mode, uint *id)
{
	config_op_t *op = arr_add(&config->ops, id);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "failed to add: %s", op_type_str(type));
		return NULL;
	}

	op->type = type;
	op->pkg	 = pkg;
	op->tgt	 = tgt;
	op->mode = mode;

	return op;
}

static config_op_t *config_add_op_int(config_t *config, config_op_type_t type, uint pkg, uint tgt, int mode, int val)
{
	config_op_t *op = config_add_op(config, type, pkg, tgt, mode, NULL);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "failed to add int op with val: %d", val);
		return NULL;
	}

	op->args.i = val;

	return op;
}

int config_str(config_t *config, config_op_type_t type, uint pkg, uint tgt, int mode, strv_t str)
{
	if (config == NULL) {
		return 1;
	}

	size_t strs_cnt;
	if (strvbuf_add(&config->strs, str, &strs_cnt)) {
		log_error("cbuild", "config", NULL, "failed to add str: %.*s", str.len, str.data);
		return 1;
	}

	config_op_t *op = config_add_op(config, type, pkg, tgt, mode, NULL);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "failed to add string op: %.*s", str.len, str.data);
		strvbuf_reset(&config->strs, strs_cnt);
		return 1;
	}

	op->args.s = strs_cnt;

	return 0;
}

int config_str_list(config_t *config, config_op_type_t type, uint pkg, uint tgt, int mode, strv_t str, uint *id)
{
	if (config == NULL) {
		return 1;
	}

	size_t strs_cnt;
	if (strvbuf_add(&config->strs, str, &strs_cnt)) {
		log_error("cbuild", "config", NULL, "failed to add str: %.*s", str.len, str.data);
		return 1;
	}

	list_node_t list;
	size_t *val = list_node(&config->lists, &list);
	if (val == NULL) {
		log_error("cbuild", "config", NULL, "failed to add list");
		strvbuf_reset(&config->strs, strs_cnt);
		return 1;
	}

	*val = strs_cnt;

	config_op_t *op = config_add_op(config, type, pkg, tgt, mode, id);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "failed to add list op: %.*s", str.len, str.data);
		strvbuf_reset(&config->strs, strs_cnt);
		list_reset(&config->lists, list);
		return 1;
	}

	op->args.l = list;

	return 0;
}

int config_str_list_add(config_t *config, uint id, strv_t str)
{
	if (config == NULL) {
		return 1;
	}

	const config_op_t *op = arr_get(&config->ops, id);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "invalid op id: %d", id);
		return 1;
	}

	size_t strs_cnt;
	if (strvbuf_add(&config->strs, str, &strs_cnt)) {
		log_error("cbuild", "config", NULL, "failed to add str: %.*s", str.len, str.data);
		return 1;
	}

	list_node_t list;
	size_t *val = list_node(&config->lists, &list);
	if (val == NULL) {
		log_error("cbuild", "config", NULL, "failed to add list");
		strvbuf_reset(&config->strs, strs_cnt);
		return 1;
	}

	*val = strs_cnt;

	if (list_app(&config->lists, op->args.l, list)) {
		log_error("cbuild", "config", NULL, "failed to append list");
		strvbuf_reset(&config->strs, strs_cnt);
		list_reset(&config->lists, list);
		return 1;
	}

	return 0;
}

int config_pkg(config_t *config, uint pkg, config_mode_t mode)
{
	if (config == NULL) {
		return 1;
	}

	config_op_t *op = config_add_op(config, CONFIG_OP_TYPE_PKG, pkg, -1, mode, NULL);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "failed to create package: %d", pkg);
		return 1;
	}

	return 0;
}

int config_tgt(config_t *config, uint pkg, uint tgt, config_mode_t mode)
{
	if (config == NULL) {
		return 1;
	}

	config_op_t *op = config_add_op(config, CONFIG_OP_TYPE_TGT, pkg, tgt, mode, NULL);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "failed to create target: %d", tgt);
		return 1;
	}

	return 0;
}

int config_tgt_type(config_t *config, uint pkg, uint tgt, config_mode_t mode, int type)
{
	if (config == NULL) {
		return 1;
	}

	config_op_t *op = config_add_op_int(config, CONFIG_OP_TYPE_TGT_TYPE, pkg, tgt, mode, type);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "failed to set target type: %d", type);
		return 1;
	}

	return 0;
}

typedef enum op_type_s {
	OP_TYPE_UNKNOWN,
	OP_TYPE_SEL,
	OP_TYPE_INT,
	OP_TYPE_STR,
	OP_TYPE_STR_LIST,
} op_type_t;

static op_type_t op_types[] = {
	[CONFIG_OP_TYPE_UNKNOWN]       = OP_TYPE_UNKNOWN,
	[CONFIG_OP_TYPE_PKG]	       = OP_TYPE_SEL,
	[CONFIG_OP_TYPE_TGT]	       = OP_TYPE_SEL,
	[CONFIG_OP_TYPE_PKG_PATH]      = OP_TYPE_STR,
	[CONFIG_OP_TYPE_PKG_INC]       = OP_TYPE_STR,
	[CONFIG_OP_TYPE_PKG_URI]       = OP_TYPE_STR,
	[CONFIG_OP_TYPE_PKG_DEPS]      = OP_TYPE_STR_LIST,
	[CONFIG_OP_TYPE_TGT_SRC]       = OP_TYPE_STR,
	[CONFIG_OP_TYPE_TGT_INC]       = OP_TYPE_STR,
	[CONFIG_OP_TYPE_TGT_TYPE]      = OP_TYPE_INT,
	[CONFIG_OP_TYPE_TGT_INCS_PRIV] = OP_TYPE_STR_LIST,
	[CONFIG_OP_TYPE_TGT_DEPS]      = OP_TYPE_STR_LIST,
	[CONFIG_OP_TYPE_TGT_PREP]      = OP_TYPE_STR,
	[CONFIG_OP_TYPE_TGT_CONF]      = OP_TYPE_STR,
	[CONFIG_OP_TYPE_TGT_COMP]      = OP_TYPE_STR,
	[CONFIG_OP_TYPE_TGT_INST]      = OP_TYPE_STR,
	[CONFIG_OP_TYPE_TGT_OUT]       = OP_TYPE_STR,
	[CONFIG_OP_TYPE_TGT_LIB]       = OP_TYPE_STR,
	[CONFIG_OP_TYPE_TGT_EXE]       = OP_TYPE_STR,
};

static strv_t contains_str_list(const config_t *a, list_node_t a_l, const config_t *b, list_node_t b_l)
{
	const size_t *a_str;
	list_foreach(&a->lists, a_l, a_str)
	{
		strv_t a_val = strvbuf_get(&a->strs, *a_str);
		const size_t *b_str;
		int found = 0;
		list_foreach(&b->lists, b_l, b_str)
		{
			strv_t b_val = strvbuf_get(&b->strs, *b_str);
			if (strv_eq(a_val, b_val)) {
				found = 1;
				break;
			}
		}

		if (!found) {
			return a_val;
		}
	}

	return STRV_NULL;
}

static int config_str_list_app_new(config_t *config, uint id, const config_t *other, list_node_t other_list)
{
	const config_op_t *op = arr_get(&config->ops, id);

	int ret = 0;

	const size_t *other_str;
	list_foreach(&other->lists, other_list, other_str)
	{
		strv_t other_val = strvbuf_get(&other->strs, *other_str);
		const size_t *b_str;
		int found      = 0;
		list_node_t it = op->args.l;
		list_foreach(&config->lists, it, b_str)
		{
			strv_t val = strvbuf_get(&config->strs, *b_str);
			if (strv_eq(other_val, val)) {
				found = 1;
				break;
			}
		}

		if (!found) {
			ret |= config_str_list_add(config, id, other_val);
		}
	}

	return ret;
}

static const int s_sel_mode_prio[] = {
	[CONFIG_MODE_SET]     = 0,
	[CONFIG_MODE_APP]     = 1,
	[CONFIG_MODE_EN]      = 2,
	[CONFIG_MODE_UNKNOWN] = 3,
};

static int get_sel_mode_prio(config_mode_t mode)
{
	if (mode < CONFIG_MODE_UNKNOWN || mode >= __CONFIG_MODE_CNT) {
		log_error("cbuild", "config", NULL, "invalid mode: %d", mode);
		mode = CONFIG_MODE_UNKNOWN;
	}
	return s_sel_mode_prio[mode];
}

int config_merge(config_t *config, const config_t *other, config_state_t start, const registry_t *registry)
{
	if (config == NULL || other == NULL || registry == NULL) {
		return 1;
	}

	int ret = 0;

	config_op_t *other_op;
	uint i = start.ops_cnt;
	arr_foreach(&other->ops, i, other_op)
	{
		int found = 0;
		config_op_t *op;
		uint j = 0;
		arr_foreach(&config->ops, j, op)
		{
			if (op->type == other_op->type && op->pkg == other_op->pkg && op->tgt == other_op->tgt) {
				found = 1;
				break;
			}
		}

		if (found) {
			int mode = op->mode;
			if (get_sel_mode_prio(other_op->mode) < get_sel_mode_prio(mode)) {
				mode = other_op->mode;
			}
			switch (op_types[op->type]) {
			case OP_TYPE_SEL:
				op->mode = mode;
				break;
			case OP_TYPE_STR: {
				strv_t pkg	 = registry_get_pkg(registry, op->pkg);
				strv_t tgt	 = op->tgt == (uint)-1 ? STRV("") : registry_get_tgt(registry, op->tgt);
				strv_t val	 = strvbuf_get(&config->strs, op->args.s);
				strv_t other_val = strvbuf_get(&other->strs, other_op->args.s);
				if (!strv_eq(val, other_val)) {
					if (other->prio == config->prio) {
						log_error("cbuild",
							  "config",
							  NULL,
							  "value mismatch: %.*s:%.*s:%s: %.*s != %.*s",
							  pkg.len,
							  pkg.data,
							  tgt.len,
							  tgt.data,
							  op_type_str(op->type),
							  val.len,
							  val.data,
							  other_val.len,
							  other_val.data);
						ret |= 1;
					} else if (other->prio > config->prio) {
						strvbuf_add(&config->strs, other_val, &op->args.s);
					}

				} else {
					op->mode = mode;
				}
				break;
			}
			case OP_TYPE_INT: {
				strv_t pkg    = registry_get_pkg(registry, op->pkg);
				strv_t tgt    = op->tgt == (uint)-1 ? STRV("") : registry_get_tgt(registry, op->tgt);
				int val	      = op->args.i;
				int other_val = other_op->args.i;
				if (val != other_val) {
					if (other->prio == config->prio) {
						log_error("cbuild",
							  "config",
							  NULL,
							  "value mismatch: %.*s:%.*s:%s: %d != %d",
							  pkg.len,
							  pkg.data,
							  tgt.len,
							  tgt.data,
							  op_type_str(op->type),
							  val,
							  other_val);
						ret |= 1;
					} else if (other->prio > config->prio) {
						op->args.i = other_val;
					}
				} else {
					op->mode = mode;
				}
				break;
			}
			case OP_TYPE_STR_LIST: {
				if (mode == CONFIG_MODE_APP) {
					config_str_list_app_new(config, j, other, other_op->args.l);
					op->mode = mode;
				} else {
					strv_t a = contains_str_list(config, op->args.l, other, other_op->args.l);
					strv_t b = contains_str_list(other, other_op->args.l, config, op->args.l);
					if (a.data == NULL && b.data == NULL) {
						op->mode = mode;
					} else {
						if (other->prio == config->prio) {
							strv_t pkg = registry_get_pkg(registry, op->pkg);
							strv_t tgt = op->tgt == (uint)-1 ? STRV("") : registry_get_tgt(registry, op->tgt);

							strv_t val = a.data != NULL ? a : b;
							log_error("cbuild",
								  "config",
								  NULL,
								  "value mismatch: %.*s:%.*s:%s: missing: %.*s",
								  pkg.len,
								  pkg.data,
								  tgt.len,
								  tgt.data,
								  op_type_str(op->type),
								  val.len,
								  val.data);

							ret |= 1;
						} else if (other->prio > config->prio) {
							const size_t *off;
							list_node_t it = other_op->args.l;
							list_foreach(&other->lists, it, off)
							{
								strv_t val = strvbuf_get(&other->strs, *off);

								size_t strs_cnt;
								if (strvbuf_add(&config->strs, val, &strs_cnt)) {
									log_error("cbuild",
										  "config",
										  NULL,
										  "failed to add str: %.*s",
										  val.len,
										  val.data);
									return 1;
								}

								list_node_t list;
								size_t *off = list_node(&config->lists, &list);
								if (off == NULL) {
									log_error("cbuild", "config", NULL, "failed to add list");
									strvbuf_reset(&config->strs, strs_cnt);
									return 1;
								}

								*off = strs_cnt;

								if (it == other_op->args.l) {
									op->args.l = list;
								} else {
									list_app(&config->lists, op->args.l, list);
								}
							}
						}
					}
				}
				break;
			}
			default:
				log_error("cbuild", "config", NULL, "unknown op: %d", op->type);
				break;
			}
		} else {
			switch (op_types[other_op->type]) {
			case OP_TYPE_SEL: {
				config_add_op(config, other_op->type, other_op->pkg, other_op->tgt, other_op->mode, NULL);
				break;
			}
			case OP_TYPE_STR: {
				config_str(config,
					   other_op->type,
					   other_op->pkg,
					   other_op->tgt,
					   other_op->mode,
					   strvbuf_get(&other->strs, other_op->args.s));
				break;
			}
			case OP_TYPE_INT: {
				config_add_op_int(config, other_op->type, other_op->pkg, other_op->tgt, other_op->mode, other_op->args.i);
				break;
			}
			case OP_TYPE_STR_LIST: {
				int first = 1;
				const size_t *str;
				uint strs;
				list_node_t it = other_op->args.l;
				list_foreach(&other->lists, it, str)
				{
					strv_t val = strvbuf_get(&other->strs, *str);
					if (first) {
						config_str_list(
							config, other_op->type, other_op->pkg, other_op->tgt, other_op->mode, val, &strs);
						first = 0;
					} else {
						config_str_list_add(config, strs, val);
					}
				}

				break;
			}
			default:
				log_error("cbuild", "config", NULL, "unknown op: %d", other_op->type);
				break;
			}
		}
	}
	return ret;
}

static const char *s_config_mode_str[] = {
	[CONFIG_MODE_UNKNOWN] = "",
	[CONFIG_MODE_SET]     = "",
	[CONFIG_MODE_EN]      = "?",
	[CONFIG_MODE_APP]     = "+",
};

static const char *config_mode_str(config_mode_t mode)
{
	if (mode < CONFIG_MODE_UNKNOWN || mode >= __CONFIG_MODE_CNT) {
		log_error("cbuild", "config", NULL, "invalid set mode: %d", mode);
		mode = CONFIG_MODE_UNKNOWN;
	}

	return s_config_mode_str[mode];
}

size_t config_print(const config_t *config, const registry_t *registry, dst_t dst)
{
	if (config == NULL || registry == NULL) {
		return 0;
	}

	size_t off = dst.off;

	config_op_t *op;
	uint i = 0;
	arr_foreach(&config->ops, i, op)
	{
		switch (op_types[op->type]) {
		case OP_TYPE_SEL: {
			const char *mode = config_mode_str(op->mode);
			if (op->type == CONFIG_OP_TYPE_PKG) {
				size_t *name_off = arr_get(&registry->pkgs, op->pkg);
				strv_t name	 = strvbuf_get(&registry->strs, *name_off);
				if (i == 0) {
					dst.off += dputf(dst, "%s %s= %.*s\n", op_type_str(op->type), mode, name.len, name.data);
				} else {
					dst.off += dputf(dst, "\n%s %s= %.*s\n", op_type_str(op->type), mode, name.len, name.data);
				}
			} else {
				strv_t pkg	 = registry_get_pkg(registry, op->pkg);
				size_t *name_off = arr_get(&registry->tgts, op->tgt);
				strv_t name	 = strvbuf_get(&registry->strs, *name_off);
				if (i == 0) {
					dst.off += dputf(dst,
							 "%.*s:%s %s= %.*s\n",
							 pkg.len,
							 pkg.data,
							 op_type_str(op->type),
							 mode,
							 name.len,
							 name.data);
				} else {
					dst.off += dputf(dst,
							 "\n%.*s:%s %s= %.*s\n",
							 pkg.len,
							 pkg.data,
							 op_type_str(op->type),
							 mode,
							 name.len,
							 name.data);
				}
			}

			break;
		}
		case OP_TYPE_STR: {
			strv_t pkg	 = registry_get_pkg(registry, op->pkg);
			strv_t tgt	 = op->tgt == (uint)-1 ? STRV("") : registry_get_tgt(registry, op->tgt);
			const char *mode = config_mode_str(op->mode);
			strv_t path	 = strvbuf_get(&config->strs, op->args.s);
			dst.off += dputf(dst,
					 "%.*s:%.*s:%s %s= %.*s\n",
					 pkg.len,
					 pkg.data,
					 tgt.len,
					 tgt.data,
					 op_type_str(op->type),
					 mode,
					 path.len,
					 path.data);
			break;
		}
		case OP_TYPE_INT: {
			strv_t pkg	 = registry_get_pkg(registry, op->pkg);
			strv_t tgt	 = op->tgt == (uint)-1 ? STRV("") : registry_get_tgt(registry, op->tgt);
			const char *mode = config_mode_str(op->mode);
			dst.off += dputf(dst,
					 "%.*s:%.*s:%s %s= %d\n",
					 pkg.len,
					 pkg.data,
					 tgt.len,
					 tgt.data,
					 op_type_str(op->type),
					 mode,
					 op->args.i);
			break;
		}
		case OP_TYPE_STR_LIST: {
			strv_t pkg	 = registry_get_pkg(registry, op->pkg);
			strv_t tgt	 = op->tgt == (uint)-1 ? STRV("") : registry_get_tgt(registry, op->tgt);
			const char *mode = config_mode_str(op->mode);
			dst.off += dputf(dst, "%.*s:%.*s:%s %s= ", pkg.len, pkg.data, tgt.len, tgt.data, op_type_str(op->type), mode);
			int first = 1;
			const size_t *str;
			list_node_t it = op->args.l;
			list_foreach(&config->lists, it, str)
			{
				strv_t val = strvbuf_get(&config->strs, *str);
				if (first) {
					dst.off += dputf(dst, "%.*s", val.len, val.data);
					first = 0;
				} else {
					dst.off += dputf(dst, ", %.*s", val.len, val.data);
				}
			}
			dst.off += dputf(dst, "\n");
			break;
		}
		default:
			log_error("cbuild", "config", NULL, "unknown op: %d", op->type);
			break;
		}
	}

	return dst.off - off;
}
