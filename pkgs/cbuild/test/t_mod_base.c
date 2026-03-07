#include "mod.h"

#include "log.h"
#include "mem.h"
#include "mod_base.h"
#include "test.h"

TESTP(mod_base_apply_val, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str(&config, CONFIG_PKG_PATH, 0, 0, CONFIG_ACT_SET, STRV("path"));

	log_set_quiet(0, 1);
	config_val_t val;
	val.op = schema->ops.cnt;
	EXPECT_EQ(mod->apply_val(mod, schema, &config, &val, &proj), 1);
	val.op	= mod->ops + CONFIG_PKG_PATH;
	val.pkg = proj.pkgs.cnt;
	EXPECT_EQ(mod->apply_val(mod, schema, &config, &val, &proj), 1);
	val.op	= mod->ops + CONFIG_TGT_TYPE;
	val.tgt = proj.targets.cnt;
	EXPECT_EQ(mod->apply_val(mod, schema, &config, &val, &proj), 1);
	val.op = CONFIG_PKGS;
	EXPECT_EQ(mod->apply_val(mod, schema, &config, &val, &proj), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_pkg_path, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	proj_add_pkg(&proj, NULL);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str(&config, CONFIG_PKG_PATH, 0, 0, CONFIG_ACT_SET, STRV("path"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	const pkg_t *pkg = proj_get_pkg(&proj, 0);

	strv_t str = proj_get_str(&proj, pkg->strs + PKG_STR_PATH);
	EXPECT_STRN(str.data, "path", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_pkg_dep_pkg, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("dep"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("dep_tgt"));
	pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	tgt = proj_add_target(&proj, 1, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 4, ALLOC_STD);
	config_str_list(&config, CONFIG_PKG_DEPS, 1, 0, CONFIG_ACT_SET, STRV("dep"), NULL);

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_pkg_dep_tgt, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("dep"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("dep_tgt"));
	pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	tgt = proj_add_target(&proj, 1, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 4, ALLOC_STD);
	config_str_list(&config, CONFIG_PKG_DEPS, 1, -1, CONFIG_ACT_SET, STRV("dep:dep_tgt"), NULL);

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_pkg_dep_not_test, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("dep"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("test"));
	tgt->type = TARGET_TYPE_TST;
	pkg	  = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	tgt = proj_add_target(&proj, 1, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 6, ALLOC_STD);
	config_str_list(&config, CONFIG_PKG_DEPS, 1, -1, CONFIG_ACT_SET, STRV("dep"), NULL);

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_pkg_dep_pkg_not_found, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str_list(&config, CONFIG_PKG_DEPS, 0, 0, CONFIG_ACT_SET, STRV("dep"), NULL);

	log_set_quiet(0, 1);
	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_pkg_dep_tgt_not_found, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("dep"));
	pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str_list(&config, CONFIG_PKG_DEPS, 1, -1, CONFIG_ACT_SET, STRV("dep:dep_tgt"), NULL);

	log_set_quiet(0, 1);
	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_tgt_src, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str(&config, CONFIG_TGT_SRC, 0, 0, CONFIG_ACT_SET, STRV("src"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	tgt = proj_get_target(&proj, 0);

	strv_t str = proj_get_str(&proj, tgt->strs + TGT_STR_SRC);
	EXPECT_STRN(str.data, "src", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_tgt_inc, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str(&config, CONFIG_TGT_INC, 0, 0, CONFIG_ACT_SET, STRV("include"));

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	tgt = proj_get_target(&proj, 0);

	strv_t str = proj_get_str(&proj, tgt->strs + TGT_STR_INC);
	EXPECT_STRN(str.data, "include", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_tgt_inc_priv, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str_list(&config, CONFIG_TGT_INCS_PRIV, 0, 0, CONFIG_ACT_SET, STRV("src"), NULL);

	config_val_t *val = arr_get(&config.vals, 0);
	mem_oom(1);
	proj.lists.cnt = proj.lists.cap;
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 1);
	proj.lists.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	tgt	   = proj_get_target(&proj, 0);
	uint *id   = list_get_at(&proj.lists, tgt->incs_priv, 0, NULL);
	strv_t str = proj_get_str(&proj, *id);
	EXPECT_STRN(str.data, "src", str.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_tgt_dep_pkg, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("dep"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("dep_tgt"));
	pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	tgt = proj_add_target(&proj, 1, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 5, ALLOC_STD);
	config_str_list(&config, CONFIG_TGT_DEPS, 1, 1, CONFIG_ACT_SET, STRV("dep"), NULL);

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_tgt_dep_tgt, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("dep"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("dep_tgt"));
	pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	tgt = proj_add_target(&proj, 1, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 5, ALLOC_STD);
	config_str_list(&config, CONFIG_TGT_DEPS, 1, 1, CONFIG_ACT_SET, STRV("dep:dep_tgt"), NULL);

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_tgt_dep_pkg_not_test, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("dep"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("test"));
	tgt->type = TARGET_TYPE_TST;
	pkg	  = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	tgt = proj_add_target(&proj, 1, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 6, ALLOC_STD);
	config_str_list(&config, CONFIG_TGT_DEPS, 1, 1, CONFIG_ACT_SET, STRV("dep"), NULL);

	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_tgt_dep_pkg_not_found, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	target_t *tgt = proj_add_target(&proj, 0, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str_list(&config, CONFIG_TGT_DEPS, 0, 0, CONFIG_ACT_SET, STRV("dep"), NULL);

	log_set_quiet(0, 1);
	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);

	END;
}

TESTP(mod_base_apply_val_tgt_dep_tgt_not_found, mod_t *mod, const config_schema_t *schema)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);
	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("dep"));
	pkg = proj_add_pkg(&proj, NULL);
	proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));
	target_t *tgt = proj_add_target(&proj, 1, NULL);
	proj_set_str(&proj, tgt->strs + TGT_STR_NAME, STRV("tgt"));

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str_list(&config, CONFIG_TGT_DEPS, 1, 0, CONFIG_ACT_SET, STRV("dep:dep_tgt"), NULL);

	log_set_quiet(0, 1);
	config_val_t *val = arr_get(&config.vals, 0);
	EXPECT_EQ(mod->apply_val(mod, schema, &config, val, &proj), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);

	END;
}

STEST(mod_base)
{
	SSTART;

	config_schema_t schema = {0};
	config_schema_init(&schema, 8, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mod_t *mod = mod_base_get();
	RUNP(mod_base_apply_val, mod, &schema);
	RUNP(mod_base_apply_val_pkg_path, mod, &schema);
	RUNP(mod_base_apply_val_pkg_dep_pkg, mod, &schema);
	RUNP(mod_base_apply_val_pkg_dep_tgt, mod, &schema);
	RUNP(mod_base_apply_val_pkg_dep_not_test, mod, &schema);
	RUNP(mod_base_apply_val_pkg_dep_pkg_not_found, mod, &schema);
	RUNP(mod_base_apply_val_pkg_dep_tgt_not_found, mod, &schema);
	RUNP(mod_base_apply_val_tgt_src, mod, &schema);
	RUNP(mod_base_apply_val_tgt_inc, mod, &schema);
	RUNP(mod_base_apply_val_tgt_inc_priv, mod, &schema);
	RUNP(mod_base_apply_val_tgt_dep_pkg, mod, &schema);
	RUNP(mod_base_apply_val_tgt_dep_tgt, mod, &schema);
	RUNP(mod_base_apply_val_tgt_dep_pkg_not_test, mod, &schema);
	RUNP(mod_base_apply_val_tgt_dep_pkg_not_found, mod, &schema);
	RUNP(mod_base_apply_val_tgt_dep_tgt_not_found, mod, &schema);
	config_schema_free(&schema);

	SEND;
}
