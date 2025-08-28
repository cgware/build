#include "config.h"

#include "file/cfg_prs.h"
#include "log.h"
#include "path.h"

config_t *config_init(config_t *config, uint dirs_cap, uint pkgs_cap, uint targets_cap, alloc_t alloc)
{
	if (config == NULL) {
		return NULL;
	}

	if (strvbuf_init(&config->strs, 4 * dirs_cap + 1 * pkgs_cap + 1 * targets_cap, 16, alloc) == NULL ||
	    arr_init(&config->dirs, dirs_cap, sizeof(config_dir_t), alloc) == NULL ||
	    list_init(&config->pkgs, pkgs_cap, sizeof(config_pkg_t), alloc) == NULL ||
	    list_init(&config->targets, targets_cap, sizeof(config_target_t), alloc) == NULL ||
	    list_init(&config->deps, pkgs_cap, sizeof(size_t), alloc) == NULL) {
		return NULL;
	}

	return config;
}

void config_free(config_t *config)
{
	if (config == NULL) {
		return;
	}

	list_free(&config->deps);
	list_free(&config->targets);
	list_free(&config->pkgs);
	arr_free(&config->dirs);
	strvbuf_free(&config->strs);
}

int config_load(config_t *config, fs_t *fs, proc_t *proc, strv_t base_path, strv_t dir_path, strv_t name, str_t *buf, alloc_t alloc,
		dst_t dst)
{
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
	dir->has_pkgs	  = 0;

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
				list_node_t pkg_id;
				config_pkg_t *pkg = list_node(&config->pkgs, &pkg_id);
				strvbuf_add(&config->strs, STRV_NULL, &pkg->name);
				pkg->has_targets = 0;
				pkg->has_deps	 = 0;

				if (dir->has_pkgs) {
					list_app(&config->pkgs, dir->pkgs, pkg_id);
				} else {
					dir->pkgs     = pkg_id;
					dir->has_pkgs = 1;
				}

				cfg_var_t var;
				if (cfg_has_var(&cfg, tbl, STRV("deps"), &var)) {
					cfg_var_t dep_str;
					void *data;
					cfg_foreach(&cfg, var, data, &dep_str)
					{
						strv_t val = {0};
						if (cfg_get_lit(&cfg, dep_str, &val)) {
							log_error("cbuild", "proj_cfg", NULL, "invalid dependency");
							ret = 1;
							continue;
						}

						list_node_t dep_id;
						size_t *dep = list_node(&config->deps, &dep_id);

						if (pkg->has_deps) {
							list_app(&config->deps, pkg->deps, dep_id);
						} else {
							pkg->deps     = dep_id;
							pkg->has_deps = 1;
						}

						strvbuf_add(&config->strs, val, dep);
					}
				}
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

	uint i = 0;
	const config_dir_t *dir;
	arr_foreach(&config->dirs, i, dir)
	{
		strv_t dir_name = strvbuf_get(&config->strs, dir->name);
		strv_t dir_path = strvbuf_get(&config->strs, dir->path);
		strv_t dir_src	= strvbuf_get(&config->strs, dir->src);
		strv_t dir_inc	= strvbuf_get(&config->strs, dir->inc);

		dst.off += dputf(dst, "[dir]\n");

		dst.off += dputf(dst,
				 "NAME: %.*s\n"
				 "PATH: %.*s\n"
				 "SRC: %.*s\n"
				 "INC: %.*s\n",
				 dir_name.len,
				 dir_name.data,
				 dir_path.len,
				 dir_path.data,
				 dir_src.len,
				 dir_src.data,
				 dir_inc.len,
				 dir_inc.data);

		dst.off += dputf(dst, "\n");

		if (dir->has_pkgs) {
			const config_pkg_t *pkg;
			estx_node_t pkgs = dir->pkgs;
			list_foreach(&config->pkgs, pkgs, pkg)
			{
				dst.off += dputf(dst, "[pkg]\n");
				strv_t pkg_name = strvbuf_get(&config->strs, pkg->name);
				dst.off += dputf(dst, "NAME: %.*s\n", pkg_name.len, pkg_name.data);
				dst.off += dputf(dst, "DEPS:");
				const size_t *dep;
				estx_node_t deps = pkg->deps;
				list_foreach(&config->deps, deps, dep)
				{
					strv_t dep_str = strvbuf_get(&config->strs, *dep);
					dst.off += dputf(dst, " %.*s", dep_str.len, dep_str.data);
				}

				dst.off += dputf(dst, "\n\n");
			}
		}
	}

	return dst.off - off;
}
