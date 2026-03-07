#include "mod.h"

#include "config_fs.h"
#include "file/cfg_prs.h"
#include "log.h"
#include "mem.h"
#include "mod_base.h"
#include "path.h"
#include "proj_utils.h"

typedef struct mod_cfg_priv_s {
	cfg_prs_t prs;
	cfg_t cfg;
} mod_cfg_priv_t;

enum {
	CONFIG_PKG_URI,
	CONFIG_PKG_INC,
	CONFIG_TGT_PREP,
	CONFIG_TGT_CONF,
	CONFIG_TGT_COMP,
	CONFIG_TGT_INST,
	CONFIG_TGT_OUT_PATH,
	CONFIG_TGT_OUT_NAME,
	CONFIG_TGT_OUT_TYPE,
};

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
	uint deps = -1;
	void *data;
	cfg_foreach(cfg, var, data, &dep_str)
	{
		strv_t val = {0};
		if (cfg_get_lit(cfg, dep_str, &val)) {
			log_error("cbuild", "mod_cfg", NULL, "invalid dependency");
			ret = 1;
			continue;
		}

		if (config_str_list(config, CONFIG_PKG_DEPS, pkg, -1, CONFIG_ACT_SET, val, &deps)) {
			log_error("cbuild", "mod_cfg", NULL, "failed to set package dependency: %.*s", val.len, val.data);
			ret = 1;
		}
	}
	return ret;
}

static int parse_tgt_deps(config_t *config, uint pkg, uint tgt, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	cfg_var_t dep_str;
	uint deps = -1;
	void *data;
	cfg_foreach(cfg, var, data, &dep_str)
	{
		strv_t val = {0};
		if (cfg_get_lit(cfg, dep_str, &val)) {
			log_error("cbuild", "mod_cfg", NULL, "invalid dependency");
			ret = 1;
			continue;
		}

		if (config_str_list(config, CONFIG_TGT_DEPS, pkg, tgt, CONFIG_ACT_SET, val, &deps)) {
			log_error("cbuild", "mod_cfg", NULL, "failed to set target dependency: %.*s", val.len, val.data);
			ret = 1;
		}
	}
	return ret;
}

static int parse_cfg_ext(config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, const cfg_t *cfg,
			 cfg_var_t root, fs_t *fs, proc_t *proc, strv_t proj_path, str_t *buf, alloc_t alloc, dst_t dst)
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

				ret |= config_fs(config, tmp, schema, registry, fs, proc, proj_path, STRVS(dir), name, buf, alloc, dst);
			}
		}
	}

	return ret;
}

static int parse_pkg_uri(mod_t *mod, config_t *config, uint pkg, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	strv_t val;
	if (cfg_get_str(cfg, var, &val)) {
		log_error("cbuild", "config_cfg", NULL, "invalid uri");
		return 1;
	}
	config_str(config, mod->ops + CONFIG_PKG_URI, pkg, -1, CONFIG_ACT_SET, val);
	return ret;
}

static int parse_pkg_inc(mod_t *mod, config_t *config, uint pkg, const cfg_t *cfg, cfg_var_t var)
{
	int ret = 0;
	strv_t val;
	if (cfg_get_str(cfg, var, &val)) {
		log_error("cbuild", "config_cfg", NULL, "invalid include");
		return 1;
	}
	config_str(config, mod->ops + CONFIG_PKG_INC, pkg, -1, CONFIG_ACT_SET, val);
	return ret;
}

static int parse_cfg_com(mod_t *mod, config_t *config, registry_t *registry, uint pkg, const cfg_t *cfg, cfg_var_t root)
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
			config_act_t act = CONFIG_ACT_SET;
			if (type.len > 0 && type.data[0] == '+') {
				act  = CONFIG_ACT_APP;
				type = STRVN(&type.data[1], type.len - 1);
			} else if (type.len > 0 && type.data[0] == '?') {
				act  = CONFIG_ACT_EN;
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

				if (act == CONFIG_ACT_SET) {
					if (registry_find_pkg(registry, name, &pkg)) {
						log_error("cbuild", "mod_cfg", NULL, "package does not exist: %.*s", name.len, name.data);
						ret = 1;
						continue;
					}
				} else {
					registry_add_pkg(registry, name, &pkg);
					config_str_list(config, CONFIG_PKGS, pkg, -1, act, name, NULL);
				}

				cfg_var_t var;
				if (cfg_has_var(cfg, tbl, STRV("deps"), &var)) {
					ret |= parse_pkg_deps(config, pkg, cfg, var);
				}
				if (cfg_has_var(cfg, tbl, STRV("uri"), &var)) {
					ret |= parse_pkg_uri(mod, config, pkg, cfg, var);
				}
				if (cfg_has_var(cfg, tbl, STRV("include"), &var)) {
					ret |= parse_pkg_inc(mod, config, pkg, cfg, var);
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
				if (act == CONFIG_ACT_SET) {
					if (registry_find_tgt(registry, pkg, name, &tgt)) {
						log_error("cbuild", "mod_cfg", NULL, "target does not exist: %.*s", name.len, name.data);
						ret = 1;
						continue;
					}
				} else {
					registry_add_tgt(registry, pkg, name, &tgt);
					config_str_list(config, CONFIG_TGTS, pkg, tgt, act, name, NULL);
				}

				cfg_var_t var;
				strv_t val;

				if (cfg_has_var(cfg, tbl, STRV("prep"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, mod->ops + CONFIG_TGT_PREP, pkg, tgt, CONFIG_ACT_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("conf"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, mod->ops + CONFIG_TGT_CONF, pkg, tgt, CONFIG_ACT_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("comp"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, mod->ops + CONFIG_TGT_COMP, pkg, tgt, CONFIG_ACT_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("inst"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, mod->ops + CONFIG_TGT_INST, pkg, tgt, CONFIG_ACT_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("deps"), &var)) {
					ret |= parse_tgt_deps(config, pkg, tgt, cfg, var);
				}

				if (cfg_has_var(cfg, tbl, STRV("out"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, mod->ops + CONFIG_TGT_OUT_PATH, pkg, tgt, CONFIG_ACT_SET, val);
				}

				if (cfg_has_var(cfg, tbl, STRV("lib"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, mod->ops + CONFIG_TGT_OUT_NAME, pkg, tgt, CONFIG_ACT_SET, val);
					config_int(config, mod->ops + CONFIG_TGT_OUT_TYPE, pkg, tgt, CONFIG_ACT_SET, TARGET_TGT_TYPE_LIB);
				}

				if (cfg_has_var(cfg, tbl, STRV("exe"), &var)) {
					cfg_get_str(cfg, var, &val);
					config_str(config, mod->ops + CONFIG_TGT_OUT_NAME, pkg, tgt, CONFIG_ACT_SET, val);
					config_int(config, mod->ops + CONFIG_TGT_OUT_TYPE, pkg, tgt, CONFIG_ACT_SET, TARGET_TGT_TYPE_EXE);
				}
			}
		} else {
			if (strv_eq(key, STRV("deps"))) {
				ret |= parse_pkg_deps(config, pkg, cfg, tbl);
			} else if (strv_eq(key, STRV("uri"))) {
				ret |= parse_pkg_uri(mod, config, pkg, cfg, tbl);
			} else if (strv_eq(key, STRV("include"))) {
				ret |= parse_pkg_inc(mod, config, pkg, cfg, tbl);
			}
		}
	}

	return ret;
}

int config_cfg(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, cfg_var_t root, fs_t *fs,
	       proc_t *proc, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst)
{
	if (config == NULL) {
		return 1;
	}

	int ret = 0;

	mod_cfg_priv_t *priv = mod->priv;

	ret |= parse_cfg_ext(config, tmp, schema, registry, &priv->cfg, root, fs, proc, proj_path, buf, alloc, dst);

	uint pkg;
	if (registry_add_pkg(registry, name, &pkg)) {
		return 1;
	}

	config_state_t state;
	config_get_state(tmp, &state);
	ret |= config_str_list(tmp, CONFIG_PKGS, pkg, -1, CONFIG_ACT_EN, name, NULL);
	ret |= config_str(tmp, CONFIG_PKG_PATH, pkg, -1, CONFIG_ACT_EN, cur_path);
	ret |= parse_cfg_com(mod, tmp, registry, pkg, &priv->cfg, root);
	tmp->prio = config->prio + 1;
	ret |= config_merge(config, tmp, state, schema, registry);
	tmp->prio = config->prio;
	config_set_state(tmp, state);

	return ret;
}

static int mod_cfg_config_fs(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, fs_t *fs,
			     proc_t *proc, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst)
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
		ret |= config_cfg(mod, config, tmp, schema, registry, root, fs, proc, proj_path, cur_path, name, buf, alloc, dst);
		priv->cfg.vars.cnt  = vars_cnt;
		priv->cfg.strs.used = strs_cnt;
	}

	return ret;
}

static int mod_cfg_apply_val(mod_t *mod, const config_schema_t *schema, const config_t *config, const config_val_t *v, proj_t *proj)

{
	config_schema_op_t *op = config_schema_get_op(schema, v->op);
	if (op == NULL) {
		log_error("cbuild", "config", NULL, "invalid op: %d", v->op);
		return 1;
	}

	pkg_t *pkg    = NULL;
	target_t *tgt = NULL;

	switch (op->scope) {
	case CONFIG_SCOPE_PKG: {
		pkg = proj_get_pkg(proj, v->pkg);
		if (pkg == NULL) {
			log_error("cbuild", "proj_cfg", NULL, "package does not exist: %d", v->pkg);
			return 1;
		}
		break;
	}
	case CONFIG_SCOPE_TGT: {
		tgt = proj_get_target(proj, v->tgt);
		if (tgt == NULL) {
			log_error("cbuild", "proj_cfg", NULL, "target does not exist: %d", v->tgt);
			return 1;
		}
		break;
	}
	default: {
		log_error("cbuild", "mod_cfg", NULL, "invalid op scope: %d", op->scope);
		return 1;
	}
	}

	switch (v->op - mod->ops) {
	case CONFIG_PKG_URI: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_uri(proj, pkg, val);
		break;
	}
	case CONFIG_PKG_INC: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		if (pkg->has_targets) {
			target_t *target;
			list_node_t k = pkg->targets;
			list_foreach(&proj->targets, k, target)
			{
				proj_set_str(proj, target->strs + TGT_STR_INC, val);
			}
		}
		break;
	}
	case CONFIG_TGT_PREP: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_str(proj, tgt->strs + TGT_STR_PREP, val);
		break;
	}
	case CONFIG_TGT_CONF: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_str(proj, tgt->strs + TGT_STR_CONF, val);
		break;
	}
	case CONFIG_TGT_COMP: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_str(proj, tgt->strs + TGT_STR_COMP, val);
		break;
	}
	case CONFIG_TGT_INST: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_str(proj, tgt->strs + TGT_STR_INST, val);
		break;
	}
	case CONFIG_TGT_OUT_PATH: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_str(proj, tgt->strs + TGT_STR_OUT, val);
		break;
	}
	case CONFIG_TGT_OUT_NAME: {
		strv_t val = strvbuf_get(&config->strs, v->args.s);
		proj_set_str(proj, tgt->strs + TGT_STR_TGT, val);
		break;
	}
	case CONFIG_TGT_OUT_TYPE: {
		tgt->out_type = v->args.i;
		break;
	}
	}

	return 0;
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

static int mod_cfg_init(mod_t *mod, uint cap, config_schema_t *config_schema, alloc_t alloc)
{
	mod_cfg_priv_t *priv = mod->priv = mem_alloc(sizeof(mod_cfg_priv_t));
	cfg_prs_init(&priv->prs, alloc);
	cfg_init(&priv->cfg, cap * 4, cap * 4, alloc);

	mod->ops = config_schema->ops.cnt;

	config_schema_op_desc_t ops[] = {
		{.name = STRV("uri"), .scope = CONFIG_SCOPE_PKG, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("inc"), .scope = CONFIG_SCOPE_PKG, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("prep"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("conf"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("comp"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("inst"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("path"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("name"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_STR, .priv = mod},
		{.name = STRV("type"), .scope = CONFIG_SCOPE_TGT, .type = CONFIG_TYPE_INT, .priv = mod},

	};

	config_schema_add_ops(config_schema, ops, sizeof(ops));

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
	.apply_val = mod_cfg_apply_val,
	.proj_cfg  = mod_cfg_proj_cfg,
};

MOD(mod_cfg, &mod_cfg);
