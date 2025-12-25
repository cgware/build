#include "config_cfg.h"

#include "log.h"
#include "path.h"
#include "test.h"

TEST(config_cfg_empty)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root;
	cfg_root(&cfg, &root);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(NULL, NULL, cfg.vars.cnt, NULL, NULL, STRV_NULL, config.dirs.cnt, NULL, ALLOC_STD, DST_NONE()), 1);
	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_NONE()), 0);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_deps)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, deps, var;
	cfg_root(&cfg, &root);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, root, deps);
	cfg_lit(&cfg, STRV_NULL, STRV("dep"), &var);
	cfg_add_var(&cfg, deps, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_NONE()), 0);

	const config_pkg_t *pkg = config_get_pkg(&config, 0);

	uint *dep_id = list_get_at(&config.deps, pkg->deps, 0, NULL);
	strv_t dep   = config_get_str(&config, *dep_id);
	EXPECT_STRN(dep.data, "dep", dep.len);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_deps_invalid)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, deps, var;
	cfg_root(&cfg, &root);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, root, deps);
	cfg_str(&cfg, STRV_NULL, STRV("dep"), &var);
	cfg_add_var(&cfg, deps, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_NONE()), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_uri)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var;
	cfg_root(&cfg, &root);
	cfg_str(&cfg, STRV("uri"), STRV("uri"), &var);
	cfg_add_var(&cfg, root, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_NONE()), 0);

	const config_pkg_t *pkg = config_get_pkg(&config, 0);

	strv_t uri = config_get_str(&config, pkg->strs + CONFIG_PKG_URI);
	EXPECT_STRN(uri.data, "uri", uri.len);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_uri_invalid)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var;
	cfg_root(&cfg, &root);
	cfg_lit(&cfg, STRV("uri"), STRV("uri"), &var);
	cfg_add_var(&cfg, root, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_NONE()), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_inc)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var;
	cfg_root(&cfg, &root);
	cfg_str(&cfg, STRV("include"), STRV("include"), &var);
	cfg_add_var(&cfg, root, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_NONE()), 0);

	const config_pkg_t *pkg = config_get_pkg(&config, 0);

	strv_t uri = config_get_str(&config, pkg->strs + CONFIG_PKG_INC);
	EXPECT_STRN(uri.data, "include", uri.len);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_inc_invalid)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var;
	cfg_root(&cfg, &root);
	cfg_lit(&cfg, STRV("include"), STRV("include"), &var);
	cfg_add_var(&cfg, root, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_NONE()), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_pkg)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, tbl, deps, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &tbl);
	cfg_add_var(&cfg, root, tbl);
	cfg_tbl(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, tbl, deps);
	cfg_lit(&cfg, STRV_NULL, STRV("dep"), &var);
	cfg_add_var(&cfg, deps, var);
	cfg_str(&cfg, STRV("uri"), STRV("uri"), &var);
	cfg_add_var(&cfg, tbl, var);
	cfg_str(&cfg, STRV("include"), STRV("include"), &var);
	cfg_add_var(&cfg, tbl, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_NONE()), 0);

	const config_pkg_t *pkg = config_get_pkg(&config, 0);

	uint *dep_id = list_get_at(&config.deps, pkg->deps, 0, NULL);
	strv_t val;

	val = config_get_str(&config, *dep_id);
	EXPECT_STRN(val.data, "dep", val.len);
	val = config_get_str(&config, pkg->strs + CONFIG_PKG_URI);
	EXPECT_STRN(val.data, "uri", val.len);
	val = config_get_str(&config, pkg->strs + CONFIG_PKG_INC);
	EXPECT_STRN(val.data, "include", val.len);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_target)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, tbl, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("target"), &tbl);
	cfg_add_var(&cfg, root, tbl);
	cfg_str(&cfg, STRV("cmd"), STRV("cmd"), &var);
	cfg_add_var(&cfg, tbl, var);
	cfg_str(&cfg, STRV("out"), STRV("out"), &var);
	cfg_add_var(&cfg, tbl, var);
	cfg_str(&cfg, STRV("dst"), STRV("dst"), &var);
	cfg_add_var(&cfg, tbl, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_NONE()), 0);

	const config_target_t *target = config_get_target(&config, 0);

	strv_t val;

	val = config_get_str(&config, target->strs + CONFIG_TARGET_PREP);
	EXPECT_STRN(val.data, "prep", val.len);
	val = config_get_str(&config, target->strs + CONFIG_TARGET_CONF);
	EXPECT_STRN(val.data, "conf", val.len);
	val = config_get_str(&config, target->strs + CONFIG_TARGET_COMP);
	EXPECT_STRN(val.data, "comp", val.len);
	val = config_get_str(&config, target->strs + CONFIG_TARGET_INST);
	EXPECT_STRN(val.data, "inst", val.len);
	val = config_get_str(&config, target->strs + CONFIG_TARGET_DST);
	EXPECT_STRN(val.data, "dst", val.len);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_ext)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, tbl, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &tbl);
	cfg_add_var(&cfg, root, tbl);
	cfg_str(&cfg, STRV_NULL, STRV("repo"), &var);
	cfg_add_var(&cfg, tbl, var);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	proc_t proc = {0};
	proc_init(&proc, 32, 1);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(config_cfg(&config, &cfg, root, &fs, &proc, STRV_NULL, dir, &buf, ALLOC_STD, DST_STD()), 0);

	EXPECT_EQ(fs_isfile(&fs, STRV("tmp/.gitignore")), 1);
	EXPECT_EQ(fs_isdir(&fs, STRV("tmp/ext/repo")), 1);
	EXPECT_STRN(proc.buf.data, "git clone repo tmp" SEP "ext" SEP "repo" SEP "\n", proc.buf.len);

	proc_free(&proc);
	fs_free(&fs);
	cfg_free(&cfg);
	config_free(&config);

	END;
}

TEST(config_cfg_ext_invalid)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, tbl, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &tbl);
	cfg_add_var(&cfg, root, tbl);
	cfg_lit(&cfg, STRV_NULL, STRV("repo"), &var);
	cfg_add_var(&cfg, tbl, var);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_cfg(&config, &cfg, root, NULL, NULL, STRV_NULL, dir, &buf, ALLOC_STD, DST_STD()), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	config_free(&config);

	END;
}

STEST(config_cfg)
{
	SSTART;

	RUN(config_cfg_empty);
	RUN(config_cfg_deps);
	RUN(config_cfg_deps_invalid);
	RUN(config_cfg_uri);
	RUN(config_cfg_uri_invalid);
	RUN(config_cfg_inc);
	RUN(config_cfg_inc_invalid);
	RUN(config_cfg_pkg);
	RUN(config_cfg_target);
	RUN(config_cfg_ext);
	RUN(config_cfg_ext_invalid);

	SEND;
}
