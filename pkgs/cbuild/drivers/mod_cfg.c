#include "mod.h"

#include "config_fs.h"
#include "file/cfg_prs.h"
#include "log.h"
#include "mem.h"
#include "path.h"

typedef struct mod_cfg_priv_s {
	cfg_prs_t prs;
	cfg_t cfg;
} mod_cfg_priv_t;

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

static int parse_pkg_deps(config_t *config, uint pkg, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	cfg_var_t dep_str;
	int first = 1;
	uint deps;
	void *data;
	cfg_foreach(cfg, var, data, &dep_str)
	{
		strv_t val = {0};
		if (cfg_get_lit(cfg, dep_str, &val)) {
			log_error("cbuild", "mod_cfg", NULL, "invalid dependency");
			ret = 1;
			continue;
		}

		if (first) {
			if (config_str_list(config, CONFIG_OP_TYPE_PKG_DEPS, pkg, -1, CONFIG_MODE_SET, val, &deps)) {
				log_error("cbuild", "mod_cfg", NULL, "failed to set package dependency: %.*s", val.len, val.data);
				ret = 1;
			}
			first = 0;
		} else {
			if (config_str_list_add(config, deps, val)) {
				log_error("cbuild", "mod_cfg", NULL, "failed to add package dependency: %.*s", val.len, val.data);
				ret = 1;
			}
		}
	}
	return ret;
}

static int parse_tgt_deps(config_t *config, uint pkg, uint tgt, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	cfg_var_t dep_str;
	int first = 1;
	uint deps;
	void *data;
	cfg_foreach(cfg, var, data, &dep_str)
	{
		strv_t val = {0};
		if (cfg_get_lit(cfg, dep_str, &val)) {
			log_error("cbuild", "mod_cfg", NULL, "invalid dependency");
			ret = 1;
			continue;
		}

		if (first) {
			if (config_str_list(config, CONFIG_OP_TYPE_TGT_DEPS, pkg, tgt, CONFIG_MODE_SET, val, &deps)) {
				log_error("cbuild", "mod_cfg", NULL, "failed to set target dependency: %.*s", val.len, val.data);
				ret = 1;
			}
			first = 0;
		} else {
			if (config_str_list_add(config, deps, val)) {
				log_error("cbuild", "mod_cfg", NULL, "failed to add target dependency: %.*s", val.len, val.data);
				ret = 1;
			}
		}
	}
	return ret;
}

static int parse_cfg_ext(config_t *config, config_t *tmp, registry_t *registry, const cfg_t *cfg, cfg_var_t root, fs_t *fs, proc_t *proc,
			 strv_t proj_path, str_t *buf, alloc_t alloc, dst_t dst)
{
	int ret = 0;

	void *data;
	cfg_var_t tbl;
	cfg_foreach(cfg, root, data, &tbl)
	{
		strv_t key = cfg_get_key(cfg, tbl);

		if (strv_eq(key, STRV("ext"))) {
			void *data;
			cfg_var_t ext;
			cfg_foreach(cfg, tbl, data, &ext)
			{
				strv_t uri;
				if (cfg_get_str(cfg, ext, &uri)) {
					log_error("cbuild", "mod_cfg", NULL, "invalid extern format");
					ret = 1;
					continue;
				}

				strv_t file = {0};
				strv_t name = {0};
				strv_rsplit(uri, '/', NULL, &file);
				strv_lsplit(file, '.', &name, NULL);

				create_tmp(fs, proj_path);

				path_t dir = {0};
				path_init(&dir, STRV("tmp"));
				path_push(&dir, STRV("ext"));
				path_push(&dir, name);
				path_push(&dir, STRV(""));

				path_t path = {0};
				path_init(&path, proj_path);
				path_push(&path, STRVS(dir));
				size_t path_len = path.len;

				if (!fs_isdir(fs, STRVS(path))) {
					fs_mkpath(fs, proj_path, STRVS(dir));
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
							log_info("cbuild", "mod_cfg", NULL, "cloning package: %.*s", uri.len, uri.data);
							proc_cmd(proc, STRVS(*buf));
						}
					}
				}

				ret |= config_fs(config, tmp, registry, fs, proc, proj_path, STRVS(dir), name, buf, alloc, dst);
			}
		}
	}

	return ret;
}

static int parse_pkg_uri(config_t *config, uint pkg, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	strv_t val;
	if (cfg_get_str(cfg, var, &val)) {
		log_error("cbuild", "config_cfg", NULL, "invalid uri");
		return 1;
	}
	config_str(config, CONFIG_OP_TYPE_PKG_URI, pkg, -1, CONFIG_MODE_SET, val);
	return ret;
}

static int parse_pkg_inc(config_t *config, uint pkg, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	strv_t val;
	if (cfg_get_str(cfg, var, &val)) {
		log_error("cbuild", "config_cfg", NULL, "invalid include");
		return 1;
	}
	config_str(config, CONFIG_OP_TYPE_PKG_INC, pkg, -1, CONFIG_MODE_SET, val);
	return ret;
}

static int parse_cfg_com(config_t *config, registry_t *registry, uint pkg, const cfg_t *cfg, cfg_var_t root)
{
	int ret = 0;

	cfg_var_data_t *data;
	cfg_var_t tbl;
	cfg_foreach(cfg, root, data, &tbl)
	{
		strv_t key = cfg_get_key(cfg, tbl);

		if (data->type == CFG_VAR_TBL) {
			strv_t type, filters;
			strv_lsplit(key, ':', &type, &filters);
			config_mode_t mode = CONFIG_MODE_SET;
			if (type.len > 0 && type.data[0] == '+') {
				mode = CONFIG_MODE_APP;
				type = STRVN(&type.data[1], type.len - 1);
			} else if (type.len > 0 && type.data[0] == '?') {
				mode = CONFIG_MODE_EN;
				type = STRVN(&type.data[1], type.len - 1);
			}

			if (strv_eq(type, STRV("pkg"))) {
				if (filters.data == NULL) {
					log_error("cbuild", "mod_cfg", NULL, "unknown package");
					ret = 1;
					break;
				}

				strv_t filter, name;
				strv_lsplit(filters, '=', &filter, &name);
				if (name.data == NULL) {
					name = filter;
				}

				if (mode == CONFIG_MODE_SET) {
					if (registry_find_pkg(registry, name, &pkg)) {
						log_error("cbuild", "mod_cfg", NULL, "package does not exist: %.*s", name.len, name.data);
						ret = 1;
						continue;
					}
				} else {
					registry_add_pkg(registry, name, &pkg);
					config_pkg(config, pkg, mode);
				}

				cfg_var_t var;
				if (cfg_has_var(cfg, tbl, STRV("deps"), &var)) {
					ret |= parse_pkg_deps(config, pkg, cfg, var);
				}
				if (cfg_has_var(cfg, tbl, STRV("uri"), &var)) {
					ret |= parse_pkg_uri(config, pkg, cfg, var);
				}
				if (cfg_has_var(cfg, tbl, STRV("include"), &var)) {
					ret |= parse_pkg_inc(config, pkg, cfg, var);
				}
			} else if (strv_eq(type, STRV("tgt"))) {
				if (filters.data == NULL) {
					log_error("cbuild", "mod_cfg", NULL, "unknown target");
					ret = 1;
					break;
				}

				strv_t filter, name;
				strv_lsplit(filters, '=', &filter, &name);
				if (name.data == NULL) {
					name = filter;
				}

				uint tgt;
				if (mode == CONFIG_MODE_SET) {
					if (registry_find_tgt(registry, name, &tgt)) {
						log_error("cbuild", "mod_cfg", NULL, "target does not exist: %.*s", name.len, name.data);
						ret = 1;
						continue;
					}
				} else {
					registry_add_tgt(registry, name, &tgt);
					config_tgt(config, pkg, tgt, mode);
				}

				cfg_var_t var;
				strv_t val;

				if (cfg_has_var(cfg, tbl, STRV("prep"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, CONFIG_OP_TYPE_TGT_PREP, pkg, tgt, CONFIG_MODE_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("conf"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, CONFIG_OP_TYPE_TGT_CONF, pkg, tgt, CONFIG_MODE_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("comp"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, CONFIG_OP_TYPE_TGT_COMP, pkg, tgt, CONFIG_MODE_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("inst"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, CONFIG_OP_TYPE_TGT_INST, pkg, tgt, CONFIG_MODE_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("deps"), &var)) {
					ret |= parse_tgt_deps(config, pkg, tgt, cfg, var);
				}

				if (cfg_has_var(cfg, tbl, STRV("out"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, CONFIG_OP_TYPE_TGT_OUT, pkg, tgt, CONFIG_MODE_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("lib"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, CONFIG_OP_TYPE_TGT_LIB, pkg, tgt, CONFIG_MODE_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("exe"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, CONFIG_OP_TYPE_TGT_EXE, pkg, tgt, CONFIG_MODE_SET, val);
				}
			}
		} else {
			if (strv_eq(key, STRV("deps"))) {
				ret |= parse_pkg_deps(config, pkg, cfg, tbl);
			} else if (strv_eq(key, STRV("uri"))) {
				ret |= parse_pkg_uri(config, pkg, cfg, tbl);
			} else if (strv_eq(key, STRV("include"))) {
				ret |= parse_pkg_inc(config, pkg, cfg, tbl);
			}
		}
	}

	return ret;
}

int config_cfg(mod_t *mod, config_t *config, config_t *tmp, registry_t *registry, cfg_var_t root, fs_t *fs, proc_t *proc, strv_t proj_path,
	       strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst)
{
	(void)mod;

	if (config == NULL) {
		return 1;
	}

	int ret = 0;

	mod_cfg_priv_t *priv = mod->priv;

	ret |= parse_cfg_ext(config, tmp, registry, &priv->cfg, root, fs, proc, proj_path, buf, alloc, dst);

	uint pkg;
	if (registry_add_pkg(registry, name, &pkg)) {
		return 1;
	}

	config_state_t state;
	config_get_state(tmp, &state);
	ret |= config_pkg(tmp, pkg, CONFIG_MODE_EN);
	ret |= config_str(tmp, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_EN, cur_path);
	ret |= parse_cfg_com(tmp, registry, pkg, &priv->cfg, root);
	tmp->prio = config->prio + 1;
	ret |= config_merge(config, tmp, state, registry);
	tmp->prio = config->prio;
	config_set_state(tmp, state);

	return ret;
}

static int mod_cfg_config_fs(mod_t *mod, config_t *config, config_t *tmp, registry_t *registry, fs_t *fs, proc_t *proc, strv_t proj_path,
			     strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst)
{
	int ret = 0;

	path_t path = {0};
	path_init(&path, proj_path);
	path_push(&path, cur_path);
	path_push(&path, STRV("build.cfg"));

	if (fs_isfile(fs, STRVS(path))) {
		mod_cfg_priv_t *priv = mod->priv;
		cfg_var_t root	     = -1;
		uint vars_cnt	     = priv->cfg.vars.cnt;
		size_t strs_cnt	     = priv->cfg.strs.used;
		fs_read(fs, STRVS(path), 0, buf);
		ret |= cfg_prs_parse(&priv->prs, STRVS(*buf), &priv->cfg, &root, dst);
		ret |= config_cfg(mod, config, tmp, registry, root, fs, proc, proj_path, cur_path, name, buf, alloc, dst);
		priv->cfg.vars.cnt  = vars_cnt;
		priv->cfg.strs.used = strs_cnt;
	}

	return ret;
}

static int mod_cfg_proj_cfg(mod_t *mod, proj_t *proj)
{
	(void)mod;

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		strv_t uri = proj_get_str(proj, pkg->strs + PKG_STR_URI);
		if (uri.len > 0) {
			if (pkg->has_targets) {
				target_t *target;
				list_node_t j = pkg->targets;
				list_foreach(&proj->targets, j, target)
				{
					target->type = TARGET_TYPE_EXT;
				}
			}
		}
	}

	return 0;
}

static int mod_cfg_init(mod_t *mod, uint cap, alloc_t alloc)
{
	mod_cfg_priv_t *priv = mod->priv = mem_alloc(sizeof(mod_cfg_priv_t));
	cfg_prs_init(&priv->prs, alloc);
	cfg_init(&priv->cfg, cap * 4, cap * 4, alloc);
	return 0;
}

static int mod_cfg_free(mod_t *mod)
{
	mod_cfg_priv_t *priv = mod->priv;
	cfg_free(&priv->cfg);
	cfg_prs_free(&priv->prs);
	mem_free(priv, sizeof(mod_cfg_priv_t));
	return 0;
}

static mod_t mod_cfg = {
	.name	   = STRVT("mod_cfg"),
	.init	   = mod_cfg_init,
	.free	   = mod_cfg_free,
	.config_fs = mod_cfg_config_fs,
	.proj_cfg  = mod_cfg_proj_cfg,
};

MOD(mod_cfg, &mod_cfg);
