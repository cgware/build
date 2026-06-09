#include "mod.h"

#include "config_fs.h"
#include "mem.h"
#include "path.h"

static int mod_pkgs_config_fs(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry,
			      config_sync_plan_t *plan, fs_t *fs, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc,
			      dst_t dst)
{
	(void)mod;
	(void)config;
	(void)tmp;
	(void)schema;
	(void)registry;
	(void)plan;
	(void)name;
	(void)buf;
	(void)dst;

	path_t path = {0};
	path_init(&path, proj_path);
	path_push(&path, cur_path);
	size_t path_len = path.len;
	path_push(&path, STRV("pkgs"));
	size_t pkgs_path_len = path.len;

	int ret = 0;

	if (fs_isdir(fs, STRVS(path))) {
		strbuf_t pkgs = {0};
		strbuf_init(&pkgs, 8, 16, alloc);

		fs_lsdir(fs, STRVS(path), &pkgs);

		uint i = 0;
		strv_t subdir;
		strbuf_foreach(&pkgs, i, subdir)
		{
			path_push(&path, subdir);
			ret |= config_sync_plan_add_dir(plan, STRVN(&path.data[path_len], path.len - path_len), subdir);
			path.len = pkgs_path_len;
		}

		strbuf_free(&pkgs);
	}

	return ret;
}

static mod_t mod_pkgs = {
	.name	   = STRVT("mod_pkgs"),
	.config_fs = mod_pkgs_config_fs,
};

MOD(mod_pkgs, &mod_pkgs);
