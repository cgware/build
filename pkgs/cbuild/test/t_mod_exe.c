#include "mod.h"

#include "mod_base.h"
#include "test.h"

static mod_t *mod_exe_init()
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (!strv_eq(mod->name, STRV("mod_exe"))) {
			continue;
		}

		return mod;
	}

	return NULL;
}

TESTP(mod_exe_config_fs, mod_t *mod, const config_schema_t *schema)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tc = {0};
	config_init(&tc, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));
	fs_mkfile(&fs, STRV("src/main.c"));

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
		   "pkgs ?= \n"
		   ":path ?= \n"
		   ":tgts += \n"
		   "::type = 1\n"
		   "::src = src\n"
		   "::incs_priv = src\n");

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(mod_exe_proj_cfg, mod_t *mod)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg;
	proj_add_pkg(&proj, &pkg);
	list_node_t exe, lib, drv;
	target_t *t = proj_add_target(&proj, pkg, &exe);
	t->type	    = TARGET_TYPE_EXE;
	t	    = proj_add_target(&proj, pkg, &lib);
	t->type	    = TARGET_TYPE_LIB;
	t	    = proj_add_target(&proj, pkg, &drv);
	t->type	    = TARGET_TYPE_DRV;

	EXPECT_EQ(mod->proj_cfg(mod, &proj), 0);

	t = proj_get_target(&proj, exe);
	list_node_t *dep;
	dep = list_get_at(&proj.deps, t->deps, 0, NULL);
	EXPECT_EQ(*dep, lib);
	dep = list_get_at(&proj.deps, t->deps, 1, NULL);
	EXPECT_EQ(*dep, drv);

	proj_free(&proj);

	END;
}

STEST(mod_exe)
{
	SSTART;

	config_schema_t schema = {0};
	config_schema_init(&schema, 8, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mod_t *mod = mod_exe_init();
	RUNP(mod_exe_config_fs, mod, &schema);
	RUNP(mod_exe_proj_cfg, mod);
	config_schema_free(&schema);

	SEND;
}
