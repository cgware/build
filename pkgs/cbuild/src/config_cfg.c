#include "config_cfg.h"

#include "config_fs.h"
#include "file/cfg_prs.h"
#include "log.h"
#include "path.h"

static int create_tmp(fs_t *fs, strv_t dir)
{
	path_t path = {0};
	path_init(&path, dir);

	path_push(&path, STRV("tmp"));
	if (!fs_isdir(fs, STRVS(path))) {
		fs_mkdir(fs, STRVS(path));
	}

	path_push(&path, STRV(".gitignore"));
	if (!fs_isfile(fs, STRVS(path))) {
		void *f;
		fs_open(fs, STRVS(path), "w", &f);
		fs_write(fs, f, STRV("*"));
		fs_close(fs, f);
	}

	return 0;
}

static int config_uri(config_t *config, config_pkg_t *pkg, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	strv_t val;
	if (cfg_get_str(cfg, var, &val)) {
		log_error("cbuild", "config_cfg", NULL, "invalid uri");
		return 1;
	}
	config_set_str(config, pkg->strs + CONFIG_PKG_URI, val);
	return ret;
}

static int config_inc(config_t *config, config_pkg_t *pkg, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	strv_t val;
	if (cfg_get_str(cfg, var, &val)) {
		log_error("cbuild", "config_cfg", NULL, "invalid include");
		return 1;
	}
	config_set_str(config, pkg->strs + CONFIG_PKG_INC, val);
	return ret;
}

static int config_pkg_deps(config_t *config, list_node_t pkg, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	cfg_var_t dep_str;
	void *data;
	cfg_foreach(cfg, var, data, &dep_str)
	{
		strv_t val = {0};
		if (cfg_get_lit(cfg, dep_str, &val)) {
			log_error("cbuild", "config_cfg", NULL, "invalid dependency");
			ret = 1;
			continue;
		}

		config_pkg_add_dep(config, pkg, val);
	}
	return ret;
}

static int config_tgt_deps(config_t *config, list_node_t tgt, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	cfg_var_t dep_str;
	void *data;
	cfg_foreach(cfg, var, data, &dep_str)
	{
		strv_t val = {0};
		if (cfg_get_lit(cfg, dep_str, &val)) {
			log_error("cbuild", "config_cfg", NULL, "invalid dependency");
			ret = 1;
			continue;
		}

		config_tgt_add_dep(config, tgt, val);
	}
	return ret;
}

int config_cfg(config_t *config, cfg_t *cfg, cfg_var_t root, fs_t *fs, proc_t *proc, strv_t base_path, uint dir, str_t *buf, alloc_t alloc,
	       dst_t dst)
{
	if (config == NULL) {
		return 1;
	}

	int ret = 0;

	list_node_t pkg_id;
	config_pkg_t *pkg	= NULL;
	config_target_t *target = NULL;

	void *data;
	cfg_var_t tbl;
	cfg_foreach(cfg, root, data, &tbl)
	{
		strv_t key = cfg_get_key(cfg, tbl);
		if (strv_eq(key, STRV("deps"))) {
			if (pkg == NULL) {
				pkg = config_add_pkg(config, dir, &pkg_id);
			}

			ret |= config_pkg_deps(config, pkg_id, cfg, tbl);
		} else if (strv_eq(key, STRV("uri"))) {
			if (pkg == NULL) {
				pkg = config_add_pkg(config, dir, &pkg_id);
			}

			ret |= config_uri(config, pkg, cfg, tbl);
		} else if (strv_eq(key, STRV("include"))) {
			if (pkg == NULL) {
				pkg = config_add_pkg(config, dir, &pkg_id);
			}

			ret |= config_inc(config, pkg, cfg, tbl);
		} else if (strv_eq(key, STRV("pkg"))) {
			pkg = config_add_pkg(config, dir, &pkg_id);

			cfg_var_t var;
			if (cfg_has_var(cfg, tbl, STRV("deps"), &var)) {
				ret |= config_pkg_deps(config, pkg_id, cfg, var);
			}
			if (cfg_has_var(cfg, tbl, STRV("uri"), &var)) {
				ret |= config_uri(config, pkg, cfg, var);
			}
			if (cfg_has_var(cfg, tbl, STRV("include"), &var)) {
				ret |= config_inc(config, pkg, cfg, var);
			}
		} else if (strv_eq(key, STRV("target"))) {
			if (pkg == NULL) {
				pkg = config_add_pkg(config, dir, &pkg_id);
			}

			list_node_t tgt_id;
			target = config_add_target(config, pkg_id, &tgt_id);

			cfg_var_t var;
			strv_t val;
			if (cfg_has_var(cfg, tbl, STRV("name"), &var)) {
				cfg_get_str(cfg, var, &val);
				config_set_str(config, target->strs + CONFIG_TARGET_NAME, val);
			}

			if (cfg_has_var(cfg, tbl, STRV("prep"), &var)) {
				cfg_get_str(cfg, var, &val);
				config_set_str(config, target->strs + CONFIG_TARGET_PREP, val);
			}

			if (cfg_has_var(cfg, tbl, STRV("conf"), &var)) {
				cfg_get_str(cfg, var, &val);
				config_set_str(config, target->strs + CONFIG_TARGET_CONF, val);
			}

			if (cfg_has_var(cfg, tbl, STRV("comp"), &var)) {
				cfg_get_str(cfg, var, &val);
				config_set_str(config, target->strs + CONFIG_TARGET_COMP, val);
			}

			if (cfg_has_var(cfg, tbl, STRV("inst"), &var)) {
				cfg_get_str(cfg, var, &val);
				config_set_str(config, target->strs + CONFIG_TARGET_INST, val);
			}

			if (cfg_has_var(cfg, tbl, STRV("deps"), &var)) {
				ret |= config_tgt_deps(config, tgt_id, cfg, var);
			}

			if (cfg_has_var(cfg, tbl, STRV("out"), &var)) {
				cfg_get_str(cfg, var, &val);
				config_set_str(config, target->strs + CONFIG_TARGET_OUT, val);
			}

			if (cfg_has_var(cfg, tbl, STRV("lib"), &var)) {
				cfg_get_str(cfg, var, &val);
				config_set_str(config, target->strs + CONFIG_TARGET_TGT, val);
				target->out_type = CONFIG_TARGET_TGT_TYPE_LIB;
			}

			if (cfg_has_var(cfg, tbl, STRV("exe"), &var)) {
				cfg_get_str(cfg, var, &val);
				config_set_str(config, target->strs + CONFIG_TARGET_TGT, val);
				target->out_type = CONFIG_TARGET_TGT_TYPE_EXE;
			}
		} else if (strv_eq(key, STRV("ext"))) {
			void *data;
			cfg_var_t ext;
			cfg_foreach(cfg, tbl, data, &ext)
			{
				strv_t uri;
				if (cfg_get_str(cfg, ext, &uri)) {
					log_error("cbuild", "pkg_loader", NULL, "invalid extern format");
					ret = 1;
					continue;
				}

				strv_t file = {0};
				strv_t name = {0};
				strv_rsplit(uri, '/', NULL, &file);
				strv_lsplit(file, '.', &name, NULL);

				create_tmp(fs, base_path);

				path_t dir = {0};
				path_init(&dir, STRV("tmp"));
				path_push(&dir, STRV("ext"));
				path_push(&dir, name);
				path_push(&dir, STRV(""));

				path_t path = {0};
				path_init(&path, base_path);
				path_push(&path, STRVS(dir));
				size_t path_len = path.len;

				if (!fs_isdir(fs, STRVS(path))) {
					fs_mkpath(fs, base_path, STRVS(dir));
				}

				path_push(&path, STRV(".git"));
				if (!fs_isdir(fs, STRVS(path))) {
					path.len = path_len;
					if (buf) {
						buf->len = 0;
						str_cat(buf, STRV("git clone "));
						str_cat(buf, uri);
						str_cat(buf, STRV(" "));
						str_cat(buf, STRVS(path));
						if (proc) {
							log_info("cbuild", "proj_fs", NULL, "cloning package: %.*s", uri.len, uri.data);
							proc_cmd(proc, STRVS(*buf));
						}
					}
				}

				if (config_fs(config, fs, proc, base_path, STRVS(dir), name, buf, alloc, dst) == NULL) {
					ret = 1;
				}
			}
		}
	}

	return ret;
}
