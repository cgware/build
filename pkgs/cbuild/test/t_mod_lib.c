#include "mod.h"

#include "mod_base.h"
#include "test.h"

static mod_t *mod_lib_init()
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (!strv_eq(mod->name, STRV("mod_lib"))) {
			continue;
		}

		return mod;
	}

	return NULL;
}

TESTP(mod_lib_config_fs, mod_t *mod, const config_schema_t *schema)
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
	fs_mkdir(&fs, STRV("include"));

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
		   "::type = 2\n"
		   "::src = src\n"
		   "::inc = include\n"
		   "::incs_priv = src\n");

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

STEST(mod_lib)
{
	SSTART;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mod_t *mod = mod_lib_init();
	RUNP(mod_lib_config_fs, mod, &schema);
	config_schema_free(&schema);

	SEND;
}
