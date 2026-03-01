#include "mod.h"

#include "path.h"

static int mod_lib_config_fs(mod_t *mod, config_t *config, config_t *tmp, registry_t *registry, fs_t *fs, proc_t *proc, strv_t proj_path,
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
	path_push(&path, STRV("src"));

	if (fs_isdir(fs, STRVS(path))) {
		path_push(&path, STRV("main.c"));
		if (!fs_isfile(fs, STRVS(path))) {
			config_state_t state;
			config_get_state(tmp, &state);

			uint pkg;
			registry_add_pkg(registry, name, &pkg);
			config_pkg(tmp, pkg, CONFIG_MODE_EN);
			config_str(tmp, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_EN, cur_path);
			uint tgt;
			registry_add_tgt(registry, pkg, name, &tgt);
			config_tgt(tmp, pkg, tgt, CONFIG_MODE_APP);
			config_tgt_type(tmp, pkg, tgt, CONFIG_MODE_SET, TARGET_TYPE_LIB);
			config_str(tmp, CONFIG_OP_TYPE_TGT_SRC, pkg, tgt, CONFIG_MODE_SET, STRV("src"));
			config_str(tmp, CONFIG_OP_TYPE_TGT_INC, pkg, tgt, CONFIG_MODE_SET, STRV("include"));
			config_str_list(tmp, CONFIG_OP_TYPE_TGT_INCS_PRIV, pkg, tgt, CONFIG_MODE_SET, STRV("src"), NULL);

			ret |= config_merge(config, tmp, state, registry);
			config_set_state(tmp, state);
		}
	}

	return ret;
}

static int mod_lib_proj_cfg(mod_t *mod, proj_t *proj)
{
	(void)mod;
	(void)proj;
	return 0;
}

static int mod_lib_init(mod_t *mod, uint cap, alloc_t alloc)
{
	(void)mod;
	(void)cap;
	(void)alloc;
	return 0;
}

static int mod_lib_free(mod_t *mod)
{
	(void)mod;
	return 0;
}

static mod_t mod_lib = {
	.name	   = STRVT("mod_lib"),
	.init	   = mod_lib_init,
	.free	   = mod_lib_free,
	.config_fs = mod_lib_config_fs,
	.proj_cfg  = mod_lib_proj_cfg,
};

MOD(mod_lib, &mod_lib);
