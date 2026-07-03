#include "mod.h"

#include "mod_base.h"
#include "path.h"
#include "test.h"

typedef struct mod_example_ctx_s {
	registry_t registry;
	config_t config;
	config_t tmp;
	fs_t fs;
	config_sync_plan_t plan;
	char data[512];
	str_t buf;
} mod_example_ctx_t;

static mod_t *mod_example_init()
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (!strv_eq(mod->name, STRV("mod_example"))) {
			continue;
		}

		return mod;
	}

	return NULL;
}

static void mod_example_ctx_init(mod_example_ctx_t *ctx)
{
	registry_init(&ctx->registry, 1, ALLOC_STD);
	config_init(&ctx->config, 1, ALLOC_STD);
	config_init(&ctx->tmp, 1, ALLOC_STD);
	fs_init(&ctx->fs, 1, 1, ALLOC_STD);
	config_sync_plan_init(&ctx->plan, 1, ALLOC_STD);
	ctx->buf = STRB(ctx->data, 0);
}

static void mod_example_ctx_free(mod_example_ctx_t *ctx)
{
	config_sync_plan_free(&ctx->plan);
	fs_free(&ctx->fs);
	config_free(&ctx->tmp);
	config_free(&ctx->config);
	registry_free(&ctx->registry);
}

static int mod_example_run(mod_t *mod, const config_schema_t *schema, mod_example_ctx_t *ctx, strv_t cur_path, strv_t name)
{
	return mod->config_fs(mod,
			      &ctx->config,
			      &ctx->tmp,
			      schema,
			      &ctx->registry,
			      &ctx->plan,
			      &ctx->fs,
			      STRV_NULL,
			      cur_path,
			      name,
			      &ctx->buf,
			      ALLOC_STD,
			      DST_STD());
}

static void mod_example_mkfile(fs_t *fs, strv_t path)
{
	void *f;
	fs_open(fs, path, "w", &f);
	fs_close(fs, f);
}

TESTP(mod_example_config_fs_no_example_dir, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_example_ctx_t ctx = {0};
	mod_example_ctx_init(&ctx);

	EXPECT_EQ(mod_example_run(mod, schema, &ctx, STRV_NULL, STRV("lib")), 0);
	EXPECT_EQ(ctx.config.vals.cnt, 0);

	mod_example_ctx_free(&ctx);

	END;
}

TESTP(mod_example_config_fs_no_example_src_dir, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_example_ctx_t ctx = {0};
	mod_example_ctx_init(&ctx);
	fs_mkdir(&ctx.fs, STRV("example"));

	EXPECT_EQ(mod_example_run(mod, schema, &ctx, STRV_NULL, STRV("lib")), 0);
	EXPECT_EQ(ctx.config.vals.cnt, 0);

	mod_example_ctx_free(&ctx);

	END;
}

TESTP(mod_example_config_fs_no_example_main, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_example_ctx_t ctx = {0};
	mod_example_ctx_init(&ctx);
	fs_mkdir(&ctx.fs, STRV("example"));
	fs_mkdir(&ctx.fs, STRV("example/src"));

	EXPECT_EQ(mod_example_run(mod, schema, &ctx, STRV_NULL, STRV("lib")), 0);
	EXPECT_EQ(ctx.config.vals.cnt, 0);

	mod_example_ctx_free(&ctx);

	END;
}

TESTP(mod_example_config_fs_lib_owner_dep, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_example_ctx_t ctx = {0};
	mod_example_ctx_init(&ctx);
	fs_mkdir(&ctx.fs, STRV("src"));
	fs_mkdir(&ctx.fs, STRV("example"));
	fs_mkdir(&ctx.fs, STRV("example/src"));
	mod_example_mkfile(&ctx.fs, STRV("example/src/main.c"));

	EXPECT_EQ(mod_example_run(mod, schema, &ctx, STRV_NULL, STRV("lib")), 0);

	char out[512] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += lib_example\n"
		   "lib_example:path ?= example\n"
		   "lib_example:tgts += lib_example\n"
		   "lib_example:lib_example:type = 1\n"
		   "lib_example:lib_example:src = src\n"
		   "lib_example:lib_example:incs_priv = src\n"
		   "lib_example:lib_example:deps = lib:lib\n");

	mod_example_ctx_free(&ctx);

	END;
}

TESTP(mod_example_config_fs_exe_owner_no_lib_dep, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_example_ctx_t ctx = {0};
	mod_example_ctx_init(&ctx);
	fs_mkdir(&ctx.fs, STRV("src"));
	mod_example_mkfile(&ctx.fs, STRV("src/main.c"));
	fs_mkdir(&ctx.fs, STRV("example"));
	fs_mkdir(&ctx.fs, STRV("example/src"));
	mod_example_mkfile(&ctx.fs, STRV("example/src/main.c"));

	EXPECT_EQ(mod_example_run(mod, schema, &ctx, STRV_NULL, STRV("exe")), 0);

	char out[512] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += exe_example\n"
		   "exe_example:path ?= example\n"
		   "exe_example:tgts += exe_example\n"
		   "exe_example:exe_example:type = 1\n"
		   "exe_example:exe_example:src = src\n"
		   "exe_example:exe_example:incs_priv = src\n");

	mod_example_ctx_free(&ctx);

	END;
}

TESTP(mod_example_config_fs_driver_dep, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_example_ctx_t ctx = {0};
	mod_example_ctx_init(&ctx);
	fs_mkdir(&ctx.fs, STRV("src"));
	fs_mkdir(&ctx.fs, STRV("drivers"));
	fs_mkdir(&ctx.fs, STRV("example"));
	fs_mkdir(&ctx.fs, STRV("example/src"));
	mod_example_mkfile(&ctx.fs, STRV("example/src/main.c"));

	EXPECT_EQ(mod_example_run(mod, schema, &ctx, STRV_NULL, STRV("lib")), 0);

	char out[512] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += lib_example\n"
		   "lib_example:path ?= example\n"
		   "lib_example:tgts += lib_example\n"
		   "lib_example:lib_example:type = 1\n"
		   "lib_example:lib_example:src = src\n"
		   "lib_example:lib_example:incs_priv = src\n"
		   "lib_example:lib_example:deps = lib:lib, lib:drivers\n");

	mod_example_ctx_free(&ctx);

	END;
}

TESTP(mod_example_config_fs_nested_path, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_example_ctx_t ctx = {0};
	mod_example_ctx_init(&ctx);
	fs_mkdir(&ctx.fs, STRV("pkgs"));
	fs_mkdir(&ctx.fs, STRV("pkgs/lib"));
	fs_mkdir(&ctx.fs, STRV("pkgs/lib/src"));
	fs_mkdir(&ctx.fs, STRV("pkgs/lib/example"));
	fs_mkdir(&ctx.fs, STRV("pkgs/lib/example/src"));
	mod_example_mkfile(&ctx.fs, STRV("pkgs/lib/example/src/main.c"));

	EXPECT_EQ(mod_example_run(mod, schema, &ctx, STRV("pkgs/lib"), STRV("lib")), 0);

	char out[512] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += lib_example\n"
		   "lib_example:path ?= pkgs" SEP "lib" SEP "example\n"
		   "lib_example:tgts += lib_example\n"
		   "lib_example:lib_example:type = 1\n"
		   "lib_example:lib_example:src = src\n"
		   "lib_example:lib_example:incs_priv = src\n"
		   "lib_example:lib_example:deps = lib:lib\n");

	mod_example_ctx_free(&ctx);

	END;
}

STEST(mod_example)
{
	SSTART;

	config_schema_t schema = {0};
	config_schema_init(&schema, 8, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mod_t *mod = mod_example_init();
	RUNP(mod_example_config_fs_no_example_dir, mod, &schema);
	RUNP(mod_example_config_fs_no_example_src_dir, mod, &schema);
	RUNP(mod_example_config_fs_no_example_main, mod, &schema);
	RUNP(mod_example_config_fs_lib_owner_dep, mod, &schema);
	RUNP(mod_example_config_fs_exe_owner_no_lib_dep, mod, &schema);
	RUNP(mod_example_config_fs_driver_dep, mod, &schema);
	RUNP(mod_example_config_fs_nested_path, mod, &schema);
	config_schema_free(&schema);

	SEND;
}
