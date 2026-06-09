#include "mod.h"

#include "mem.h"
#include "mod_base.h"
#include "path.h"
#include "test.h"

static mod_t *mod_pkgs_init()
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (!strv_eq(mod->name, STRV("mod_pkgs"))) {
			continue;
		}

		return mod;
	}

	return NULL;
}

TESTP(mod_pkgs_config_fs, mod_t *mod, const config_schema_t *schema)
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

	fs_mkdir(&fs, STRV("pkgs"));
	fs_mkdir(&fs, STRV("pkgs/p1"));
	fs_mkdir(&fs, STRV("pkgs/p1/src"));
	fs_mkdir(&fs, STRV("pkgs/p2"));
	fs_mkdir(&fs, STRV("pkgs/p2/src"));

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	config_sync_plan_t plan = {0};
	config_sync_plan_init(&plan, 1, ALLOC_STD);
	EXPECT_EQ(mod->config_fs(
			  mod, &config, &tc, schema, &registry, &plan, &fs, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()),
		  0);
	EXPECT_EQ(plan.items.cnt, 2);
	config_sync_item_t *item = arr_get(&plan.items, 0);
	EXPECT_EQ(item->kind, CONFIG_SYNC_KIND_DIR);
	strv_t val = strbuf_get(&plan.strs, item->path);
	EXPECT_STRN(val.data, "pkgs" SEP "p1", val.len);
	val = strbuf_get(&plan.strs, item->name);
	EXPECT_STRN(val.data, "p1", val.len);
	item = arr_get(&plan.items, 1);
	EXPECT_EQ(item->kind, CONFIG_SYNC_KIND_DIR);
	val = strbuf_get(&plan.strs, item->path);
	EXPECT_STRN(val.data, "pkgs" SEP "p2", val.len);
	val = strbuf_get(&plan.strs, item->name);
	EXPECT_STRN(val.data, "p2", val.len);

	config_sync_plan_free(&plan);
	fs_free(&fs);
	config_free(&tc);
	config_free(&config);
	registry_free(&registry);

	END;
}

STEST(mod_pkgs)
{
	SSTART;

	config_schema_t schema = {0};
	config_schema_init(&schema, 8, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mod_t *mod = mod_pkgs_init();
	RUNP(mod_pkgs_config_fs, mod, &schema);
	config_schema_free(&schema);

	SEND;
}
