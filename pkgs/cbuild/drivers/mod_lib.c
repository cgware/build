#include "mod.h"

#include "mod_base.h"
#include "path.h"

static int mod_lib_config_fs(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, fs_t *fs,
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
		if (!fs_isfile(fs, STRVS(path))) {
			config_state_t state;
			config_get_state(tmp, &state);

			uint pkg;
			registry_add_pkg(registry, name, &pkg);
			config_str_list(tmp, CONFIG_PKGS, pkg, -1, CONFIG_ACT_EN, name, NULL);
			config_str(tmp, CONFIG_PKG_PATH, pkg, -1, CONFIG_ACT_EN, cur_path);
			uint tgt;
			registry_add_tgt(registry, pkg, name, &tgt);
			config_str_list(tmp, CONFIG_TGTS, pkg, tgt, CONFIG_ACT_APP, name, NULL);
			config_int(tmp, CONFIG_TGT_TYPE, pkg, tgt, CONFIG_ACT_SET, TARGET_TYPE_LIB);
			config_str(tmp, CONFIG_TGT_SRC, pkg, tgt, CONFIG_ACT_SET, STRV("src"));
			config_str(tmp, CONFIG_TGT_INC, pkg, tgt, CONFIG_ACT_SET, STRV("include"));
			config_str_list(tmp, CONFIG_TGT_INCS_PRIV, pkg, tgt, CONFIG_ACT_SET, STRV("src"), NULL);

			ret |= config_merge(config, tmp, state, schema, registry);
			config_set_state(tmp, state);
		}
	}

	return ret;
}

static mod_t mod_lib = {
	.name	   = STRVT("mod_lib"),
	.config_fs = mod_lib_config_fs,
};

MOD(mod_lib, &mod_lib);
