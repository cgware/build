#include "config.h"

#include "log.h"

config_t *config_init(config_t *config, uint cap, alloc_t alloc)
{
	if (config == NULL) {
		return NULL;
	}

	if (arr_init(&config->vals, cap, sizeof(config_val_t), alloc) == NULL ||
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

	arr_free(&config->vals);
	list_free(&config->lists);
	strvbuf_free(&config->strs);
}

int config_get_state(const config_t *config, config_state_t *state)
{
	if (config == NULL || state == NULL) {
		return 1;
	}

	state->vals_cnt	 = config->vals.cnt;
	state->lists_cnt = config->lists.cnt;
	state->strs_used = config->strs.used;

	return 0;
}

int config_set_state(config_t *config, config_state_t state)
{
	if (config == NULL) {
		return 1;
	}

	arr_reset(&config->vals, state.vals_cnt);
	list_reset(&config->lists, state.lists_cnt);
	strvbuf_reset(&config->strs, state.strs_used);

	return 0;
}

static config_val_t *config_add_val(config_t *config, uint op, uint pkg, uint tgt, config_act_t act, uint *id)
{
	config_val_t *val = arr_add(&config->vals, id);
	if (val == NULL) {
		log_error("cbuild", "config", NULL, "failed to add op");
		return NULL;
	}

	val->op	 = op;
	val->pkg = pkg;
	val->tgt = tgt;
	val->act = act;

	return val;
}

int config_int(config_t *config, uint op, uint pkg, uint tgt, config_act_t act, int val)
{
	if (config == NULL) {
		return 1;
	}

	config_val_t *v = config_add_val(config, op, pkg, tgt, act, NULL);
	if (v == NULL) {
		log_error("cbuild", "config", NULL, "failed to add int op with val: %d", val);
		return 1;
	}

	v->args.i = val;

	return 0;
}

int config_str(config_t *config, uint op, uint pkg, uint tgt, config_act_t act, strv_t str)
{
	if (config == NULL) {
		return 1;
	}

	size_t strs_cnt;
	if (strvbuf_add(&config->strs, str, &strs_cnt)) {
		log_error("cbuild", "config", NULL, "failed to add str: %.*s", str.len, str.data);
		return 1;
	}

	config_val_t *v = config_add_val(config, op, pkg, tgt, act, NULL);
	if (v == NULL) {
		log_error("cbuild", "config", NULL, "failed to add string op: %.*s", str.len, str.data);
		strvbuf_reset(&config->strs, strs_cnt);
		return 1;
	}

	v->args.s = strs_cnt;

	return 0;
}

int config_str_list(config_t *config, uint op, uint pkg, uint tgt, config_act_t act, strv_t str, uint *id)
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

	if (id == NULL || *id == (uint)-1) {
		config_val_t *v = config_add_val(config, op, pkg, tgt, act, id);
		if (v == NULL) {
			log_error("cbuild", "config", NULL, "failed to add list op: %.*s", str.len, str.data);
			strvbuf_reset(&config->strs, strs_cnt);
			list_reset(&config->lists, list);
			return 1;
		}

		v->args.l = list;
	} else {
		const config_val_t *v = arr_get(&config->vals, *id);
		if (v == NULL) {
			log_error("cbuild", "config", NULL, "invalid list id: %d", id);
			strvbuf_reset(&config->strs, strs_cnt);
			list_reset(&config->lists, list);
			return 1;
		}

		if (list_app(&config->lists, v->args.l, list)) {
			log_error("cbuild", "config", NULL, "failed to append list");
			strvbuf_reset(&config->strs, strs_cnt);
			list_reset(&config->lists, list);
			return 1;
		}
	}

	return 0;
}

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
	const config_val_t *v = arr_get(&config->vals, id);

	int ret = 0;

	const size_t *other_str;
	list_foreach(&other->lists, other_list, other_str)
	{
		strv_t other_val = strvbuf_get(&other->strs, *other_str);
		const size_t *b_str;
		int found      = 0;
		list_node_t it = v->args.l;
		list_foreach(&config->lists, it, b_str)
		{
			strv_t val = strvbuf_get(&config->strs, *b_str);
			if (strv_eq(other_val, val)) {
				found = 1;
				break;
			}
		}

		if (!found) {
			ret |= config_str_list(config, v->op, v->pkg, v->tgt, v->act, other_val, &id);
		}
	}

	return ret;
}

int config_merge(config_t *config, const config_t *other, config_state_t start, const config_schema_t *schema, const registry_t *registry)
{
	if (config == NULL || other == NULL) {
		return 1;
	}

	int ret = 0;

	config_val_t *ov;
	uint i = start.vals_cnt;
	arr_foreach(&other->vals, i, ov)
	{
		int found = 0;
		config_val_t *v;
		uint j = 0;
		arr_foreach(&config->vals, j, v)
		{
			if (v->op == ov->op && v->pkg == ov->pkg && v->tgt == ov->tgt) {
				found = 1;
				break;
			}
		}

		if (found) {
			config_act_t act = v->act;
			if (ov->act < act) {
				act = ov->act;
			}

			config_schema_op_t *op = config_schema_get_op(schema, v->op);
			if (op == NULL) {
				log_error("cbuild", "config", NULL, "invalid op: %d", v->op);
				continue;
			}

			strv_t pkg  = op->scope == CONFIG_SCOPE_PKG || op->scope == CONFIG_SCOPE_TGT ? registry_get_pkg(registry, v->pkg)
												     : STRV("");
			strv_t tgt  = op->scope == CONFIG_SCOPE_TGT ? registry_get_tgt(registry, v->tgt) : STRV("");
			strv_t name = strvbuf_get(&schema->strs, op->name);

			switch (op->type) {
			case CONFIG_TYPE_INT: {
				int val	      = v->args.i;
				int other_val = ov->args.i;
				if (val != other_val) {
					if (other->prio == config->prio) {
						log_error("cbuild",
							  "config",
							  NULL,
							  "value mismatch: %.*s:%.*s:%.*s: %d != %d",
							  pkg.len,
							  pkg.data,
							  tgt.len,
							  tgt.data,
							  name.len,
							  name.data,
							  val,
							  other_val);
						ret |= 1;
					} else if (other->prio > config->prio) {
						v->args.i = other_val;
					}
				} else {
					v->act = act;
				}
				break;
			}
			case CONFIG_TYPE_STR: {
				strv_t val	 = strvbuf_get(&config->strs, v->args.s);
				strv_t other_val = strvbuf_get(&other->strs, ov->args.s);
				if (!strv_eq(val, other_val)) {
					if (other->prio == config->prio) {
						log_error("cbuild",
							  "config",
							  NULL,
							  "value mismatch: %.*s:%.*s:%.*s: %.*s != %.*s",
							  pkg.len,
							  pkg.data,
							  tgt.len,
							  tgt.data,
							  name.len,
							  name.data,
							  val.len,
							  val.data,
							  other_val.len,
							  other_val.data);
						ret |= 1;
					} else if (other->prio > config->prio) {
						strvbuf_add(&config->strs, other_val, &v->args.s);
					}

				} else {
					v->act = act;
				}
				break;
			}
			case CONFIG_TYPE_STR_LIST: {
				if (ov->act == CONFIG_ACT_APP) {
					config_str_list_app_new(config, j, other, ov->args.l);
					v->act = act;
				} else {
					strv_t a = contains_str_list(config, v->args.l, other, ov->args.l);
					strv_t b = contains_str_list(other, ov->args.l, config, v->args.l);
					if (a.data == NULL && b.data == NULL) {
						v->act = act;
					} else {
						if (other->prio == config->prio) {
							strv_t val = a.data != NULL ? a : b;
							log_error("cbuild",
								  "config",
								  NULL,
								  "value mismatch: %.*s:%.*s:%.*s: missing: %.*s",
								  pkg.len,
								  pkg.data,
								  tgt.len,
								  tgt.data,
								  name.len,
								  name.data,
								  val.len,
								  val.data);

							ret |= 1;
						} else if (other->prio > config->prio) {
							const size_t *off;
							list_node_t it = ov->args.l;
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

								if (it == ov->args.l) {
									v->args.l = list;
								} else {
									list_app(&config->lists, v->args.l, list);
								}
							}
						}
					}
				}
				break;
			}
			default:
				log_error("cbuild", "config", NULL, "unknown op type: %d", op->type);
				ret = 1;
				break;
			}
		} else {
			config_schema_op_t *op = config_schema_get_op(schema, ov->op);
			if (op == NULL) {
				log_error("cbuild", "config", NULL, "invalid op: %d", ov->op);
				continue;
			}

			switch (op->type) {
			case CONFIG_TYPE_INT: {
				config_int(config, ov->op, ov->pkg, ov->tgt, ov->act, ov->args.i);
				break;
			}
			case CONFIG_TYPE_STR: {
				config_str(config, ov->op, ov->pkg, ov->tgt, ov->act, strvbuf_get(&other->strs, ov->args.s));
				break;
			}
			case CONFIG_TYPE_STR_LIST: {
				const size_t *str;
				uint strs      = -1;
				list_node_t it = ov->args.l;
				list_foreach(&other->lists, it, str)
				{
					strv_t val = strvbuf_get(&other->strs, *str);
					config_str_list(config, ov->op, ov->pkg, ov->tgt, ov->act, val, &strs);
				}

				break;
			}
			default:
				log_error("cbuild", "config", NULL, "invalid op type: %d", op->type);
				ret = 1;
				break;
			}
		}
	}
	return ret;
}

static const char *s_config_act_str[] = {
	[CONFIG_ACT_UNKNOWN] = "",
	[CONFIG_ACT_SET]     = "",
	[CONFIG_ACT_EN]	     = "?",
	[CONFIG_ACT_APP]     = "+",
};

static const char *config_act_str(config_act_t act)
{
	if (act < CONFIG_ACT_UNKNOWN || act >= __CONFIG_ACT_CNT) {
		log_error("cbuild", "config", NULL, "invalid set act: %d", act);
		act = CONFIG_ACT_UNKNOWN;
	}

	return s_config_act_str[act];
}

size_t config_print(const config_t *config, const config_schema_t *schema, const registry_t *registry, dst_t dst)
{
	if (config == NULL) {
		return 0;
	}

	size_t off = dst.off;

	config_val_t *v;
	uint i = 0;
	arr_foreach(&config->vals, i, v)
	{
		config_schema_op_t *op = config_schema_get_op(schema, v->op);
		if (op == NULL) {
			log_error("cbuild", "config", NULL, "invalid op: %d", v->op);
			continue;
		}

		strv_t name	= strvbuf_get(&schema->strs, op->name);
		const char *act = config_act_str(v->act);

		switch (op->scope) {
		case CONFIG_SCOPE_GLOBAL: {
			dst.off += dputf(dst, "%.*s %s= ", name.len, name.data, act);
			break;
		}
		case CONFIG_SCOPE_PKG: {
			strv_t pkg = registry_get_pkg(registry, v->pkg);
			dst.off += dputf(dst, "%.*s:%.*s %s= ", pkg.len, pkg.data, name.len, name.data, act);
			break;
		}
		case CONFIG_SCOPE_TGT: {
			strv_t pkg = registry_get_pkg(registry, v->pkg);
			strv_t tgt = registry_get_tgt(registry, v->tgt);
			dst.off += dputf(dst, "%.*s:%.*s:%.*s %s= ", pkg.len, pkg.data, tgt.len, tgt.data, name.len, name.data, act);
			break;
		}
		default:
			log_error("cbuild", "config", NULL, "invalid op scope: %d", op->scope);
			break;
		}

		switch (op->type) {
		case CONFIG_TYPE_INT: {
			dst.off += dputf(dst, "%d\n", v->args.i);
			break;
		}
		case CONFIG_TYPE_STR: {
			strv_t val = strvbuf_get(&config->strs, v->args.s);
			dst.off += dputf(dst, "%.*s\n", val.len, val.data);
			break;
		}
		case CONFIG_TYPE_STR_LIST: {
			int first = 1;
			const size_t *str;
			list_node_t it = v->args.l;
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
			log_error("cbuild", "config", NULL, "invalid op type: %d", op->type);
			break;
		}
	}

	return dst.off - off;
}
