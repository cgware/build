#include "config.h"

#include "file/cfg_prs.h"
#include "log.h"
#include "path.h"

config_t *config_init(config_t *config, uint dirs_cap, uint pkgs_cap, alloc_t alloc)
{
	if (config == NULL) {
		return NULL;
	}

	if (strvbuf_init(&config->strs, 4 * dirs_cap, 16, alloc) == NULL ||
	    arr_init(&config->dirs, dirs_cap, sizeof(config_dir_t), alloc) == NULL ||
	    arr_init(&config->pkgs, pkgs_cap, sizeof(config_pkg_t), alloc) == NULL) {
		return NULL;
	}

	return config;
}

void config_free(config_t *config)
{
	arr_free(&config->pkgs);
	arr_free(&config->dirs);
	strvbuf_free(&config->strs);
}

int config_load(config_t *config, fs_t *fs, proc_t *proc, strv_t base_path, strv_t dir_path, strv_t name, str_t *buf, alloc_t alloc,
		dst_t dst)
{
	(void)proc;
	if (config == NULL) {
		return 1;
	}

	int ret = 0;

	path_t full_path = {0};
	path_init(&full_path, base_path);
	path_push(&full_path, dir_path);
	size_t dir_path_len = full_path.len;

	log_info("cbuild", "config", NULL, "loading directory: '%.*s'", full_path.len, full_path.data);

	uint dir_id;
	config_dir_t *dir = arr_add(&config->dirs, &dir_id);

	strvbuf_add(&config->strs, name, &dir->name);
	strvbuf_add(&config->strs, dir_path, &dir->path);

	path_push(&full_path, STRV("src"));
	strvbuf_add(&config->strs, fs_isdir(fs, STRVS(full_path)) ? STRV("src") : STRV_NULL, &dir->src);
	full_path.len = dir_path_len;

	path_push(&full_path, STRV("include"));
	strvbuf_add(&config->strs, fs_isdir(fs, STRVS(full_path)) ? STRV("include") : STRV_NULL, &dir->inc);
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
			ret |= config_load(config,
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

		void *data;
		cfg_var_t tbl;
		cfg_foreach(&cfg, root, data, &tbl)
		{
			strv_t key = cfg_get_key(&cfg, tbl);
			if (strv_eq(key, STRV("pkg"))) {
				config_pkg_t *pkg = arr_add(&config->pkgs, NULL);

				pkg->dir = dir_id;
			}
		}

		cfg_free(&cfg);
	}
	full_path.len = dir_path_len;

	return ret;
}

size_t config_print(const config_t *config, dst_t dst)
{
	if (config == NULL) {
		return 0;
	}

	size_t off = dst.off;

	uint i;

	dst.off += dputf(dst, "[dirs]\n");

	i = 0;
	const config_dir_t *dir;
	arr_foreach(&config->dirs, i, dir)
	{
		strv_t name = strvbuf_get(&config->strs, dir->name);
		strv_t path = strvbuf_get(&config->strs, dir->path);
		strv_t src  = strvbuf_get(&config->strs, dir->src);
		strv_t inc  = strvbuf_get(&config->strs, dir->inc);

		dst.off += dputf(dst,
				 "NAME: %.*s\n"
				 "PATH: %.*s\n"
				 "SRC: %.*s\n"
				 "INC: %.*s\n\n",
				 name.len,
				 name.data,
				 path.len,
				 path.data,
				 src.len,
				 src.data,
				 inc.len,
				 inc.data);
	}

	dst.off += dputf(dst, "[pkgs]\n");

	i = 0;
	const config_pkg_t *pkg;
	arr_foreach(&config->pkgs, i, pkg)
	{
		dir	    = arr_get(&config->dirs, pkg->dir);
		strv_t path = strvbuf_get(&config->strs, dir->path);
		dst.off += dputf(dst, "PATH: %.*s\n\n", path.len, path.data);
	}

	return dst.off - off;
}
