#include "proj_cfg.h"

#include "log.h"
#include "mod.h"
#include "proj_utils.h"

int proj_cfg(proj_t *proj, const config_t *config, const registry_t *registry)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;

	proj_set_str(proj, proj->outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint i = 0;
	config_op_t *op;
	arr_foreach(&config->ops, i, op)
	{
		switch (op->type) {
		case CONFIG_OP_TYPE_PKG: {
			if (op->mode != CONFIG_MODE_APP && op->mode != CONFIG_MODE_EN) {
				break;
			}

			size_t *off = arr_get(&registry->pkgs, op->pkg);
			strv_t name = strvbuf_get(&registry->strs, *off);

			if (proj_find_pkg(proj, name, NULL)) {
				if (op->mode == CONFIG_MODE_APP) {
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

			break;
		}
		case CONFIG_OP_TYPE_TGT: {
			if (op->mode != CONFIG_MODE_APP && op->mode != CONFIG_MODE_EN) {
				break;
			}

			size_t *off = arr_get(&registry->tgts, op->tgt);
			strv_t name = strvbuf_get(&registry->strs, *off);

			if (proj_find_target(proj, op->pkg, name, NULL)) {
				if (op->mode == CONFIG_MODE_APP) {
					log_error("cbuild", "proj_cfg", NULL, "target already exists: %.*s", name.len, name.data);
					ret = 1;
				}
				break;
			}

			target_t *tgt = proj_add_target(proj, op->pkg, NULL);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "failed to create target");
				return 1;
			}

			proj_set_str(proj, tgt->strs + TGT_STR_NAME, name);

			break;
		}
		case CONFIG_OP_TYPE_TGT_TYPE: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target type specified before target name");
				ret = 1;
				break;
			}
			tgt->type = op->args.i;
			break;
		}
		default:
			break;
		}
	}

	i = 0;
	arr_foreach(&config->ops, i, op)
	{
		switch (op->type) {
		case CONFIG_OP_TYPE_PKG_PATH: {
			pkg_t *pkg = proj_get_pkg(proj, op->pkg);
			if (pkg == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "package does not exist: %d", op->pkg);
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, pkg->strs + PKG_STR_PATH, val);
			break;
		}
		case CONFIG_OP_TYPE_PKG_INC: {
			pkg_t *pkg = proj_get_pkg(proj, op->pkg);
			if (pkg == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "package does not exist: %d", op->pkg);
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			if (pkg->has_targets) {
				target_t *target;
				list_node_t k = pkg->targets;
				list_foreach(&proj->targets, k, target)
				{
					proj_set_str(proj, target->strs + TGT_STR_INC, val);
				}
			}
			break;
		}
		case CONFIG_OP_TYPE_PKG_URI: {
			pkg_t *pkg = proj_get_pkg(proj, op->pkg);
			if (pkg == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "package does not exist: %d", op->pkg);
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			ret |= proj_set_uri(proj, pkg, val);
			break;
		}
		case CONFIG_OP_TYPE_PKG_DEPS: {
			pkg_t *pkg = proj_get_pkg(proj, op->pkg);
			if (pkg == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "package does not exist: %d", op->pkg);
				ret = 1;
				break;
			}
			const size_t *str;
			list_node_t it = op->args.l;
			list_foreach(&config->lists, it, str)
			{
				strv_t dep_pkg_name, dep_target_name;
				strv_lsplit(strvbuf_get(&config->strs, *str), ':', &dep_pkg_name, &dep_target_name);

				uint dep_pkg_id;
				pkg_t *dep_pkg = proj_find_pkg(proj, dep_pkg_name, &dep_pkg_id);
				if (dep_pkg == NULL) {
					log_error(
						"cbuild", "proj_cfg", NULL, "package not found: %.*s", dep_pkg_name.len, dep_pkg_name.data);
					ret = 1;
					continue;
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
						ret = 1;
						continue;
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
		case CONFIG_OP_TYPE_TGT_SRC: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target does not exist: %d", op->tgt);
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, tgt->strs + TGT_STR_SRC, val);
			break;
		}
		case CONFIG_OP_TYPE_TGT_INC: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target include specified before target name");
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, tgt->strs + TGT_STR_INC, val);
			break;
		}
		case CONFIG_OP_TYPE_TGT_INCS_PRIV: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target include specified before target name");
				ret = 1;
				break;
			}
			const size_t *str;
			list_node_t it = op->args.l;
			list_foreach(&config->lists, it, str)
			{
				strv_t val = strvbuf_get(&config->strs, *str);
				ret |= proj_add_inc_priv(proj, op->tgt, val);
			}
			break;
		}
		case CONFIG_OP_TYPE_TGT_DEPS: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target include specified before target name");
				ret = 1;
				break;
			}

			const size_t *str;
			list_node_t it = op->args.l;
			list_foreach(&config->lists, it, str)
			{
				strv_t dep_pkg_name, dep_target_name;
				strv_lsplit(strvbuf_get(&config->strs, *str), ':', &dep_pkg_name, &dep_target_name);

				uint dep_pkg_id;
				pkg_t *dep_pkg = proj_find_pkg(proj, dep_pkg_name, &dep_pkg_id);
				if (dep_pkg == NULL) {
					log_error(
						"cbuild", "proj_cfg", NULL, "package not found: %.*s", dep_pkg_name.len, dep_pkg_name.data);
					ret = 1;
					continue;
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
						ret = 1;
						continue;
					}
					proj_add_dep(proj, op->tgt, dep_target_id);
				} else {
					if (dep_pkg->has_targets) {
						target_t *target;
						list_node_t j = dep_pkg->targets;
						list_foreach(&proj->targets, j, target)
						{
							if (target->type == TARGET_TYPE_TST) {
								continue;
							}

							proj_add_dep(proj, op->tgt, j);
						}
					}
				}
			}
			break;
		}
		case CONFIG_OP_TYPE_TGT_PREP: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target lib specified before target name");
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, tgt->strs + TGT_STR_PREP, val);
			break;
		}
		case CONFIG_OP_TYPE_TGT_CONF: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target lib specified before target name");
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, tgt->strs + TGT_STR_CONF, val);
			break;
		}
		case CONFIG_OP_TYPE_TGT_COMP: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target lib specified before target name");
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, tgt->strs + TGT_STR_COMP, val);
			break;
		}
		case CONFIG_OP_TYPE_TGT_INST: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target lib specified before target name");
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, tgt->strs + TGT_STR_INST, val);
			break;
		}
		case CONFIG_OP_TYPE_TGT_OUT: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target lib specified before target name");
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, tgt->strs + TGT_STR_OUT, val);
			break;
		}
		case CONFIG_OP_TYPE_TGT_LIB: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target lib specified before target name");
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, tgt->strs + TGT_STR_TGT, val);
			tgt->out_type = TARGET_TGT_TYPE_LIB;
			break;
		}
		case CONFIG_OP_TYPE_TGT_EXE: {
			target_t *tgt = proj_get_target(proj, op->tgt);
			if (tgt == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target exe specified before target name");
				ret = 1;
				break;
			}
			strv_t val = strvbuf_get(&config->strs, op->args.s);
			proj_set_str(proj, tgt->strs + TGT_STR_TGT, val);
			tgt->out_type = TARGET_TGT_TYPE_EXE;
			break;
		}
		default:
			break;
		}
	}

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		mod->proj_cfg(mod, proj);
	}

	return ret;
}
