#include "mod.h"

#include "file/cfg_prs.h"
#include "log.h"
#include "mem.h"
#include "path.h"
#include "test.h"

typedef struct mod_cfg_priv_s {
	cfg_prs_t prs;
	cfg_t cfg;
} mod_cfg_priv_t;

int config_cfg(mod_t *mod, config_t *config, config_t *tmp, registry_t *registry, cfg_var_t root, fs_t *fs, proc_t *proc, strv_t proj_path,
	       strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst);

static mod_t *mod_cfg_init()
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (!strv_eq(mod->name, STRV("mod_cfg"))) {
			continue;
		}

		mod->init(mod, 2, ALLOC_STD);
		return mod;
	}

	return NULL;
}

static void mod_cfg_free(mod_t *mod)
{
	mod->free(mod);
}

TESTP(config_cfg_empty, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	cfg_var_t root;
	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;
	cfg_root(&priv->cfg, &root);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(NULL,
			     NULL,
			     NULL,
			     NULL,
			     priv->cfg.vars.cnt,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     NULL,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	mem_oom(1);

	registry.pkgs.cnt = registry.pkgs.cap;
	uint ops_cap	  = config.ops.cap;
	config.ops.cap	  = 0;
	EXPECT_EQ(
		config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV("name"), &buf, ALLOC_STD, DST_NONE()),
		1);
	config.ops.cap	  = ops_cap;
	registry.pkgs.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_deps, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, deps, var;
	cfg_root(&priv->cfg, &root);
	cfg_arr(&priv->cfg, STRV("deps"), CFG_MODE_SET, 0, &deps);
	cfg_add_var(&priv->cfg, root, deps);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_UNKNOWN, STRV("dep1"), &var);
	cfg_add_var(&priv->cfg, deps, var);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_UNKNOWN, STRV("dep2"), &var);
	cfg_add_var(&priv->cfg, deps, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "::deps = dep1, dep2\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_deps_oom, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, deps, var;
	cfg_root(&priv->cfg, &root);
	cfg_arr(&priv->cfg, STRV("deps"), CFG_MODE_SET, 0, &deps);
	cfg_add_var(&priv->cfg, root, deps);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_UNKNOWN, STRV("dep1"), &var);
	cfg_add_var(&priv->cfg, deps, var);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_UNKNOWN, STRV("dep2"), &var);
	cfg_add_var(&priv->cfg, deps, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	config.lists.cnt = config.lists.size;
	mem_oom(1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	config.lists.cnt = 0;
	mem_oom(0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_deps_invalid, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, deps, var;
	cfg_root(&priv->cfg, &root);
	cfg_arr(&priv->cfg, STRV("deps"), CFG_MODE_SET, 0, &deps);
	cfg_add_var(&priv->cfg, root, deps);
	cfg_str(&priv->cfg, STRV_NULL, CFG_MODE_UNKNOWN, STRV("dep"), &var);
	cfg_add_var(&priv->cfg, deps, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_uri, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, var;
	cfg_root(&priv->cfg, &root);
	cfg_str(&priv->cfg, STRV("uri"), CFG_MODE_SET, STRV("uri"), &var);
	cfg_add_var(&priv->cfg, root, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "::uri = uri\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_uri_invalid, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, var;
	cfg_root(&priv->cfg, &root);
	cfg_lit(&priv->cfg, STRV("uri"), CFG_MODE_SET, STRV("uri"), &var);
	cfg_add_var(&priv->cfg, root, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_inc, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, var;
	cfg_root(&priv->cfg, &root);
	cfg_str(&priv->cfg, STRV("include"), CFG_MODE_SET, STRV("include"), &var);
	cfg_add_var(&priv->cfg, root, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "::inc = include\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_inc_invalid, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, var;
	cfg_root(&priv->cfg, &root);
	cfg_lit(&priv->cfg, STRV("include"), CFG_MODE_SET, STRV("include"), &var);
	cfg_add_var(&priv->cfg, root, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_app, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+pkg:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   "::path ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_app_invalid, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+pkg"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);

	char tmp[256] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_en, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("?pkg:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);

	char tmp[256] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[512] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_set_not_found, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("pkg:pkg"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);

	char tmp[256] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	char out[512] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_ops, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, deps, var;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("?pkg:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_tbl(&priv->cfg, STRV("deps"), &deps);
	cfg_add_var(&priv->cfg, tbl, deps);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("dep"), &var);
	cfg_add_var(&priv->cfg, deps, var);
	cfg_str(&priv->cfg, STRV("uri"), CFG_MODE_SET, STRV("uri"), &var);
	cfg_add_var(&priv->cfg, tbl, var);
	cfg_str(&priv->cfg, STRV("include"), CFG_MODE_SET, STRV("include"), &var);
	cfg_add_var(&priv->cfg, tbl, var);

	char tmp[256] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[512] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "::deps = dep\n"
		   "::uri = uri\n"
		   "::inc = include\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_app, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+tgt:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "\n"
		   ":tgts += \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_app_invalid, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+tgt"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_en, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("?tgt:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "\n"
		   ":tgts ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_set_not_found, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("tgt:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_ops, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, var;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+tgt:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_str(&priv->cfg, STRV("prep"), CFG_MODE_SET, STRV("prep"), &var);
	cfg_add_var(&priv->cfg, tbl, var);
	cfg_str(&priv->cfg, STRV("conf"), CFG_MODE_SET, STRV("conf"), &var);
	cfg_add_var(&priv->cfg, tbl, var);
	cfg_str(&priv->cfg, STRV("comp"), CFG_MODE_SET, STRV("comp"), &var);
	cfg_add_var(&priv->cfg, tbl, var);
	cfg_str(&priv->cfg, STRV("inst"), CFG_MODE_SET, STRV("inst"), &var);
	cfg_add_var(&priv->cfg, tbl, var);
	cfg_str(&priv->cfg, STRV("out"), CFG_MODE_SET, STRV("out"), &var);
	cfg_add_var(&priv->cfg, tbl, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "\n"
		   ":tgts += \n"
		   "::prep = prep\n"
		   "::conf = conf\n"
		   "::comp = comp\n"
		   "::inst = inst\n"
		   "::out = out\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_deps, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	registry_add_tgt(&registry, 0, STRV(""), NULL);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, deps, var;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+tgt:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_arr(&priv->cfg, STRV("deps"), CFG_MODE_SET, 0, &deps);
	cfg_add_var(&priv->cfg, tbl, deps);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("dep1"), &var);
	cfg_add_var(&priv->cfg, deps, var);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("dep2"), &var);
	cfg_add_var(&priv->cfg, deps, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "\n"
		   ":tgts += \n"
		   "::deps = dep1, dep2\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_deps_oom, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	registry_add_tgt(&registry, 0, STRV(""), NULL);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, deps, var;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+tgt:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_arr(&priv->cfg, STRV("deps"), CFG_MODE_SET, 0, &deps);
	cfg_add_var(&priv->cfg, tbl, deps);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("dep1"), &var);
	cfg_add_var(&priv->cfg, deps, var);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("dep2"), &var);
	cfg_add_var(&priv->cfg, deps, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	mem_oom(1);
	config.lists.cnt = config.lists.cap;
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	config.lists.cnt = 0;
	mem_oom(0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_deps_invalid, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, deps, var;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+tgt:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_arr(&priv->cfg, STRV("deps"), CFG_MODE_SET, 0, &deps);
	cfg_add_var(&priv->cfg, tbl, deps);
	cfg_str(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("dep"), &var);
	cfg_add_var(&priv->cfg, deps, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_target_lib, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, var;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+tgt:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_str(&priv->cfg, STRV("lib"), CFG_MODE_SET, STRV("lib"), &var);
	cfg_add_var(&priv->cfg, tbl, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "\n"
		   ":tgts += \n"
		   "::lib = lib\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_target_exe, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, var;
	cfg_root(&priv->cfg, &root);
	cfg_tbl(&priv->cfg, STRV("+tgt:"), &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_str(&priv->cfg, STRV("exe"), CFG_MODE_SET, STRV("exe"), &var);
	cfg_add_var(&priv->cfg, tbl, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "\n"
		   ":tgts += \n"
		   "::exe = exe\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_ext, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, var;
	cfg_root(&priv->cfg, &root);
	cfg_arr(&priv->cfg, STRV("ext"), CFG_MODE_SET, 1, &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_str(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("repo"), &var);
	cfg_add_var(&priv->cfg, tbl, var);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	proc_t proc = {0};
	proc_init(&proc, 32, 1);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, &fs, &proc, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()),
		  0);

	EXPECT_EQ(fs_isfile(&fs, STRV("tmp/.gitignore")), 1);
	EXPECT_EQ(fs_isdir(&fs, STRV("tmp/ext/repo")), 1);
	EXPECT_STRN(proc.buf.data, "git clone repo tmp" SEP "ext" SEP "repo" SEP "\n", proc.buf.len);

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_ext_invalid, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, var;
	cfg_root(&priv->cfg, &root);
	cfg_arr(&priv->cfg, STRV("ext"), CFG_MODE_SET, 1, &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_lit(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("repo"), &var);
	cfg_add_var(&priv->cfg, tbl, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(mod, &config, &tc, &registry, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(mod_cfg_config_fs, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, var;
	cfg_root(&priv->cfg, &root);
	cfg_arr(&priv->cfg, STRV("ext"), CFG_MODE_SET, 1, &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_str(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("repo"), &var);
	cfg_add_var(&priv->cfg, tbl, var);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	void *f;
	fs_open(&fs, STRV("build.cfg"), "w", &f);
	fs_write(&fs, f, STRV("[+pkg:]\n"));
	fs_close(&fs, f);

	proc_t proc = {0};
	proc_init(&proc, 32, 1);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(mod->config_fs(mod, &config, &tc, &registry, &fs, &proc, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()), 0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   "::path ?= \n");

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(mod_cfg_config_fs_invalid, mod_t *mod)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	mod_cfg_priv_t *priv = mod->priv;

	priv->cfg.strs.used = 0;
	priv->cfg.vars.cnt  = 0;

	cfg_var_t root, tbl, var;
	cfg_root(&priv->cfg, &root);
	cfg_arr(&priv->cfg, STRV("ext"), CFG_MODE_SET, 1, &tbl);
	cfg_add_var(&priv->cfg, root, tbl);
	cfg_str(&priv->cfg, STRV_NULL, CFG_MODE_SET, STRV("repo"), &var);
	cfg_add_var(&priv->cfg, tbl, var);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	void *f;
	fs_open(&fs, STRV("build.cfg"), "w", &f);
	fs_write(&fs,
		 f,
		 STRV("ext:\n"
		      "uri = invalid\n"
		      "\n"));
	fs_close(&fs, f);

	proc_t proc = {0};
	proc_init(&proc, 32, 1);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(mod->config_fs(mod, &config, &tc, &registry, &fs, &proc, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(mod_cfg_proj_cfg, mod_t *mod)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	EXPECT_EQ(mod->proj_cfg(mod, &proj), 0);

	proj_free(&proj);

	END;
}

TESTP(mod_cfg_proj_cfg_uri, mod_t *mod)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	pkg_t *pkg    = proj_add_pkg(&proj, &pkg_id);
	target_t *tgt = proj_add_target(&proj, pkg_id, NULL);

	proj_set_str(&proj, pkg->strs + PKG_STR_URI, STRV("uri"));

	EXPECT_EQ(mod->proj_cfg(mod, &proj), 0);

	EXPECT_EQ(tgt->type, TARGET_TYPE_EXT);

	proj_free(&proj);

	END;
}

STEST(mod_cfg)
{
	SSTART;

	mod_t *mod = mod_cfg_init();
	RUNP(config_cfg_empty, mod);
	RUNP(config_cfg_deps, mod);
	RUNP(config_cfg_deps_oom, mod);
	RUNP(config_cfg_deps_invalid, mod);
	RUNP(config_cfg_uri, mod);
	RUNP(config_cfg_uri_invalid, mod);
	RUNP(config_cfg_inc, mod);
	RUNP(config_cfg_inc_invalid, mod);
	RUNP(config_cfg_pkg_app, mod);
	RUNP(config_cfg_pkg_app_invalid, mod);
	RUNP(config_cfg_pkg_en, mod);
	RUNP(config_cfg_pkg_set_not_found, mod);
	RUNP(config_cfg_pkg_ops, mod);
	RUNP(config_cfg_tgt_app, mod);
	RUNP(config_cfg_tgt_app_invalid, mod);
	RUNP(config_cfg_tgt_en, mod);
	RUNP(config_cfg_tgt_set_not_found, mod);
	RUNP(config_cfg_tgt_ops, mod);
	RUNP(config_cfg_tgt_deps, mod);
	RUNP(config_cfg_tgt_deps_oom, mod);
	RUNP(config_cfg_tgt_deps_invalid, mod);
	RUNP(config_cfg_target_lib, mod);
	RUNP(config_cfg_target_exe, mod);
	RUNP(config_cfg_ext, mod);
	RUNP(config_cfg_ext_invalid, mod);
	RUNP(mod_cfg_config_fs, mod);
	RUNP(mod_cfg_config_fs_invalid, mod);
	RUNP(mod_cfg_proj_cfg, mod);
	RUNP(mod_cfg_proj_cfg_uri, mod);
	mod_cfg_free(mod);
	SEND;
}
