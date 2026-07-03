#include "mod.h"

#include "mod_base.h"
#include "path.h"

static int mod_example_config_fs(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry,
				 config_sync_plan_t *plan, fs_t *fs, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf,
				 alloc_t alloc, dst_t dst)
{
	(void)mod;
	(void)plan;
	(void)alloc;
	(void)dst;

	int ret = 0;

	path_t path = {0};
	path_init(&path, proj_path);
	path_push(&path, cur_path);
	size_t path_len = path.len;
	path_push(&path, STRV("example"));

	if (!fs_isdir(fs, STRVS(path))) {
		return 0;
	}

	path_t example_path = {0};
	path_init(&example_path, cur_path);
	path_push(&example_path, STRV("example"));

	path_push(&path, STRV("src"));
	if (!fs_isdir(fs, STRVS(path))) {
		return 0;
	}

	path_push(&path, STRV("main.c"));
	if (!fs_isfile(fs, STRVS(path))) {
		return 0;
	}

	config_state_t state;
	config_get_state(tmp, &state);

	buf->len = 0;
	str_cat(buf, name);
	str_cat(buf, STRV("_example"));
	strv_t example_name = STRVS(*buf);

	uint pkg;
	registry_add_pkg(registry, example_name, &pkg);
	config_str_list(tmp, CONFIG_PKGS, pkg, -1, CONFIG_ACT_APP, example_name, NULL);
	config_str(tmp, CONFIG_PKG_PATH, pkg, -1, CONFIG_ACT_EN, STRVS(example_path));

	uint tgt;
	registry_add_tgt(registry, pkg, example_name, &tgt);
	config_str_list(tmp, CONFIG_TGTS, pkg, tgt, CONFIG_ACT_APP, example_name, NULL);
	config_int(tmp, CONFIG_TGT_TYPE, pkg, tgt, CONFIG_ACT_SET, TARGET_TYPE_EXE);
	config_str(tmp, CONFIG_TGT_SRC, pkg, tgt, CONFIG_ACT_SET, STRV("src"));
	config_str_list(tmp, CONFIG_TGT_INCS_PRIV, pkg, tgt, CONFIG_ACT_SET, STRV("src"), NULL);

	path.len = path_len;
	path_push(&path, STRV("src"));
	path_push(&path, STRV("main.c"));
	if (!fs_isfile(fs, STRVS(path))) {
		buf->len = 0;
		str_cat(buf, name);
		str_cat(buf, STRV(":"));
		str_cat(buf, name);
		config_str_list(tmp, CONFIG_TGT_DEPS, pkg, tgt, CONFIG_ACT_SET, STRVS(*buf), NULL);
	}

	path.len = path_len;
	path_push(&path, STRV("drivers"));
	if (fs_isdir(fs, STRVS(path))) {
		buf->len = 0;
		str_cat(buf, name);
		str_cat(buf, STRV(":drivers"));
		config_str_list(tmp, CONFIG_TGT_DEPS, pkg, tgt, CONFIG_ACT_APP, STRVS(*buf), NULL);
	}

	ret |= config_merge(config, tmp, state, schema, registry);
	config_set_state(tmp, state);

	return ret;
}

static mod_t mod_example = {
	.name	   = STRVT("mod_example"),
	.config_fs = mod_example_config_fs,
};

MOD(mod_example, &mod_example);
