#include "mod.h"

#include "log.h"
#include "mem.h"
#include "mod_base.h"
#include "path.h"
#include "test.h"

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

typedef struct mod_cfg_ctx_s {
	registry_t registry;
	config_t config;
	config_t tmp;
	fs_t fs;
	proc_t proc;
	char data[256];
	str_t buf;
} mod_cfg_ctx_t;

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

static void mod_cfg_ctx_init(mod_cfg_ctx_t *ctx)
{
	registry_init(&ctx->registry, 1, ALLOC_STD);
	config_init(&ctx->config, 1, ALLOC_STD);
	config_init(&ctx->tmp, 1, ALLOC_STD);
	fs_init(&ctx->fs, 4, 1, ALLOC_STD);
	proc_init(&ctx->proc, 32, 1, ALLOC_STD);
	ctx->buf = STRB(ctx->data, 0);
}

static void mod_cfg_ctx_free(mod_cfg_ctx_t *ctx)
{
	proc_free(&ctx->proc);
	fs_free(&ctx->fs);
	config_free(&ctx->tmp);
	config_free(&ctx->config);
	registry_free(&ctx->registry);
}

static void write_build_cfg(fs_t *fs, strv_t cur_path, strv_t cfg)
{
	if (cur_path.data) {
		fs_mkpath(fs, STRV_NULL, cur_path);
	}

	path_t path = {0};
	path_init(&path, cur_path);
	path_push(&path, STRV("build.cfg"));

	void *f;
	fs_open(fs, STRVS(path), "w", &f);
	fs_writes(fs, f, cfg);
	fs_close(fs, f);
}

static int run_build_cfg(mod_t *mod, mod_cfg_ctx_t *ctx, const config_schema_t *schema, config_sync_plan_t *plan, strv_t cur_path,
			 strv_t name, dst_t dst)
{
	return mod->config_fs(
		mod, &ctx->config, &ctx->tmp, schema, &ctx->registry, plan, &ctx->fs, STRV_NULL, cur_path, name, &ctx->buf, ALLOC_STD, dst);
}

static int load_build_cfg(mod_t *mod, mod_cfg_ctx_t *ctx, const config_schema_t *schema, strv_t cfg, strv_t cur_path, strv_t name,
			  dst_t dst)
{
	write_build_cfg(&ctx->fs, cur_path, cfg);

	config_sync_plan_t plan = {0};
	if (config_sync_plan_init(&plan, 1, ALLOC_STD) == NULL) {
		return 1;
	}

	int ret = run_build_cfg(mod, ctx, schema, &plan, cur_path, name, dst);
	config_sync_plan_free(&plan);

	return ret;
}

TESTP(config_cfg_empty, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV(""), STRV_NULL, STRV_NULL, DST_NONE()), 0);
	EXPECT_EQ(ctx.config.vals.cnt, 0);
	EXPECT_EQ(ctx.registry.pkgs.cnt, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_pkg_add_oom, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);
	write_build_cfg(&ctx.fs, STRV_NULL, STRV("deps = [dep]\n"));

	config_sync_plan_t plan = {0};
	EXPECT_EQ(config_sync_plan_init(&plan, 1, ALLOC_STD), &plan);

	ctx.registry.pkgs.cnt = ctx.registry.pkgs.cap;
	mem_oom(1);
	EXPECT_EQ(run_build_cfg(mod, &ctx, schema, &plan, STRV_NULL, STRV("root"), DST_NONE()), 1);
	mem_oom(0);

	config_sync_plan_free(&plan);
	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_deps, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("deps = [dep1, dep2]\n"), STRV_NULL, STRV_NULL, DST_NONE()), 0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":deps = dep1, dep2\n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_deps_oom, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);
	write_build_cfg(&ctx.fs, STRV_NULL, STRV("deps = [dep1, dep2]\n"));

	config_free(&ctx.tmp);
	config_init(&ctx.tmp, 2, ALLOC_STD);

	config_sync_plan_t plan = {0};
	EXPECT_EQ(config_sync_plan_init(&plan, 1, ALLOC_STD), &plan);

	ctx.tmp.lists.cnt = ctx.tmp.lists.cap - 1;
	mem_oom(1);
	EXPECT_EQ(run_build_cfg(mod, &ctx, schema, &plan, STRV_NULL, STRV_NULL, DST_NONE()), 1);
	ctx.tmp.lists.cnt = 0;
	mem_oom(0);

	config_sync_plan_free(&plan);
	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_deps_invalid, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("deps = [\"dep\"]\n"), STRV_NULL, STRV_NULL, DST_NONE()), 1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_uri, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("uri = \"uri\"\n"), STRV_NULL, STRV_NULL, DST_NONE()), 0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":uri = uri\n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_uri_invalid, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("uri = uri\n"), STRV_NULL, STRV_NULL, DST_NONE()), 1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_inc, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("include = \"include\"\n"), STRV_NULL, STRV_NULL, DST_NONE()), 0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":inc = include\n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_inc_invalid, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("include = include\n"), STRV_NULL, STRV_NULL, DST_NONE()), 1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_pkg_app, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("[+pkg:]\n"), STRV_NULL, STRV_NULL, DST_NONE()), 0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_pkg_app_invalid, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("[+pkg]\n"), STRV_NULL, STRV_NULL, DST_NONE()), 1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_pkg_en, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("[?pkg:]\n"), STRV_NULL, STRV_NULL, DST_NONE()), 0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_pkg_ops, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod,
				 &ctx,
				 schema,
				 STRV("[?pkg:]\n"
				      "deps = [dep]\n"
				      "uri = \"uri\"\n"
				      "include = \"include\"\n"),
				 STRV_NULL,
				 STRV_NULL,
				 DST_NONE()),
		  0);

	char out[512] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":deps = dep\n"
		   ":uri = uri\n"
		   ":inc = include\n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_pkg_ops_oom, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);
	write_build_cfg(&ctx.fs,
			STRV_NULL,
			STRV("[?pkg:]\n"
			     "deps = [dep]\n"));

	config_free(&ctx.tmp);
	config_init(&ctx.tmp, 2, ALLOC_STD);

	config_sync_plan_t plan = {0};
	EXPECT_EQ(config_sync_plan_init(&plan, 1, ALLOC_STD), &plan);

	ctx.tmp.lists.cnt = ctx.tmp.lists.cap;
	mem_oom(1);
	EXPECT_EQ(run_build_cfg(mod, &ctx, schema, &plan, STRV_NULL, STRV_NULL, DST_NONE()), 1);
	ctx.tmp.lists.cnt = 0;
	mem_oom(0);

	config_sync_plan_free(&plan);
	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_pkg_set_not_found, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod,
				 &ctx,
				 schema,
				 STRV("[pkg:other=other]\n"
				      "deps = [dep]\n"
				      ""),
				 STRV("pkgs/root"),
				 STRV("root"),
				 DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_tgt_app, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("[+tgt:]\n"), STRV_NULL, STRV_NULL, DST_NONE()), 0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":tgts += \n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_tgt_app_invalid, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("[+tgt]\n"), STRV_NULL, STRV_NULL, DST_NONE()), 1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_tgt_en, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("[?tgt:]\n"), STRV_NULL, STRV_NULL, DST_NONE()), 0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":tgts ?= \n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_tgt_ops, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod,
				 &ctx,
				 schema,
				 STRV("[+tgt:]\n"
				      "prep = \"prep\"\n"
				      "conf = \"conf\"\n"
				      "comp = \"comp\"\n"
				      "inst = \"inst\"\n"
				      "out = \"out\"\n"),
				 STRV_NULL,
				 STRV_NULL,
				 DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":tgts += \n"
		   "::prep = prep\n"
		   "::conf = conf\n"
		   "::comp = comp\n"
		   "::inst = inst\n"
		   "::path = out\n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_tgt_deps, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod,
				 &ctx,
				 schema,
				 STRV("[+tgt:]\n"
				      "deps = [dep1, dep2]\n"),
				 STRV_NULL,
				 STRV_NULL,
				 DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":tgts += \n"
		   "::deps = dep1, dep2\n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_tgt_deps_oom, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);
	write_build_cfg(&ctx.fs,
			STRV_NULL,
			STRV("[+tgt:]\n"
			     "deps = [dep1, dep2]\n"));

	config_free(&ctx.tmp);
	config_init(&ctx.tmp, 3, ALLOC_STD);

	config_sync_plan_t plan = {0};
	EXPECT_EQ(config_sync_plan_init(&plan, 1, ALLOC_STD), &plan);

	ctx.tmp.lists.cnt = ctx.tmp.lists.cap - 2;
	mem_oom(1);
	EXPECT_EQ(run_build_cfg(mod, &ctx, schema, &plan, STRV_NULL, STRV_NULL, DST_NONE()), 1);
	ctx.tmp.lists.cnt = 0;
	mem_oom(0);

	config_sync_plan_free(&plan);
	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_tgt_deps_invalid, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod,
				 &ctx,
				 schema,
				 STRV("[+tgt:]\n"
				      "deps = [\"dep\"]\n"),
				 STRV_NULL,
				 STRV_NULL,
				 DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_tgt_set_not_found, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod,
				 &ctx,
				 schema,
				 STRV("[tgt:miss=miss]\n"
				      "prep = \"prep\"\n"
				      ""),
				 STRV("pkgs/root"),
				 STRV("root"),
				 DST_NONE()),
		  1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_target_lib, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod,
				 &ctx,
				 schema,
				 STRV("[+tgt:]\n"
				      "lib = \"lib\"\n"),
				 STRV_NULL,
				 STRV_NULL,
				 DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":tgts += \n"
		   "::name = lib\n"
		   "::type = 1\n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_target_exe, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod,
				 &ctx,
				 schema,
				 STRV("[+tgt:]\n"
				      "exe = \"exe\"\n"),
				 STRV_NULL,
				 STRV_NULL,
				 DST_NONE()),
		  0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n"
		   ":tgts += \n"
		   "::name = exe\n"
		   "::type = 2\n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_ext, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("ext:\n\"repo\"\n\n"), STRV_NULL, STRV_NULL, DST_STD()), 0);
	EXPECT_EQ(fs_isfile(&ctx.fs, STRV("tmp/.gitignore")), 0);
	EXPECT_EQ(fs_isdir(&ctx.fs, STRV("tmp/ext/repo")), 0);
	EXPECT_EQ(ctx.proc.buf.len, 0);
	EXPECT_EQ(ctx.config.vals.cnt, 0);
	EXPECT_EQ(ctx.registry.pkgs.cnt, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_ext_invalid, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("ext:\nrepo\n"), STRV_NULL, STRV_NULL, DST_STD()), 1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_ext_invalid_format, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("ext:\n1\n\n"), STRV_NULL, STRV_NULL, DST_STD()), 1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(config_cfg_ext_oom_second, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t warmup = {0};
	mod_cfg_ctx_init(&warmup);
	EXPECT_EQ(load_build_cfg(mod, &warmup, schema, STRV("ext:\n\"repo1\"\n\"repo2\"\n\n"), STRV_NULL, STRV_NULL, DST_NONE()), 0);
	mod_cfg_ctx_free(&warmup);

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);
	write_build_cfg(&ctx.fs, STRV_NULL, STRV("ext:\n\"repo1\"\n\"repo2\"\n\n"));

	config_sync_plan_t plan = {0};
	EXPECT_EQ(config_sync_plan_init(&plan, 1, ALLOC_STD), &plan);

	log_set_quiet(0, 1);
	mem_oom(1);
	EXPECT_EQ(run_build_cfg(mod, &ctx, schema, &plan, STRV_NULL, STRV_NULL, DST_NONE()), 1);
	mem_oom(0);
	log_set_quiet(0, 0);

	config_sync_plan_free(&plan);
	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(mod_cfg_config_fs_null_config, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);
	write_build_cfg(&ctx.fs, STRV_NULL, STRV("[+pkg:]\n"));

	config_sync_plan_t plan = {0};
	config_sync_plan_init(&plan, 1, ALLOC_STD);

	EXPECT_EQ(mod->config_fs(mod,
				 NULL,
				 &ctx.tmp,
				 schema,
				 &ctx.registry,
				 &plan,
				 &ctx.fs,
				 STRV_NULL,
				 STRV_NULL,
				 STRV_NULL,
				 &ctx.buf,
				 ALLOC_STD,
				 DST_NONE()),
		  1);

	config_sync_plan_free(&plan);
	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(mod_cfg_config_fs, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("[+pkg:]\n"), STRV_NULL, STRV_NULL, DST_STD()), 0);

	char out[256] = {0};
	config_print(&ctx.config, schema, &ctx.registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += \n"
		   ":path ?= \n");

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(mod_cfg_config_fs_invalid, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);

	log_set_quiet(0, 1);
	EXPECT_EQ(load_build_cfg(mod, &ctx, schema, STRV("ext:\nuri = invalid\n"), STRV_NULL, STRV_NULL, DST_NONE()), 1);
	log_set_quiet(0, 0);

	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(mod_cfg_config_fs_null_priv, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);
	write_build_cfg(&ctx.fs, STRV_NULL, STRV("[+pkg:]\n"));

	config_sync_plan_t plan = {0};
	config_sync_plan_init(&plan, 1, ALLOC_STD);

	mod_t fake = *mod;
	fake.priv  = NULL;
	EXPECT_EQ(fake.config_fs(&fake,
				 &ctx.config,
				 &ctx.tmp,
				 schema,
				 &ctx.registry,
				 &plan,
				 &ctx.fs,
				 STRV_NULL,
				 STRV_NULL,
				 STRV_NULL,
				 &ctx.buf,
				 ALLOC_STD,
				 DST_NONE()),
		  1);

	config_sync_plan_free(&plan);
	mod_cfg_ctx_free(&ctx);

	END;
}

TESTP(mod_cfg_config_fs_null_buf, mod_t *mod, const config_schema_t *schema)
{
	START;

	mod_cfg_ctx_t ctx = {0};
	mod_cfg_ctx_init(&ctx);
	write_build_cfg(&ctx.fs, STRV_NULL, STRV("[+pkg:]\n"));

	config_sync_plan_t plan = {0};
	config_sync_plan_init(&plan, 1, ALLOC_STD);

	EXPECT_EQ(mod->config_fs(mod,
				 &ctx.config,
				 &ctx.tmp,
				 schema,
				 &ctx.registry,
				 &plan,
				 &ctx.fs,
				 STRV_NULL,
				 STRV_NULL,
				 STRV_NULL,
				 NULL,
				 ALLOC_STD,
				 DST_NONE()),
		  1);

	config_sync_plan_free(&plan);
	mod_cfg_ctx_free(&ctx);

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
	RUNP(config_cfg_pkg_add_oom, mod, &schema);
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
	RUNP(config_cfg_pkg_ops, mod, &schema);
	RUNP(config_cfg_pkg_ops_oom, mod, &schema);
	RUNP(config_cfg_pkg_set_not_found, mod, &schema);
	RUNP(config_cfg_tgt_app, mod, &schema);
	RUNP(config_cfg_tgt_app_invalid, mod, &schema);
	RUNP(config_cfg_tgt_en, mod, &schema);
	RUNP(config_cfg_tgt_ops, mod, &schema);
	RUNP(config_cfg_tgt_deps, mod, &schema);
	RUNP(config_cfg_tgt_deps_oom, mod, &schema);
	RUNP(config_cfg_tgt_deps_invalid, mod, &schema);
	RUNP(config_cfg_tgt_set_not_found, mod, &schema);
	RUNP(config_cfg_target_lib, mod, &schema);
	RUNP(config_cfg_target_exe, mod, &schema);
	RUNP(config_cfg_ext, mod, &schema);
	RUNP(config_cfg_ext_invalid, mod, &schema);
	RUNP(config_cfg_ext_invalid_format, mod, &schema);
	RUNP(config_cfg_ext_oom_second, mod, &schema);
	RUNP(mod_cfg_config_fs, mod, &schema);
	RUNP(mod_cfg_config_fs_invalid, mod, &schema);
	RUNP(mod_cfg_config_fs_null_priv, mod, &schema);
	RUNP(mod_cfg_config_fs_null_buf, mod, &schema);
	RUNP(mod_cfg_config_fs_null_config, mod, &schema);
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
