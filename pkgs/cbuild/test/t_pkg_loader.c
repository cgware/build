#include "pkg_loader.h"

#include "log.h"
#include "mem.h"
#include "pkgs.h"
#include "test.h"

TEST(pkg_load_empty)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	EXPECT_EQ(pkg_load(&fs, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD, NULL), NULL);
	EXPECT_NE(pkg_load(&fs, STRV_NULL, STRV_NULL, STRV_NULL, &pkgs, ALLOC_STD, NULL), NULL);

	pkg_t *pkg = pkgs_get(&pkgs, 0);

	strv_t dir  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_DIR]);
	strv_t name = strvbuf_get(&pkgs.strs, pkg->strs[PKG_NAME]);
	strv_t src  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_SRC]);
	strv_t inc  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_INC]);
	EXPECT_EQ(dir.len, 0);
	EXPECT_EQ(name.len, 0);
	EXPECT_EQ(src.len, 0);
	EXPECT_EQ(inc.len, 0);
	EXPECT_EQ(pkgs.targets.targets.cnt, 1);

	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_empty_cfg)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	fs_mkfile(&fs, STRV("pkg.cfg"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	EXPECT_NE(pkg_load(&fs, STRV_NULL, STRV_NULL, STRV_NULL, &pkgs, ALLOC_STD, &tmp), NULL);

	pkg_t *pkg = pkgs_get(&pkgs, 0);

	strv_t dir  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_DIR]);
	strv_t name = strvbuf_get(&pkgs.strs, pkg->strs[PKG_NAME]);
	strv_t src  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_SRC]);
	strv_t inc  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_INC]);
	EXPECT_EQ(dir.len, 0);
	EXPECT_EQ(name.len, 0);
	EXPECT_EQ(src.len, 0);
	EXPECT_EQ(inc.len, 0);
	EXPECT_EQ(pkgs.targets.targets.cnt, 1);

	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_name)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	void *f;
	fs_open(&fs, STRV("pkg.cfg"), "w", &f);
	fs_write(&fs, f, STRV("name = pkg\n"));
	fs_close(&fs, f);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	EXPECT_NE(pkg_load(&fs, STRV_NULL, STRV_NULL, STRV_NULL, &pkgs, ALLOC_STD, &tmp), NULL);

	pkg_t *pkg = pkgs_get(&pkgs, 0);

	strv_t name = strvbuf_get(&pkgs.strs, pkg->strs[PKG_NAME]);
	EXPECT_STRN(name.data, "pkg", name.len);

	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_exists)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	void *f;
	fs_open(&fs, STRV("pkg.cfg"), "w", &f);
	fs_write(&fs, f, STRV("name = pkg\n"));
	fs_close(&fs, f);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);
	strvbuf_set(&pkgs.strs, pkg->strs[PKG_NAME], STRV("pkg"));
	pkg->loaded = 1;

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_load(&fs, STRV_NULL, STRV_NULL, STRV_NULL, &pkgs, ALLOC_STD, &tmp), NULL);
	log_set_quiet(0, 0);

	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg)
{
	START;

	EXPECT_EQ(pkg_set_cfg(NULL, NULL, 0, NULL, STRV_NULL, NULL), 1);

	END;
}

TEST(pkg_set_cfg_src_not_found)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, src;

	cfg_root(&cfg, &root);
	cfg_str(&cfg, STRV("src"), STRV("src"), &src);
	cfg_add_var(&cfg, root, src);

	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_inc_not_found)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, inc;

	cfg_root(&cfg, &root);
	cfg_str(&cfg, STRV("include"), STRV("include"), &inc);
	cfg_add_var(&cfg, root, inc);

	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_invalid_id)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_set_cfg(NULL, &cfg, -1, &pkgs, STRV_NULL, NULL), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	pkgs_free(&pkgs);

	END;
}

TEST(pkg_set_cfg_target_oom)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root;

	cfg_root(&cfg, &root);

	mem_oom(1);
	pkgs.targets.targets.cnt = pkgs.targets.targets.cap;
	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 1);
	pkgs.targets.targets.cnt = 0;
	mem_oom(0);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_target_type_exe)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cfg_target;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("target"), &cfg_target);
	cfg_add_var(&cfg, root, cfg_target);

	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 0);

	strv_t src = strvbuf_get(&pkgs.strs, pkg->strs[PKG_SRC]);
	EXPECT_STRN(src.data, "src", src.len);

	target_t *target = targets_get(&pkgs.targets, pkg->targets);
	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_target_type_lib)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("include"));

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cfg_target;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("target"), &cfg_target);
	cfg_add_var(&cfg, root, cfg_target);

	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 0);

	strv_t inc = strvbuf_get(&pkgs.strs, pkg->strs[PKG_INC]);
	EXPECT_STRN(inc.data, "include", inc.len);

	target_t *target = targets_get(&pkgs.targets, pkg->targets);
	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_target_cfg_oom)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, targets;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("target"), &targets);
	cfg_add_var(&cfg, root, targets);

	mem_oom(1);
	pkgs.targets.targets.cnt = pkgs.targets.targets.cap;
	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 1);
	pkgs.targets.targets.cnt = 0;
	mem_oom(0);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_target_cfg_type_exe)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cfg_target, type;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("target"), &cfg_target);
	cfg_add_var(&cfg, root, cfg_target);
	cfg_lit(&cfg, STRV("type"), STRV("EXE"), &type);
	cfg_add_var(&cfg, cfg_target, type);

	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 0);

	target_t *target = targets_get(&pkgs.targets, pkg->targets);
	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_target_cfg_type_lib)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cfg_target, type;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("target"), &cfg_target);
	cfg_add_var(&cfg, root, cfg_target);
	cfg_lit(&cfg, STRV("type"), STRV("LIB"), &type);
	cfg_add_var(&cfg, cfg_target, type);

	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 0);

	target_t *target = targets_get(&pkgs.targets, pkg->targets);
	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_target_cfg_type_invalid)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cfg_target, type;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("target"), &cfg_target);
	cfg_add_var(&cfg, root, cfg_target);
	cfg_lit(&cfg, STRV("type"), STRV("A"), &type);
	cfg_add_var(&cfg, cfg_target, type);

	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 1);
	log_set_quiet(0, 0);

	target_t *target = targets_get(&pkgs.targets, pkg->targets);
	EXPECT_EQ(target->type, TARGET_TYPE_UNKNOWN);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_exe_dep_lib)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	fs_mkdir(&fs, STRV("src"));

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, deps, lib;

	cfg_root(&cfg, &root);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, root, deps);
	cfg_lit(&cfg, STRV_NULL, STRV("lib"), &lib);
	cfg_add_var(&cfg, deps, lib);

	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 0);

	EXPECT_EQ(pkgs.targets.targets.cnt, 2);
	EXPECT_EQ(pkgs.targets.deps.cnt, 1);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_wrong_dep_type)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, deps, in;

	cfg_root(&cfg, &root);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, root, deps);
	cfg_int(&cfg, STRV_NULL, 0, &in);
	cfg_add_var(&cfg, deps, in);

	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg_dep_oom)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, deps, lit;

	cfg_root(&cfg, &root);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, root, deps);
	cfg_lit(&cfg, STRV_NULL, STRV("lib"), &lit);
	cfg_add_var(&cfg, deps, lit);

	pkgs.targets.targets.cnt = pkgs.targets.targets.cap;

	mem_oom(1);
	EXPECT_EQ(pkg_set_cfg(pkg, &cfg, root, &pkgs, STRV_NULL, &fs), 1);
	mem_oom(0);

	cfg_free(&cfg);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

STEST(pkg_loader)
{
	SSTART;

	RUN(pkg_load_empty);
	RUN(pkg_load_empty_cfg);
	RUN(pkg_load_name);
	RUN(pkg_load_exists);
	RUN(pkg_set_cfg);
	RUN(pkg_set_cfg_src_not_found);
	RUN(pkg_set_cfg_inc_not_found);
	RUN(pkg_set_cfg_invalid_id);
	RUN(pkg_set_cfg_target_oom);
	RUN(pkg_set_cfg_target_type_exe);
	RUN(pkg_set_cfg_target_type_lib);
	RUN(pkg_set_cfg_target_cfg_oom);
	RUN(pkg_set_cfg_target_cfg_type_exe);
	RUN(pkg_set_cfg_target_cfg_type_lib);
	RUN(pkg_set_cfg_target_cfg_type_invalid);
	RUN(pkg_set_cfg_exe_dep_lib);
	RUN(pkg_set_cfg_wrong_dep_type);
	RUN(pkg_set_cfg_dep_oom);

	SEND;
}
