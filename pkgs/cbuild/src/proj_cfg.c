#include "proj_cfg.h"

#include "log.h"
#include "mod_base.h"

int proj_cfg(proj_t *proj, const config_t *config, const config_schema_t *schema, const registry_t *registry)
{
	(void)registry;

	if (proj == NULL || config == NULL) {
		return 1;
	}

	int ret = 0;

	proj_set_str(proj, proj->outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint i = 0;
	config_val_t *v;
	arr_foreach(&config->vals, i, v)
	{
		config_schema_op_t *op = config_schema_get_op(schema, v->op);
		if (op == NULL) {
			log_error("cbuild", "config", NULL, "invalid op: %d", v->op);
			ret = 1;
			continue;
		}

		switch (v->op) {
		case CONFIG_PKGS: {
			if (v->act != CONFIG_ACT_APP && v->act != CONFIG_ACT_EN) {
				break;
			}

			const size_t *str;
			list_node_t it = v->args.l;
			list_foreach(&config->lists, it, str)
			{
				strv_t name = strvbuf_get(&config->strs, *str);
				if (proj_find_pkg(proj, name, NULL)) {
					if (v->act == CONFIG_ACT_APP) {
						log_error("cbuild", "proj_cfg", NULL, "package already exists: %.*s", name.len, name.data);
						ret = 1;
					}
					break;
				}

				pkg_t *pkg = proj_add_pkg(proj, NULL);
				if (pkg == NULL) {
					log_error("cbuild", "proj_cfg", NULL, "failed to create package: %.*s", name.len, name.data);
					return 1;
				}

				proj_set_str(proj, pkg->strs + PKG_STR_NAME, name);
			}

			break;
		}
		case CONFIG_TGTS: {
			if (v->act != CONFIG_ACT_APP && v->act != CONFIG_ACT_EN) {
				break;
			}

			const size_t *str;
			list_node_t it = v->args.l;
			list_foreach(&config->lists, it, str)
			{
				strv_t name = strvbuf_get(&config->strs, *str);

				if (proj_find_target(proj, v->pkg, name, NULL)) {
					if (v->act == CONFIG_ACT_APP) {
						log_error("cbuild", "proj_cfg", NULL, "target already exists: %.*s", name.len, name.data);
						ret = 1;
					}
					break;
				}

				target_t *tgt = proj_add_target(proj, v->pkg, NULL);
				if (tgt == NULL) {
					log_error("cbuild", "proj_cfg", NULL, "failed to create target");
					return 1;
				}

				proj_set_str(proj, tgt->strs + TGT_STR_NAME, name);
			}

			break;
		}
		case CONFIG_TGT_TYPE: {
			target_t *tgt = proj_get_target(proj, v->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target type specified before target name");
				ret = 1;
				break;
			}
			tgt->type = v->args.i;
			break;
		}
		}
	}

	i = 0;

	arr_foreach(&config->vals, i, v)
	{
		config_schema_op_t *op = config_schema_get_op(schema, v->op);
		if (op == NULL) {
			log_error("cbuild", "config", NULL, "invalid op: %d", v->op);
			continue;
		}

		switch (v->op) {
		case CONFIG_PKGS:
		case CONFIG_TGTS:
		case CONFIG_TGT_TYPE:
			break;
		default: {
			mod_t *mod = op->priv;
			if (mod && mod->apply_val) {
				ret |= mod->apply_val(mod, schema, config, v, proj);
			}
			break;
		}
		}
	}

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (mod->proj_cfg) {
			ret |= mod->proj_cfg(mod, proj);
		}
	}

	return ret;
}
