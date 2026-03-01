#include "mod.h"

#include "test.h"

static mod_t *mod_test_init()
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (!strv_eq(mod->name, STRV("mod_test"))) {
			continue;
		}

		mod->init(mod, 0, ALLOC_STD);
		return mod;
	}

	return NULL;
}

static void mod_test_free(mod_t *mod)
{
	mod->free(mod);
}

TESTP(mod_test_config_fs, mod_t *mod)
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

	fs_mkdir(&fs, STRV("test"));
	fs_mkdir(&fs, STRV("src"));
	fs_mkdir(&fs, STRV("drivers"));

	proc_t proc = {0};
	proc_init(&proc, 32, 1);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(mod->config_fs(mod, &config, &tc, &registry, &fs, &proc, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()), 0);

	char out[256] = {0};
	config_print(&config, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs ?= \n"
		   "::path ?= \n"
		   "\n"
		   ":tgts += _test\n"
		   ":_test:type = 5\n"
		   ":_test:src = test\n"
		   ":_test:incs_priv = test, src, drivers\n");

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

TESTP(mod_test_proj_cfg, mod_t *mod)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg;
	proj_add_pkg(&proj, &pkg);
	list_node_t test, lib, drv;
	target_t *t = proj_add_target(&proj, pkg, &test);
	t->type	    = TARGET_TYPE_TST;
	t	    = proj_add_target(&proj, pkg, &lib);
	t->type	    = TARGET_TYPE_LIB;
	t	    = proj_add_target(&proj, pkg, &drv);
	t->type	    = TARGET_TYPE_DRV;

	EXPECT_EQ(mod->proj_cfg(mod, &proj), 0);

	t = proj_get_target(&proj, test);
	list_node_t *dep;
	dep = list_get_at(&proj.deps, t->deps, 0, NULL);
	EXPECT_EQ(*dep, lib);
	dep = list_get_at(&proj.deps, t->deps, 1, NULL);
	EXPECT_EQ(*dep, drv);

	proj_free(&proj);

	END;
}

STEST(mod_test)
{
	SSTART;

	mod_t *mod = mod_test_init();
	RUNP(mod_test_config_fs, mod);
	RUNP(mod_test_proj_cfg, mod);
	mod_test_free(mod);
	SEND;
}
