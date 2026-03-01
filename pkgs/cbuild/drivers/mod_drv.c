#include "mod.h"

#include "path.h"

static int mod_drv_config_fs(mod_t *mod, config_t *config, config_t *tmp, registry_t *registry, fs_t *fs, proc_t *proc, strv_t proj_path,
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
	path_push(&path, STRV("drivers"));

	if (fs_isdir(fs, STRVS(path))) {
		config_state_t state;
		config_get_state(tmp, &state);

		uint pkg;
		registry_add_pkg(registry, name, &pkg);
		config_pkg(tmp, pkg, CONFIG_MODE_EN);
		config_str(tmp, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_EN, cur_path);
		str_t tgt_name = strz(64);
		str_cat(&tgt_name, name);
		str_cat(&tgt_name, STRV("_drv"));
		uint tgt;
		registry_add_tgt(registry, STRVS(tgt_name), &tgt);
		str_free(&tgt_name);
		config_tgt(tmp, pkg, tgt, CONFIG_MODE_APP);
		config_tgt_type(tmp, pkg, tgt, CONFIG_MODE_SET, TARGET_TYPE_DRV);
		config_str(tmp, CONFIG_OP_TYPE_TGT_SRC, pkg, tgt, CONFIG_MODE_SET, STRV("drivers"));
		config_str_list(tmp, CONFIG_OP_TYPE_TGT_INCS_PRIV, pkg, tgt, CONFIG_MODE_SET, STRV("drivers"), NULL);

		path.len = path_len;
		path_push(&path, STRV("include"));
		if (fs_isdir(fs, STRVS(path))) {
			config_str(tmp, CONFIG_OP_TYPE_TGT_INC, pkg, tgt, CONFIG_MODE_SET, STRV("include"));
		}

		ret |= config_merge(config, tmp, state, registry);
		config_set_state(tmp, state);
	}

	return ret;
}

static int mod_drv_proj_cfg(mod_t *mod, proj_t *proj)
{
	(void)mod;
	(void)proj;

	return 0;
}

static int mod_drv_init(mod_t *mod, uint cap, alloc_t alloc)
{
	(void)mod;
	(void)cap;
	(void)alloc;
	return 0;
}

static int mod_drv_free(mod_t *mod)
{
	(void)mod;
	return 0;
}

static mod_t mod_drv = {
	.name	   = STRVT("mod_drv"),
	.init	   = mod_drv_init,
	.free	   = mod_drv_free,
	.config_fs = mod_drv_config_fs,
	.proj_cfg  = mod_drv_proj_cfg,
};

MOD(mod_drv, &mod_drv);
