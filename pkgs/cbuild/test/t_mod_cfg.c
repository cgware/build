#include "mod.h"

#include "file/cfg_prs.h"
#include "log.h"
#include "mem.h"
#include "mod_base.h"
#include "path.h"
#include "test.h"

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

int config_cfg(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, cfg_var_t root, fs_t *fs,
	       proc_t *proc, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst);

static mod_t *mod_cfg_init(config_schema_t *schema)
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (!strv_eq(mod->name, STRV("mod_cfg"))) {
			continue;
		}

		mod->init(mod, 2, schema, ALLOC_STD);
		return mod;
	}

	return NULL;
}

static void mod_cfg_free(mod_t *mod)
{
	mod->free(mod);
}

TESTP(config_cfg_empty, mod_t *mod, const config_schema_t *schema)
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
	uint ops_cap	  = config.vals.cap;
	config.vals.cap	  = 0;
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV("name"),
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	config.vals.cap	  = ops_cap;
	registry.pkgs.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_deps, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":deps = dep1, dep2\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_deps_oom, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	config.lists.cnt = 0;
	mem_oom(0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_deps_invalid, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_uri, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":uri = uri\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_uri_invalid, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_inc, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":inc = include\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_inc_invalid, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_app, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_app_invalid, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_en, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[512] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_set_not_found, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	char out[512] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_pkg_ops, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[512] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":deps = dep\n"
		   ":uri = uri\n"
		   ":inc = include\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_app, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":tgts += \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_app_invalid, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_en, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":tgts ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_set_not_found, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_ops, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":tgts += \n"
		   "::prep = prep\n"
		   "::conf = conf\n"
		   "::comp = comp\n"
		   "::inst = inst\n"
		   "::path = out\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_deps, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":tgts += \n"
		   "::deps = dep1, dep2\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_deps_oom, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	config.lists.cnt = 0;
	mem_oom(0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_tgt_deps_invalid, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_target_lib, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":tgts += \n"
		   "::name = lib\n"
		   "::type = 1\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_target_exe, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":tgts += \n"
		   "::name = exe\n"
		   "::type = 2\n");

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(config_cfg_ext, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     &fs,
			     &proc,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_STD()),
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

TESTP(config_cfg_ext_invalid, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(config_cfg(mod,
			     &config,
			     &tc,
			     schema,
			     &registry,
			     root,
			     NULL,
			     NULL,
			     STRV_NULL,
			     STRV_NULL,
			     STRV_NULL,
			     &buf,
			     ALLOC_STD,
			     DST_STD()),
		  1);
	log_set_quiet(0, 0);

	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(mod_cfg_config_fs, mod_t *mod, const config_schema_t *schema)
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

	EXPECT_EQ(mod->config_fs(
			  mod, &config, &tc, schema, &registry, &fs, &proc, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()),
		  0);

	char out[256] = {0};
	config_print(&config, schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n");

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(mod_cfg_config_fs_invalid, mod_t *mod, const config_schema_t *schema)
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
	EXPECT_EQ(mod->config_fs(
			  mod, &config, &tc, schema, &registry, &fs, &proc, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(mod_cfg_apply_val, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str(&config, mod->ops + CONFIG_PKG_URI, 0, 0, CONFIG_ACT_SET, STRV("https://host.com/file.zip"));

	log_set_quiet(0, 1);
	config_val_t val;
	val.op = schema->ops.cnt;
	EXPECT_EQ(mod->apply_val(mod, schema, &config, &val, &proj), 1);
	val.op	= mod->ops + CONFIG_PKG_URI;
	val.pkg = proj.pkgs.cnt;
	EXPECT_EQ(mod->apply_val(mod, schema, &config, &val, &proj), 1);
	val.op	= mod->ops + CONFIG_TGT_PREP;
	val.tgt = proj.targets.cnt;
	EXPECT_EQ(mod->apply_val(mod, schema, &config, &val, &proj), 1);
	val.op = CONFIG_PKGS;
	EXPECT_EQ(mod->apply_val(mod, schema, &config, &val, &proj), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_cfg_apply_val_pkg_uri, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str(&config, mod->ops + CONFIG_PKG_URI, 0, 0, CONFIG_ACT_SET, STRV("https://host.com/file.zip"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const pkg_t *pkg = proj_get_pkg(&proj, 0);

	strv_t str = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(str.data, "https://host.com/file.zip", str.len);
	str = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(str.data, "file.zip", str.len);
	str = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(str.data, "file", str.len);
	str = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(str.data, "", str.len);
	str = proj_get_str(&proj, pkg->strs + PKG_STR_URI_DIR);
	EXPECT_STRN(str.data, "file" SEP, str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_cfg_apply_val_pkg_inc, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);
	proj_add_target(&proj, 0, NULL);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str(&config, mod->ops + CONFIG_PKG_INC, 0, 0, CONFIG_ACT_SET, STRV("include"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t str = proj_get_str(&proj, tgt->strs + TGT_STR_INC);
	EXPECT_STRN(str.data, "include", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_cfg_apply_val_tgt_prep, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);
	proj_add_target(&proj, 0, NULL);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str(&config, mod->ops + CONFIG_TGT_PREP, 0, 0, CONFIG_ACT_SET, STRV("prep"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t str = proj_get_str(&proj, tgt->strs + TGT_STR_PREP);
	EXPECT_STRN(str.data, "prep", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_cfg_apply_val_tgt_conf, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);
	proj_add_target(&proj, 0, NULL);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str(&config, mod->ops + CONFIG_TGT_CONF, 0, 0, CONFIG_ACT_SET, STRV("conf"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t str = proj_get_str(&proj, tgt->strs + TGT_STR_CONF);
	EXPECT_STRN(str.data, "conf", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_cfg_apply_val_tgt_comp, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);
	proj_add_target(&proj, 0, NULL);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str(&config, mod->ops + CONFIG_TGT_COMP, 0, 0, CONFIG_ACT_SET, STRV("comp"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t str = proj_get_str(&proj, tgt->strs + TGT_STR_COMP);
	EXPECT_STRN(str.data, "comp", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_cfg_apply_val_tgt_inst, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);
	proj_add_target(&proj, 0, NULL);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str(&config, mod->ops + CONFIG_TGT_INST, 0, 0, CONFIG_ACT_SET, STRV("inst"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t str = proj_get_str(&proj, tgt->strs + TGT_STR_INST);
	EXPECT_STRN(str.data, "inst", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_cfg_apply_val_tgt_out_path, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);
	proj_add_target(&proj, 0, NULL);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str(&config, mod->ops + CONFIG_TGT_OUT_PATH, 0, 0, CONFIG_ACT_SET, STRV("out"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t str = proj_get_str(&proj, tgt->strs + TGT_STR_OUT);
	EXPECT_STRN(str.data, "out", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_cfg_apply_val_tgt_out_name, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);
	proj_add_target(&proj, 0, NULL);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str(&config, mod->ops + CONFIG_TGT_OUT_NAME, 0, 0, CONFIG_ACT_SET, STRV("name"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t str = proj_get_str(&proj, tgt->strs + TGT_STR_TGT);
	EXPECT_STRN(str.data, "name", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_cfg_apply_val_tgt_out_type, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);
	proj_add_target(&proj, 0, NULL);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_int(&config, mod->ops + CONFIG_TGT_OUT_TYPE, 0, 0, CONFIG_ACT_SET, TARGET_TGT_TYPE_LIB);

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const target_t *tgt = proj_get_target(&proj, 0);
	EXPECT_EQ(tgt->out_type, TARGET_TGT_TYPE_LIB);

	config_free(&config);
	proj_free(&proj);

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

	config_schema_t schema = {0};
	config_schema_init(&schema, 8, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mod_t *mod = mod_cfg_init(&schema);
	RUNP(config_cfg_empty, mod, &schema);
	RUNP(config_cfg_deps, mod, &schema);
	RUNP(config_cfg_deps_oom, mod, &schema);
	RUNP(config_cfg_deps_invalid, mod, &schema);
	RUNP(config_cfg_uri, mod, &schema);
	RUNP(config_cfg_uri_invalid, mod, &schema);
	RUNP(config_cfg_inc, mod, &schema);
	RUNP(config_cfg_inc_invalid, mod, &schema);
	RUNP(config_cfg_pkg_app, mod, &schema);
	RUNP(config_cfg_pkg_app_invalid, mod, &schema);
	RUNP(config_cfg_pkg_en, mod, &schema);
	RUNP(config_cfg_pkg_set_not_found, mod, &schema);
	RUNP(config_cfg_pkg_ops, mod, &schema);
	RUNP(config_cfg_tgt_app, mod, &schema);
	RUNP(config_cfg_tgt_app_invalid, mod, &schema);
	RUNP(config_cfg_tgt_en, mod, &schema);
	RUNP(config_cfg_tgt_set_not_found, mod, &schema);
	RUNP(config_cfg_tgt_ops, mod, &schema);
	RUNP(config_cfg_tgt_deps, mod, &schema);
	RUNP(config_cfg_tgt_deps_oom, mod, &schema);
	RUNP(config_cfg_tgt_deps_invalid, mod, &schema);
	RUNP(config_cfg_target_lib, mod, &schema);
	RUNP(config_cfg_target_exe, mod, &schema);
	RUNP(config_cfg_ext, mod, &schema);
	RUNP(config_cfg_ext_invalid, mod, &schema);
	RUNP(mod_cfg_config_fs, mod, &schema);
	RUNP(mod_cfg_config_fs_invalid, mod, &schema);
	RUNP(mod_cfg_apply_val, mod, &schema);
	RUNP(mod_cfg_apply_val_pkg_uri, mod, &schema);
	RUNP(mod_cfg_apply_val_pkg_inc, mod, &schema);
	RUNP(mod_cfg_apply_val_tgt_prep, mod, &schema);
	RUNP(mod_cfg_apply_val_tgt_conf, mod, &schema);
	RUNP(mod_cfg_apply_val_tgt_comp, mod, &schema);
	RUNP(mod_cfg_apply_val_tgt_inst, mod, &schema);
	RUNP(mod_cfg_apply_val_tgt_out_path, mod, &schema);
	RUNP(mod_cfg_apply_val_tgt_out_name, mod, &schema);
	RUNP(mod_cfg_apply_val_tgt_out_type, mod, &schema);
	RUNP(mod_cfg_proj_cfg, mod);
	RUNP(mod_cfg_proj_cfg_uri, mod);
	mod_cfg_free(mod);
	config_schema_free(&schema);

	SEND;
}
