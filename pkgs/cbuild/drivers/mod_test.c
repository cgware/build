#include "mod.h"

#include "path.h"

int config_fs(const mod_driver_t *drv, fs_t *fs, config_t *config, config_dir_t *dir, strv_t path)
{
	(void)drv;
	path_t full_path = {0};
	path_init(&full_path, path);
	path_push(&full_path, STRV("test"));

	config_set_str(config, dir->strs + CONFIG_DIR_TST, fs_isdir(fs, STRVS(full_path)) ? STRV("test") : STRV_NULL);

	return 0;
}

int proj_cfg(const mod_driver_t *drv, fs_t *fs, config_t *config, config_dir_t *dir, strv_t path)
{
	int ret = 0;
	uint i;

	i = 0;
	const config_dir_t *dir;
	arr_foreach(&config->dirs, i, dir)
	{
		strv_t test = config_get_str(config, dir->strs + CONFIG_DIR_TST);

		if (test.len > 0) {
			list_node_t target_id;
			target = proj_add_target(proj, pkg_id, &target_id);
			proj_set_str(proj, target->strs + TARGET_NAME, name);
			strbuf_app(&proj->strs, target->strs + TARGET_NAME, STRV("_test"));
			target->type = TARGET_TYPE_TST;
		}
	}

	return 0;
}

static mod_driver_t test = {
	.config_fs = config_fs,
	.proj_cfg  = proj_cfg,
};

MOD_DRIVER(test, &test);
