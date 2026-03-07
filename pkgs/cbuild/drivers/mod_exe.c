#include "mod.h"

#include "mod_base.h"
#include "path.h"

static int mod_exe_config_fs(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, fs_t *fs,
			     proc_t *proc, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst)
{
	(void)mod;
	(void)proc;
	(void)buf;
	(void)alloc;
	(void)dst;

	int ret = 0;

	path_t path = {0};
	path_init(&path, proj_path);
	path_push(&path, cur_path);
	path_push(&path, STRV("src"));

	if (fs_isdir(fs, STRVS(path))) {
		path_push(&path, STRV("main.c"));
		if (fs_isfile(fs, STRVS(path))) {
			config_state_t state;
			config_get_state(tmp, &state);

			uint pkg;
			registry_add_pkg(registry, name, &pkg);
			config_str_list(tmp, CONFIG_PKGS, pkg, -1, CONFIG_ACT_EN, name, NULL);
			config_str(tmp, CONFIG_PKG_PATH, pkg, -1, CONFIG_ACT_EN, cur_path);
			uint tgt;
			registry_add_tgt(registry, pkg, name, &tgt);
			config_str_list(tmp, CONFIG_TGTS, pkg, tgt, CONFIG_ACT_APP, name, NULL);
			config_int(tmp, CONFIG_TGT_TYPE, pkg, tgt, CONFIG_ACT_SET, TARGET_TYPE_EXE);
			config_str(tmp, CONFIG_TGT_SRC, pkg, tgt, CONFIG_ACT_SET, STRV("src"));

			uint incs = -1;
			config_str_list(tmp, CONFIG_TGT_INCS_PRIV, pkg, tgt, CONFIG_ACT_SET, STRV("src"), &incs);

			ret |= config_merge(config, tmp, state, schema, registry);
			config_set_state(tmp, state);
		}
	}

	return ret;
}

static int mod_exe_proj_cfg(mod_t *mod, proj_t *proj)
{
	(void)mod;

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		if (pkg->has_targets) {
			target_t *target;
			list_node_t j = pkg->targets;
			list_foreach(&proj->targets, j, target)
			{
				switch (target->type) {
				case TARGET_TYPE_EXE: {
					const target_t *dep_target;
					list_node_t k = pkg->targets;
					list_foreach(&proj->targets, k, dep_target)
					{
						if (dep_target->type == TARGET_TYPE_LIB || dep_target->type == TARGET_TYPE_DRV) {
							proj_add_dep(proj, j, k);
						}
					}
					break;
				}
				default:
					break;
				}
			}
		}
	}

	return 0;
}

static mod_t mod_exe = {
	.name	   = STRVT("mod_exe"),
	.config_fs = mod_exe_config_fs,
	.proj_cfg  = mod_exe_proj_cfg,
};

MOD(mod_exe, &mod_exe);
