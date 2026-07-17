#include "config_fs.h"

#include "log.h"
#include "mem.h"
#include "mod_base.h"
#include "path.h"
#include "test.h"

typedef struct fail_alloc_ctx_s {
	uint fail_at;
	uint cnt;
} fail_alloc_ctx_t;

static void *fail_alloc_alloc(alloc_t *alloc, size_t size)
{
	fail_alloc_ctx_t *ctx = alloc->priv;

	if (ctx && ctx->fail_at && ++ctx->cnt == ctx->fail_at) {
		return NULL;
	}

	return alloc_alloc_std(alloc, size);
}

static int fail_alloc_realloc(alloc_t *alloc, void **ptr, size_t *old_size, size_t new_size)
{
	return alloc_realloc_std(alloc, ptr, old_size, new_size);
}

static void fail_alloc_free(alloc_t *alloc, void *ptr, size_t size)
{
	alloc_free_std(alloc, ptr, size);
}

static void *fail_realloc_alloc(alloc_t *alloc, size_t size)
{
	return alloc_alloc_std(alloc, size);
}

static int fail_realloc_realloc(alloc_t *alloc, void **ptr, size_t *old_size, size_t new_size)
{
	fail_alloc_ctx_t *ctx = alloc->priv;

	if (ctx && ctx->fail_at && ++ctx->cnt == ctx->fail_at) {
		return 1;
	}

	return alloc_realloc_std(alloc, ptr, old_size, new_size);
}

static void fail_realloc_free(alloc_t *alloc, void *ptr, size_t size)
{
	alloc_free_std(alloc, ptr, size);
}

static void mods_init(config_schema_t *schema)
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (mod->init) {
			mod->init(mod, 2, schema, ALLOC_STD);
		}
	}
}

static void mods_free(void)
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (mod->free) {
			mod->free(mod);
		}
	}
}

static mod_t *find_mod(strv_t name)
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != DRIVER_TYPE_MOD) {
			continue;
		}

		mod_t *mod = i->data;
		if (strv_eq(mod->name, name)) {
			return mod;
		}
	}

	return NULL;
}

static int invalid_kind_config_fs(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry,
				  config_sync_plan_t *plan, fs_t *fs, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf,
				  alloc_t alloc, dst_t dst)
{
	(void)mod;
	(void)config;
	(void)tmp;
	(void)schema;
	(void)registry;
	(void)fs;
	(void)proj_path;
	(void)cur_path;
	(void)name;
	(void)buf;
	(void)alloc;
	(void)dst;

	config_sync_item_t *item = arr_add(&plan->items, NULL);
	if (item == NULL) {
		return 1;
	}

	item->kind = CONFIG_SYNC_KIND_UNKNOWN;
	item->uri  = -1;
	item->path = -1;
	item->name = -1;

	return 0;
}

static int ext_duplicate_config_fs(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry,
				   config_sync_plan_t *plan, fs_t *fs, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf,
				   alloc_t alloc, dst_t dst)
{
	(void)mod;
	(void)config;
	(void)tmp;
	(void)schema;
	(void)registry;
	(void)fs;
	(void)proj_path;
	(void)cur_path;
	(void)name;
	(void)buf;
	(void)alloc;
	(void)dst;

	if (config_sync_plan_add_ext(plan, STRV("u"), STRV("r"))) {
		return 1;
	}
	if (config_sync_plan_add_ext(plan, STRV("u"), STRV("r"))) {
		return 1;
	}

	return 0;
}

static int ext_fill_config_fs(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry,
			      config_sync_plan_t *plan, fs_t *fs, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc,
			      dst_t dst)
{
	(void)mod;
	(void)config;
	(void)tmp;
	(void)schema;
	(void)registry;
	(void)fs;
	(void)proj_path;
	(void)cur_path;
	(void)name;
	(void)buf;
	(void)alloc;
	(void)dst;

	if (config_sync_plan_add_ext(plan, STRV("u"), STRV("r"))) {
		return 1;
	}
	if (config_sync_plan_add_ext(plan, STRV("u"), STRV("r"))) {
		return 1;
	}
	if (config_sync_plan_add_ext(plan, STRV("u"), STRV("r"))) {
		return 1;
	}

	return 0;
}

static int ext_str_full_config_fs(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry,
				  config_sync_plan_t *plan, fs_t *fs, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf,
				  alloc_t alloc, dst_t dst)
{
	(void)mod;
	(void)config;
	(void)tmp;
	(void)schema;
	(void)registry;
	(void)fs;
	(void)proj_path;
	(void)cur_path;
	(void)name;
	(void)buf;
	(void)alloc;
	(void)dst;

	if (config_sync_plan_add_ext(plan, STRV("u"), STRV("r"))) {
		return 1;
	}

	plan->strs.off.cnt = plan->strs.off.cap;

	return 0;
}

static void config_fs_ext_clone_cmd(strv_t uri, int develop, str_t *cmd)
{
	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 4, 1, ALLOC_STD);

	void *f;
	fs_open(&fs, STRV("build.cfg"), "w", &f);
	fs_writes(&fs, f, STRV("ext:\n\""));
	fs_writes(&fs, f, uri);
	fs_writes(&fs, f, STRV("\"\n\n"));
	fs_close(&fs, f);

	proc_t proc = {0};
	proc_init(&proc, 32, 1, ALLOC_STD);

	char storage[256] = {0};
	str_t buf	  = STRB(storage, 0);

	config_fs(&config, &tmp, &schema, &registry, &fs, &proc, STRV_NULL, STRV_NULL, STRV("proj"), develop, &buf, ALLOC_STD, DST_NONE());
	str_cat(cmd, STRVS(proc.buf));

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);
}

TEST(config_fs_empty)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	EXPECT_EQ(config_fs(NULL, NULL, NULL, NULL, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, 0, NULL, ALLOC_STD, DST_NONE()), 0);
	EXPECT_EQ(config_fs(&config, &tmp, &schema, &registry, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, 0, NULL, ALLOC_STD, DST_NONE()),
		  0);

	config_free(&config);
	config_free(&tmp);
	registry_free(&registry);
	config_schema_free(&schema);

	END;
}

TEST(config_sync_plan_helpers)
{
	START;

	EXPECT_EQ(config_sync_plan_init(NULL, 1, ALLOC_STD), NULL);
	config_sync_plan_free(NULL);
	EXPECT_EQ(config_sync_plan_add_ext(NULL, STRV("uri"), STRV("name")), 1);

	config_sync_plan_t plan = {0};
	EXPECT_EQ(config_sync_plan_init(&plan, 1, ALLOC_STD), &plan);
	EXPECT_EQ(config_sync_plan_add_ext(&plan, STRV("uri"), STRV("name")), 0);
	EXPECT_EQ(config_sync_plan_add_dir(&plan, STRV("pkgs/p1"), STRV("p1")), 0);
	EXPECT_EQ(config_sync_plan_add_dir(&plan, STRV("pkgs/p1"), STRV("p1")), 0);
	EXPECT_EQ(plan.items.cnt, 2);
	config_sync_plan_free(&plan);

	config_sync_plan_t path_fail = {0};
	EXPECT_EQ(config_sync_plan_init(&path_fail, 1, ALLOC_STD), &path_fail);
	path_fail.strs.off.cnt = path_fail.strs.off.cap;
	mem_oom(1);
	EXPECT_EQ(config_sync_plan_add_dir(&path_fail, STRV("0123456789abcdef0"), STRV("p1")), 1);
	mem_oom(0);
	config_sync_plan_free(&path_fail);

	fail_alloc_ctx_t init_fail_ctx = {.fail_at = 2};
	alloc_t init_fail_alloc	       = {
		       .alloc = fail_alloc_alloc, .realloc = fail_alloc_realloc, .free = fail_alloc_free, .priv = &init_fail_ctx};
	config_sync_plan_t init_fail = {0};
	log_set_quiet(0, 1);
	EXPECT_EQ(config_sync_plan_init(&init_fail, 1, init_fail_alloc), NULL);
	log_set_quiet(0, 0);

	config_sync_plan_t uri_fail = {0};
	EXPECT_EQ(config_sync_plan_init(&uri_fail, 1, ALLOC_STD), &uri_fail);
	uri_fail.strs.off.cnt = uri_fail.strs.off.cap;
	mem_oom(1);
	EXPECT_EQ(config_sync_plan_add_ext(&uri_fail, STRV("0123456789abcdef0"), STRV("repo")), 1);
	mem_oom(0);
	config_sync_plan_free(&uri_fail);

	config_sync_plan_t name_fail = {0};
	EXPECT_EQ(config_sync_plan_init(&name_fail, 1, ALLOC_STD), &name_fail);
	name_fail.strs.off.cnt = name_fail.strs.off.cap - 1;
	mem_oom(1);
	EXPECT_EQ(config_sync_plan_add_ext(&name_fail, STRV("uri"), STRV("repo")), 1);
	mem_oom(0);
	config_sync_plan_free(&name_fail);

	config_sync_plan_t item_fail = {0};
	EXPECT_EQ(config_sync_plan_init(&item_fail, 2, ALLOC_STD), &item_fail);
	EXPECT_EQ(config_sync_plan_add_ext(&item_fail, STRV("u"), STRV("n")), 0);
	item_fail.items.cnt = item_fail.items.cap;
	mem_oom(1);
	EXPECT_EQ(config_sync_plan_add_ext(&item_fail, STRV("u2"), STRV("n2")), 1);
	mem_oom(0);
	config_sync_plan_free(&item_fail);
	config_sync_plan_free(&plan);

	END;
}

TEST(config_fs_pkgs_queue)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 8, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("pkgs"));
	fs_mkdir(&fs, STRV("pkgs/p1"));
	fs_mkdir(&fs, STRV("pkgs/p1/src"));
	fs_mkdir(&fs, STRV("pkgs/p2"));
	fs_mkdir(&fs, STRV("pkgs/p2/src"));

	proc_t proc = {0};
	proc_init(&proc, 32, 1, ALLOC_STD);

	char storage[256] = {0};
	str_t buf	  = STRB(storage, 0);

	EXPECT_EQ(config_fs(&config, &tmp, &schema, &registry, &fs, &proc, STRV_NULL, STRV_NULL, STRV_NULL, 0, &buf, ALLOC_STD, DST_NONE()),
		  0);

	char out[512]	   = {0};
	char expected[512] = "pkgs += p1\n"
			     "p1:path ?= pkgs" SEP "p1\n"
			     "p1:tgts += p1\n"
			     "p1:p1:type = 2\n"
			     "p1:p1:src = src\n"
			     "p1:p1:inc = include\n"
			     "p1:p1:incs_priv = src\n"
			     "pkgs += p2\n"
			     "p2:path ?= pkgs" SEP "p2\n"
			     "p2:tgts += p2\n"
			     "p2:p2:type = 2\n"
			     "p2:p2:src = src\n"
			     "p2:p2:inc = include\n"
			     "p2:p2:incs_priv = src\n";
	config_print(&config, &schema, &registry, DST_BUF(out));
	EXPECT_STR(out, expected);

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);

	END;
}

TEST(config_fs_ext_sync_queue_oom)
{
	START;

	char repo[301] = {0};
	mem_set(repo, 'r', sizeof(repo) - 1);
	char cur_path[271] = {0};
	mem_set(cur_path, 'p', sizeof(cur_path) - 1);

	char cfg[640] = {0};
	str_t cfg_buf = STRB(cfg, 0);
	str_cat(&cfg_buf, STRV("ext:\n\"https://host/"));
	str_cat(&cfg_buf, STRV(repo));
	str_cat(&cfg_buf, STRV(".git\"\n\n"));

	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 4, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV(cur_path));
	void *f;
	path_t cfg_path = {0};
	path_init(&cfg_path, STRV(cur_path));
	path_push(&cfg_path, STRV("build.cfg"));
	fs_open(&fs, STRVS(cfg_path), "w", &f);
	fs_writes(&fs, f, STRVS(cfg_buf));
	fs_close(&fs, f);

	char storage[1024] = {0};
	str_t buf	   = STRB(storage, 0);

	fail_alloc_ctx_t ctx = {.fail_at = 2};
	alloc_t alloc	     = {.alloc = fail_realloc_alloc, .realloc = fail_realloc_realloc, .free = fail_realloc_free, .priv = &ctx};

	log_set_quiet(0, 1);
	int ret = config_fs(
		&config, &tmp, &schema, &registry, &fs, NULL, STRV_NULL, STRV(cur_path), STRV("proj"), 0, &buf, alloc, DST_NONE());
	log_set_quiet(0, 0);
	int gitignore = fs_isfile(&fs, STRV("tmp/.gitignore"));

	path_t path = {0};
	path_init(&path, STRV("tmp/ext"));
	path_push(&path, STRV(repo));
	int extdir = fs_isdir(&fs, STRVS(path));

	EXPECT_EQ(ret, 1);
	EXPECT_EQ(ctx.cnt, 2);
	EXPECT_EQ(gitignore, 0);
	EXPECT_EQ(extdir, 0);

	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);

	END;
}

TEST(config_fs_ext_sync_clone_cmd)
{
	START;

	char storage[256] = {0};
	str_t cmd	  = STRB(storage, 0);

	config_fs_ext_clone_cmd(STRV("https://github.com/cgware/repo.git"), 0, &cmd);

	EXPECT_STR(storage, "git clone https://github.com/cgware/repo.git tmp" SEP "ext" SEP "repo" SEP "\n");

	END;
}

TEST(config_fs_ext_sync_develop_clone_cmd)
{
	START;

	char storage[256] = {0};
	str_t cmd	  = STRB(storage, 0);

	config_fs_ext_clone_cmd(STRV("https://git.example.com/user/repo.git"), 1, &cmd);

	EXPECT_STR(storage, "git clone git@git.example.com:user/repo.git tmp" SEP "ext" SEP "repo" SEP "\n");

	END;
}

TEST(config_fs_ext_sync_develop_clone_cmd_fallback)
{
	START;

	char storage[256] = {0};
	str_t cmd	  = STRB(storage, 0);

	config_fs_ext_clone_cmd(STRV("https://git.example.com/user/repo.zip"), 1, &cmd);

	EXPECT_STR(storage, "git clone https://git.example.com/user/repo.zip tmp" SEP "ext" SEP "repo" SEP "\n");

	END;
}

TEST(config_fs_ext_sync_develop_clone_cmd_parse_fallback)
{
	START;

	char storage[256] = {0};
	str_t cmd	  = STRB(storage, 0);

	config_fs_ext_clone_cmd(STRV("https://git.example.com.git"), 1, &cmd);

	EXPECT_STR(storage, "git clone https://git.example.com.git tmp" SEP "ext" SEP "git" SEP "\n");

	END;
}

TEST(config_fs_ext_sync)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 4, 1, ALLOC_STD);

	void *f;
	fs_open(&fs, STRV("build.cfg"), "w", &f);
	fs_writes(&fs,
		  f,
		  STRV("ext:\n"
		       "\"https://host/repo.git\"\n"
		       "\n"));
	fs_close(&fs, f);

	proc_t proc = {0};
	proc_init(&proc, 32, 1, ALLOC_STD);

	char storage[256] = {0};
	str_t buf	  = STRB(storage, 0);

	EXPECT_EQ(
		config_fs(
			&config, &tmp, &schema, &registry, &fs, &proc, STRV_NULL, STRV_NULL, STRV("proj"), 0, &buf, ALLOC_STD, DST_NONE()),
		0);
	EXPECT_EQ(fs_isfile(&fs, STRV("tmp/.gitignore")), 1);
	EXPECT_EQ(fs_isdir(&fs, STRV("tmp/ext/repo")), 1);
	EXPECT_EQ(proc.buf.len > 0, 1);

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);

	END;
}

TEST(config_fs_ext_sync_name_ref)
{
	START;

	strv_t name = STRV("really_long_repo_name_that_forces_strbuf_reallocation");

	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 8, 1, ALLOC_STD);

	void *f;
	fs_open(&fs, STRV("build.cfg"), "w", &f);
	fs_writes(&fs, f, STRV("ext:\n\"https://host/really_long_repo_name_that_forces_strbuf_reallocation.git\"\n\n"));
	fs_close(&fs, f);

	path_t path = {0};
	path_init(&path, STRV("tmp"));
	path_push(&path, STRV("ext"));
	path_push(&path, name);
	path_push(&path, STRV("src"));
	fs_mkpath(&fs, STRV_NULL, STRVS(path));

	char storage[256] = {0};
	str_t buf	  = STRB(storage, 0);

	EXPECT_EQ(
		config_fs(&config, &tmp, &schema, &registry, &fs, NULL, STRV_NULL, STRV_NULL, STRV("proj"), 0, &buf, ALLOC_STD, DST_NONE()),
		0);

	char out[2048] = {0};
	config_print(&config, &schema, &registry, DST_BUF(out));
	EXPECT_STR(out,
		   "pkgs += really_long_repo_name_that_forces_strbuf_reallocation\n"
		   "really_long_repo_name_that_forces_strbuf_reallocation:path ?= tmp" SEP "ext" SEP
		   "really_long_repo_name_that_forces_strbuf_reallocation" SEP "\n"
		   "really_long_repo_name_that_forces_strbuf_reallocation:tgts += really_long_repo_name_that_forces_strbuf_reallocation\n"
		   "really_long_repo_name_that_forces_strbuf_reallocation:really_long_repo_name_that_forces_strbuf_reallocation:type = 2\n"
		   "really_long_repo_name_that_forces_strbuf_reallocation:really_long_repo_name_that_forces_strbuf_reallocation:src = src\n"
		   "really_long_repo_name_that_forces_strbuf_reallocation:really_long_repo_name_that_forces_strbuf_reallocation:inc = "
		   "include\n"
		   "really_long_repo_name_that_forces_strbuf_reallocation:really_long_repo_name_that_forces_strbuf_reallocation:incs_priv "
		   "= src\n");

	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);

	END;
}

TEST(config_fs_ext_sync_dir_ref_duplicate)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 4, 1, ALLOC_STD);

	mod_t *mod = find_mod(STRV("mod_pkgs"));
	EXPECT_NE(mod, NULL);
	int (*config_fs_fn)(mod_t *,
			    config_t *,
			    config_t *,
			    const config_schema_t *,
			    registry_t *,
			    config_sync_plan_t *,
			    fs_t *,
			    strv_t,
			    strv_t,
			    strv_t,
			    str_t *,
			    alloc_t,
			    dst_t) = mod->config_fs;
	mod->config_fs		   = ext_duplicate_config_fs;

	char storage[256] = {0};
	str_t buf	  = STRB(storage, 0);

	EXPECT_EQ(
		config_fs(&config, &tmp, &schema, &registry, &fs, NULL, STRV_NULL, STRV_NULL, STRV("proj"), 0, &buf, ALLOC_STD, DST_NONE()),
		0);

	mod->config_fs = config_fs_fn;
	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);

	END;
}

TEST(config_fs_ext_sync_dir_ref_str_oom)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 4, 1, ALLOC_STD);

	mod_t *mod = find_mod(STRV("mod_pkgs"));
	EXPECT_NE(mod, NULL);
	int (*config_fs_fn)(mod_t *,
			    config_t *,
			    config_t *,
			    const config_schema_t *,
			    registry_t *,
			    config_sync_plan_t *,
			    fs_t *,
			    strv_t,
			    strv_t,
			    strv_t,
			    str_t *,
			    alloc_t,
			    dst_t) = mod->config_fs;
	mod->config_fs		   = ext_str_full_config_fs;

	char storage[256] = {0};
	str_t buf	  = STRB(storage, 0);

	fail_alloc_ctx_t ctx = {.fail_at = 1};
	alloc_t alloc	     = {.alloc = fail_realloc_alloc, .realloc = fail_realloc_realloc, .free = fail_realloc_free, .priv = &ctx};

	log_set_quiet(0, 1);
	EXPECT_EQ(config_fs(&config, &tmp, &schema, &registry, &fs, NULL, STRV_NULL, STRV_NULL, STRV_NULL, 0, &buf, alloc, DST_NONE()), 1);
	log_set_quiet(0, 0);

	mod->config_fs = config_fs_fn;
	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);

	END;
}

TEST(config_fs_ext_sync_dir_ref_item_oom)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 4, 1, ALLOC_STD);

	mod_t *mod = find_mod(STRV("mod_pkgs"));
	EXPECT_NE(mod, NULL);
	int (*config_fs_fn)(mod_t *,
			    config_t *,
			    config_t *,
			    const config_schema_t *,
			    registry_t *,
			    config_sync_plan_t *,
			    fs_t *,
			    strv_t,
			    strv_t,
			    strv_t,
			    str_t *,
			    alloc_t,
			    dst_t) = mod->config_fs;
	mod->config_fs		   = ext_fill_config_fs;

	char storage[256] = {0};
	str_t buf	  = STRB(storage, 0);

	fail_alloc_ctx_t ctx = {.fail_at = 1};
	alloc_t alloc	     = {.alloc = fail_realloc_alloc, .realloc = fail_realloc_realloc, .free = fail_realloc_free, .priv = &ctx};

	log_set_quiet(0, 1);
	EXPECT_EQ(config_fs(&config, &tmp, &schema, &registry, &fs, NULL, STRV_NULL, STRV_NULL, STRV_NULL, 0, &buf, alloc, DST_NONE()), 1);
	log_set_quiet(0, 0);

	mod->config_fs = config_fs_fn;
	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);

	END;
}

TEST(config_fs_ext_sync_invalid)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 4, 1, ALLOC_STD);

	void *f;
	fs_open(&fs, STRV("build.cfg"), "w", &f);
	fs_writes(&fs,
		  f,
		  STRV("ext:\n"
		       "\"https://host/repo.git\"\n"
		       "\n"));
	fs_close(&fs, f);

	fs_mkpath(&fs, STRV_NULL, STRV("tmp/ext/repo"));
	fs_open(&fs, STRV("tmp/ext/repo/build.cfg"), "w", &f);
	fs_writes(&fs,
		  f,
		  STRV("ext:\n"
		       "uri = invalid\n"));
	fs_close(&fs, f);

	proc_t proc = {0};
	proc_init(&proc, 32, 1, ALLOC_STD);

	char storage[256] = {0};
	str_t buf	  = STRB(storage, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(
		config_fs(
			&config, &tmp, &schema, &registry, &fs, &proc, STRV_NULL, STRV_NULL, STRV("proj"), 0, &buf, ALLOC_STD, DST_NONE()),
		1);
	log_set_quiet(0, 0);
	EXPECT_EQ(proc.buf.len > 0, 1);

	proc_free(&proc);
	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);

	END;
}

TEST(config_fs_invalid_work_kind)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 16, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);
	mods_init(&schema);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	mod_t *mod = find_mod(STRV("mod_pkgs"));
	EXPECT_NE(mod, NULL);
	int (*config_fs_fn)(mod_t *,
			    config_t *,
			    config_t *,
			    const config_schema_t *,
			    registry_t *,
			    config_sync_plan_t *,
			    fs_t *,
			    strv_t,
			    strv_t,
			    strv_t,
			    str_t *,
			    alloc_t,
			    dst_t) = mod->config_fs;
	mod->config_fs		   = invalid_kind_config_fs;

	char storage[64] = {0};
	str_t buf	 = STRB(storage, 0);

	EXPECT_EQ(
		config_fs(&config, &tmp, &schema, &registry, &fs, NULL, STRV_NULL, STRV_NULL, STRV("proj"), 0, &buf, ALLOC_STD, DST_NONE()),
		1);

	mod->config_fs = config_fs_fn;
	fs_free(&fs);
	config_free(&tmp);
	config_free(&config);
	registry_free(&registry);
	mods_free();
	config_schema_free(&schema);

	END;
}

STEST(config_fs)
{
	SSTART;

	RUN(config_fs_empty);
	RUN(config_sync_plan_helpers);
	RUN(config_fs_pkgs_queue);
	RUN(config_fs_ext_sync_queue_oom);
	RUN(config_fs_ext_sync_clone_cmd);
	RUN(config_fs_ext_sync_develop_clone_cmd);
	RUN(config_fs_ext_sync_develop_clone_cmd_fallback);
	RUN(config_fs_ext_sync_develop_clone_cmd_parse_fallback);
	RUN(config_fs_ext_sync);
	RUN(config_fs_ext_sync_name_ref);
	RUN(config_fs_ext_sync_dir_ref_duplicate);
	RUN(config_fs_ext_sync_dir_ref_str_oom);
	RUN(config_fs_ext_sync_dir_ref_item_oom);
	RUN(config_fs_ext_sync_invalid);
	RUN(config_fs_invalid_work_kind);

	SEND;
}
