#include "mod.h"

#include "path.h"

static int mod_test_config_fs(mod_t *mod, config_t *config, config_t *tmp, registry_t *registry, fs_t *fs, proc_t *proc, strv_t proj_path,
			      strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst)
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
	size_t path_len = path.len;
	path_push(&path, STRV("test"));

	if (fs_isdir(fs, STRVS(path))) {
		config_state_t state;
		config_get_state(tmp, &state);

		uint pkg;
		registry_add_pkg(registry, name, &pkg);
		config_pkg(tmp, pkg, CONFIG_MODE_EN);
		config_str(tmp, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_EN, cur_path);
		str_t tgt_name = strz(64);
		str_cat(&tgt_name, name);
		str_cat(&tgt_name, STRV("_test"));
		uint tgt;
		registry_add_tgt(registry, STRVS(tgt_name), &tgt);
		config_tgt(tmp, pkg, tgt, CONFIG_MODE_APP);
		str_free(&tgt_name);
		config_tgt_type(tmp, pkg, tgt, CONFIG_MODE_SET, TARGET_TYPE_TST);
		config_str(tmp, CONFIG_OP_TYPE_TGT_SRC, pkg, tgt, CONFIG_MODE_SET, STRV("test"));

		uint incs;
		config_str_list(tmp, CONFIG_OP_TYPE_TGT_INCS_PRIV, pkg, tgt, CONFIG_MODE_SET, STRV("test"), &incs);

		path.len = path_len;
		path_push(&path, STRV("src"));
		if (fs_isdir(fs, STRVS(path))) {
			config_str_list_add(tmp, incs, STRV("src"));
		}

		path.len = path_len;
		path_push(&path, STRV("drivers"));
		if (fs_isdir(fs, STRVS(path))) {
			config_str_list_add(tmp, incs, STRV("drivers"));
		}

		ret |= config_merge(config, tmp, state, registry);
		config_set_state(tmp, state);
	}

	return ret;
}

static int mod_test_proj_cfg(mod_t *mod, proj_t *proj)
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
				case TARGET_TYPE_TST: {
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

static int mod_test_init(mod_t *mod, uint cap, alloc_t alloc)
{
	(void)mod;
	(void)cap;
	(void)alloc;
	return 0;
}

static int mod_test_free(mod_t *mod)
{
	(void)mod;
	return 0;
}

static mod_t mod_test = {
	.name	   = STRVT("mod_test"),
	.init	   = mod_test_init,
	.free	   = mod_test_free,
	.config_fs = mod_test_config_fs,
	.proj_cfg  = mod_test_proj_cfg,
};

MOD(mod_test, &mod_test);
