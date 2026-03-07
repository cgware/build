#include "mod_base.h"

#include "log.h"
#include "path.h"

static int mod_base_apply_val(mod_t *mod, const config_schema_t *schema, const config_t *config, const config_val_t *v, proj_t *proj)

{
	config_schema_op_t *op = config_schema_get_op(schema, v->op);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "invalid op: %d", v->op);
		return 1;
	}

	pkg_t *pkg    = NULL;
	target_t *tgt = NULL;

	switch (op->scope) {
	case CONFIG_SCOPE_PKG: {
		pkg = proj_get_pkg(proj, v->pkg);
		if (pkg == NULL) {
			log_error("cbuild", "proj_cfg", NULL, "package does not exist: %d", v->pkg);
			return 1;
		}
		break;
	}
	case CONFIG_SCOPE_TGT: {
		tgt = proj_get_target(proj, v->tgt);
		if (tgt == NULL) {
			log_error("cbuild", "proj_cfg", NULL, "target does not exist: %d", v->tgt);
			return 1;
		}
		break;
	}
	default: {
		log_error("cbuild", "mod_cfg", NULL, "invalid op scope: %d", op->scope);
		return 1;
	}
	}

	switch (v->op - mod->ops) {
	case CONFIG_PKG_PATH: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_str(proj, pkg->strs + PKG_STR_PATH, val);
		break;
	}
	case CONFIG_PKG_DEPS: {
		const size_t *str;
		list_node_t it = v->args.l;
		list_foreach(&config->lists, it, str)
		{
			strv_t dep_pkg_name, dep_target_name;
			strv_lsplit(strvbuf_get(&config->strs, *str), ':', &dep_pkg_name, &dep_target_name);

			uint dep_pkg_id;
			pkg_t *dep_pkg = proj_find_pkg(proj, dep_pkg_name, &dep_pkg_id);
			if (dep_pkg == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "package not found: %.*s", dep_pkg_name.len, dep_pkg_name.data);
				return 1;
			}

			if (dep_target_name.len > 0) {
				uint dep_target_id;
				target_t *target = proj_find_target(proj, dep_pkg_id, dep_target_name, &dep_target_id);
				if (target == NULL) {
					log_error("cbuild",
						  "proj_cfg",
						  NULL,
						  "target not found: %.*s:%.*s",
						  dep_pkg_name.len,
						  dep_pkg_name.data,
						  dep_target_name.len,
						  dep_target_name.data);
					return 1;
				}
				if (pkg->has_targets) {
					target_t *target;
					list_node_t k = pkg->targets;
					list_foreach(&proj->targets, k, target)
					{
						proj_add_dep(proj, k, dep_target_id);
					}
				}
			} else {
				if (dep_pkg->has_targets) {
					target_t *target;
					list_node_t j = dep_pkg->targets;
					list_foreach(&proj->targets, j, target)
					{
						if (target->type == TARGET_TYPE_TST) {
							continue;
						}

						if (pkg->has_targets) {
							target_t *target;
							list_node_t k = pkg->targets;
							list_foreach(&proj->targets, k, target)
							{
								proj_add_dep(proj, k, j);
							}
						}
					}
				}
			}
		}
		break;
	}
	case CONFIG_TGT_SRC: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_str(proj, tgt->strs + TGT_STR_SRC, val);
		break;
	}
	case CONFIG_TGT_INC: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_str(proj, tgt->strs + TGT_STR_INC, val);
		break;
	}
	case CONFIG_TGT_INCS_PRIV: {
		const size_t *str;
		list_node_t it = v->args.l;
		list_foreach(&config->lists, it, str)
		{
			strv_t val = strvbuf_get(&config->strs, *str);
			if (proj_add_inc_priv(proj, v->tgt, val)) {
				return 1;
			}
		}
		break;
	}
	case CONFIG_TGT_DEPS: {
		const size_t *str;
		list_node_t it = v->args.l;
		list_foreach(&config->lists, it, str)
		{
			strv_t dep_pkg_name, dep_target_name;
			strv_lsplit(strvbuf_get(&config->strs, *str), ':', &dep_pkg_name, &dep_target_name);

			uint dep_pkg_id;
			pkg_t *dep_pkg = proj_find_pkg(proj, dep_pkg_name, &dep_pkg_id);
			if (dep_pkg == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "package not found: %.*s", dep_pkg_name.len, dep_pkg_name.data);
				return 1;
			}

			if (dep_target_name.len > 0) {
				uint dep_target_id;
				target_t *target = proj_find_target(proj, dep_pkg_id, dep_target_name, &dep_target_id);
				if (target == NULL) {
					log_error("cbuild",
						  "proj_cfg",
						  NULL,
						  "target not found: %.*s:%.*s",
						  dep_pkg_name.len,
						  dep_pkg_name.data,
						  dep_target_name.len,
						  dep_target_name.data);
					return 1;
				}
				proj_add_dep(proj, v->tgt, dep_target_id);
			} else {
				if (dep_pkg->has_targets) {
					target_t *target;
					list_node_t j = dep_pkg->targets;
					list_foreach(&proj->targets, j, target)
					{
						if (target->type == TARGET_TYPE_TST) {
							continue;
						}

						proj_add_dep(proj, v->tgt, j);
					}
				}
			}
		}
		break;
	}
	}

	return 0;
}

static int s_mod_base_init(mod_t *mod, uint cap, config_schema_t *config_schema, alloc_t alloc)
{
	(void)cap;
	(void)alloc;

	mod->ops = config_schema->ops.cnt;

	config_schema_op_desc_t ops[] = {
		{.name = STRV("pkgs"), .scope = CONFIG_SCOPE_GLOBAL, .type = CONFIG_TYPE_STR_LIST, .priv = mod},
		{.name = STRV("path"), .scope = CONFIG_SCOPE_PKG, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("deps"), .scope = CONFIG_SCOPE_PKG, .type = CONFIG_TYPE_STR_LIST, .priv = mod},
		{.name = STRV("tgts"), .scope = CONFIG_SCOPE_PKG, .type = CONFIG_TYPE_STR_LIST, .priv = mod},
		{.name = STRV("type"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_INT, .priv = mod},
		{.name = STRV("src"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("inc"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("incs_priv"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR_LIST, .priv = mod},
		{.name = STRV("deps"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR_LIST, .priv = mod},

	};

	config_schema_add_ops(config_schema, ops, sizeof(ops));

	return 0;
}

static mod_t mod_base = {
	.name	   = STRVT("mod_base"),
	.init	   = s_mod_base_init,
	.apply_val = mod_base_apply_val,
};

int mod_base_init(uint cap, config_schema_t *config_schema, alloc_t alloc)
{
	return mod_base.init(&mod_base, cap, config_schema, alloc);
}

mod_t *mod_base_get()
{
	return &mod_base;
}
