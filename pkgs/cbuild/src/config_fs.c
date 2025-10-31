#include "config_fs.h"

#include "config_cfg.h"
#include "file/cfg_prs.h"
#include "log.h"
#include "path.h"

config_dir_t *config_fs(config_t *config, fs_t *fs, proc_t *proc, strv_t base_path, strv_t dir_path, strv_t name, str_t *buf, alloc_t alloc,
			dst_t dst)
{
	if (config == NULL) {
		return NULL;
	}

	path_t full_path = {0};
	path_init(&full_path, base_path);
	path_push(&full_path, dir_path);
	size_t dir_path_len = full_path.len;

	log_info("cbuild", "config", NULL, "loading directory: '%.*s'", full_path.len, full_path.data);

	uint dir_id;
	config_dir_t *dir = config_add_dir(config, &dir_id);

	config_set_str(config, dir->strs + CONFIG_DIR_NAME, name);
	config_set_str(config, dir->strs + CONFIG_DIR_PATH, dir_path);

	path_push(&full_path, STRV("src"));
	config_set_str(config, dir->strs + CONFIG_DIR_SRC, fs_isdir(fs, STRVS(full_path)) ? STRV("src") : STRV_NULL);
	if (fs_isdir(fs, STRVS(full_path))) {
		path_push(&full_path, STRV("main.c"));
		dir->has_main = fs_isfile(fs, STRVS(full_path));
	}
	full_path.len = dir_path_len;

	path_push(&full_path, STRV("include"));
	config_set_str(config, dir->strs + CONFIG_DIR_INC, fs_isdir(fs, STRVS(full_path)) ? STRV("include") : STRV_NULL);
	full_path.len = dir_path_len;

	path_push(&full_path, STRV("drivers"));
	config_set_str(config, dir->strs + CONFIG_DIR_DRV, fs_isdir(fs, STRVS(full_path)) ? STRV("drivers") : STRV_NULL);
	full_path.len = dir_path_len;

	path_push(&full_path, STRV("test"));
	config_set_str(config, dir->strs + CONFIG_DIR_TST, fs_isdir(fs, STRVS(full_path)) ? STRV("test") : STRV_NULL);
	full_path.len = dir_path_len;

	path_push(&full_path, STRV("pkgs"));
	if (fs_isdir(fs, STRVS(full_path))) {
		strbuf_t pkgs = {0};
		strbuf_init(&pkgs, 8, 16, alloc);

		fs_lsdir(fs, STRVS(full_path), &pkgs);

		size_t pkgs_path_len = full_path.len;

		uint i = 0;
		strv_t subdir;
		strbuf_foreach(&pkgs, i, subdir)
		{
			path_push(&full_path, subdir);
			config_fs(config,
				  fs,
				  proc,
				  base_path,
				  STRVN(&full_path.data[dir_path_len], full_path.len - dir_path_len),
				  subdir,
				  buf,
				  alloc,
				  dst);
			full_path.len = pkgs_path_len;
		}

		strbuf_free(&pkgs);
	}
	full_path.len = dir_path_len;

	path_push(&full_path, STRV("build.cfg"));
	if (fs_isfile(fs, STRVS(full_path))) {
		cfg_t cfg      = {0};
		cfg_var_t root = -1;
		cfg_init(&cfg, 4, 4, alloc);

		cfg_prs_t prs = {0};
		cfg_prs_init(&prs, alloc);
		fs_read(fs, STRVS(full_path), 0, buf);
		cfg_prs_parse(&prs, STRVS(*buf), &cfg, alloc, &root, dst);
		cfg_prs_free(&prs);

		config_cfg(config, &cfg, root, fs, proc, base_path, dir_id, buf, alloc, dst);

		cfg_free(&cfg);
	}

	full_path.len = dir_path_len;

	return dir;
}
