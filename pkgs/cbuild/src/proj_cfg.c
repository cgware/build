#include "proj_cfg.h"

#include "log.h"
#include "proj_utils.h"

int proj_cfg(proj_t *proj, const config_t *config)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;
	uint i;

	proj_set_str(proj, proj->outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	i = 0;
	const config_dir_t *dir;
	arr_foreach(&config->dirs, i, dir)
	{
		strv_t name = config_get_str(config, dir->strs + CONFIG_DIR_NAME);
		strv_t path = config_get_str(config, dir->strs + CONFIG_DIR_PATH);
		strv_t src  = config_get_str(config, dir->strs + CONFIG_DIR_SRC);
		strv_t inc  = config_get_str(config, dir->strs + CONFIG_DIR_INC);
		strv_t drv  = config_get_str(config, dir->strs + CONFIG_DIR_DRV);
		strv_t test = config_get_str(config, dir->strs + CONFIG_DIR_TST);

		uint pkg_id;
		pkg_t *pkg	 = NULL;
		target_t *target = NULL;

		if (src.len > 0 || inc.len > 0 || test.len > 0) {
			pkg = proj_add_pkg(proj, &pkg_id);

			proj_set_str(proj, pkg->strs + PKG_NAME, name);
			proj_set_str(proj, pkg->strs + PKG_PATH, path);
			proj_set_str(proj, pkg->strs + PKG_SRC, src);
			proj_set_str(proj, pkg->strs + PKG_INC, inc);
			proj_set_str(proj, pkg->strs + PKG_DRV, drv);
			proj_set_str(proj, pkg->strs + PKG_TST, test);

			if (src.len > 0 || inc.len > 0) {
				list_node_t target_id;
				target = proj_add_target(proj, pkg_id, &target_id);
				proj_set_str(proj, target->strs + TARGET_NAME, name);

				if (src.len > 0 && inc.len > 0) {
					target->type = dir->has_main ? TARGET_TYPE_EXE : TARGET_TYPE_LIB;
				} else if (src.len > 0) {
					target->type = TARGET_TYPE_EXE;
				} else if (inc.len > 0) {
					target->type = TARGET_TYPE_LIB;
				}
			}

			if (test.len > 0) {
				list_node_t target_id;
				target = proj_add_target(proj, pkg_id, &target_id);
				proj_set_str(proj, target->strs + TARGET_NAME, name);
				strbuf_app(&proj->strs, target->strs + TARGET_NAME, STRV("_test"));
				target->type = TARGET_TYPE_TST;
			}
		}

		if (dir->has_pkgs) {
			config_pkg_t *cfg_pkg;
			list_node_t pkgs = dir->pkgs;
			list_foreach(&config->pkgs, pkgs, cfg_pkg)
			{
				strv_t uri = config_get_str(config, cfg_pkg->strs + CONFIG_PKG_URI);

				if (pkg == NULL || uri.len > 0) {
					pkg = proj_add_pkg(proj, &pkg_id);

					proj_set_str(proj, pkg->strs + PKG_PATH, path);
					proj_set_str(proj, pkg->strs + PKG_SRC, src);
					proj_set_str(proj, pkg->strs + PKG_INC, inc);
					proj_set_str(proj, pkg->strs + PKG_DRV, drv);
					proj_set_str(proj, pkg->strs + PKG_TST, test);

					if (uri.len > 0) {
						ret |= proj_set_uri(proj, pkg, uri);
						name = proj_get_str(proj, pkg->strs + PKG_NAME);
					} else {
						proj_set_str(proj, pkg->strs + PKG_NAME, name);
					}
				}

				cfg_pkg->pkg = pkg_id;

				if (cfg_pkg->has_targets) {
					config_target_t *cfg_target;
					list_node_t targets = cfg_pkg->targets;

					list_node_t target_id = pkg->targets;

					target = pkg->has_targets ? list_get(&proj->targets, pkg->targets) : NULL;

					int created = 0;

					list_foreach(&config->targets, targets, cfg_target)
					{
						strv_t cmd = config_get_str(config, cfg_target->strs + CONFIG_TARGET_CMD);
						strv_t out = config_get_str(config, cfg_target->strs + CONFIG_TARGET_OUT);

						if (target == NULL || uri.len > 0) {
							target = proj_add_target(proj, pkg_id, &target_id);
							proj_set_str(proj, target->strs + TARGET_NAME, name);
							created = 1;
						}

						proj_set_str(proj, target->strs + TARGET_CMD, cmd);
						proj_set_str(proj, target->strs + TARGET_OUT, out);

						if (uri.len > 0) {
							target->type = TARGET_TYPE_EXT;
						}

						if (!created) {
							target = list_get_next(&proj->targets, target_id, &target_id);
						}
					}
				}

				pkg = NULL;
			}
		}
	}

	i = 0;
	config_pkg_t *cfg_pkg;
	list_foreach_all(&config->pkgs, i, cfg_pkg)
	{
		if (cfg_pkg->has_deps) {
			const uint *dep;
			list_node_t deps = cfg_pkg->deps;
			list_foreach(&config->deps, deps, dep)
			{
				strv_t dep_str = config_get_str(config, *dep);
				uint dep_pkg_id;
				pkg_t *dep_pkg = proj_find_pkg(proj, dep_str, &dep_pkg_id);
				if (dep_pkg == NULL) {
					log_error("cbuild", "proj_cfg", NULL, "package not found: %.*s", dep_str.len, dep_str.data);
					continue;
				}

				uint dep_target_id;
				uint found = 0;
				if (dep_pkg->has_targets) {
					target_t *target;
					list_node_t j = dep_pkg->targets;
					list_foreach(&proj->targets, j, target)
					{
						dep_target_id = j;
						found	      = 1;
						break;
					}
				}

				if (found) {
					pkg_t *pkg = arr_get(&proj->pkgs, cfg_pkg->pkg);
					if (pkg->has_targets) {
						target_t *target;
						list_node_t j = pkg->targets;
						list_foreach(&proj->targets, j, target)
						{
							proj_add_dep(proj, j, dep_target_id);
						}
					}
				}
			}
		}
	}

	i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		if (pkg->has_targets) {
			target_t *target;
			list_node_t j = pkg->targets;
			list_foreach(&proj->targets, j, target)
			{
				if (target->type != TARGET_TYPE_TST) {
					continue;
				}

				const target_t *dep_target;
				list_node_t k = pkg->targets;
				list_foreach(&proj->targets, k, dep_target)
				{
					if (dep_target->type == TARGET_TYPE_LIB) {
						proj_add_dep(proj, j, k);
						break;
					}
				}
			}
		}
	}

	return ret;
}
