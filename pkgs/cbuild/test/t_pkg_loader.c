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

	fs_mkdir(&fs, STRV("empty"));

	EXPECT_EQ(pkg_load(&fs, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(pkg_load(&fs, STRV_NULL, STRV("empty"), &pkgs, ALLOC_STD), 0);

	pkg_t *pkg = pkgs_get(&pkgs, 0);

	strv_t dir  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_DIR]);
	strv_t name = strvbuf_get(&pkgs.strs, pkg->strs[PKG_NAME]);
	strv_t src  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_SRC]);
	strv_t inc  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_INC]);
	EXPECT_STRN(dir.data, "empty", dir.len);
	EXPECT_STRN(name.data, "empty", name.len);
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
	fs_init(&fs, 2, 1, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("empty_cfg"));
	fs_mkfile(&fs, STRV("empty_cfg/pkg.cfg"));

	EXPECT_EQ(pkg_load(&fs, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(pkg_load(&fs, STRV_NULL, STRV("empty_cfg"), &pkgs, ALLOC_STD), 0);

	pkg_t *pkg = pkgs_get(&pkgs, 0);

	strv_t dir  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_DIR]);
	strv_t name = strvbuf_get(&pkgs.strs, pkg->strs[PKG_NAME]);
	strv_t src  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_SRC]);
	strv_t inc  = strvbuf_get(&pkgs.strs, pkg->strs[PKG_INC]);
	EXPECT_STRN(dir.data, "empty_cfg", dir.len);
	EXPECT_STRN(name.data, "empty_cfg", name.len);
	EXPECT_EQ(src.len, 0);
	EXPECT_EQ(inc.len, 0);
	EXPECT_EQ(pkgs.targets.targets.cnt, 1);

	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_exe)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	EXPECT_EQ(pkg_load(&fs, STRV("test"), STRV("exe"), &pkgs, ALLOC_STD), 0);

	pkg_t *pkg = pkgs_get(&pkgs, 0);

	strv_t src = strvbuf_get(&pkgs.strs, pkg->strs[PKG_SRC]);
	EXPECT_STRN(src.data, "test/exe" SEP "src", src.len);
	EXPECT_EQ(pkgs.targets.targets.cnt, 1);

	target_t *target = targets_get(&pkgs.targets, pkg->targets);

	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_lib)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	mem_oom(1);
	pkgs.targets.targets.cnt = pkgs.targets.targets.cap;
	EXPECT_EQ(pkg_load(&fs, STRV("test"), STRV("lib"), &pkgs, ALLOC_STD), 1);
	pkgs.targets.targets.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(pkg_load(&fs, STRV("test"), STRV("lib"), &pkgs, ALLOC_STD), 0);

	pkg_t *pkg = pkgs_get(&pkgs, 0);

	strv_t inc = strvbuf_get(&pkgs.strs, pkg->strs[PKG_INC]);
	EXPECT_STRN(inc.data, "test/lib" SEP "include", inc.len);
	EXPECT_EQ(pkgs.targets.targets.cnt, 1);

	target_t *target = targets_get(&pkgs.targets, pkg->targets);

	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_exe_dep_lib)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	EXPECT_EQ(pkg_load(&fs, STRV("test/exe_dep_lib"), STRV("pkgs/exe"), &pkgs, ALLOC_STD), 0);

	EXPECT_EQ(pkgs.targets.targets.cnt, 2);
	EXPECT_EQ(pkgs.targets.deps.cnt, 1);

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
	RUN(pkg_load_exe);
	RUN(pkg_load_lib);
	RUN(pkg_load_exe_dep_lib);
	RUN(pkg_set_cfg);
	RUN(pkg_set_cfg_invalid_id);
	RUN(pkg_set_cfg_target_oom);
	RUN(pkg_set_cfg_wrong_dep_type);
	RUN(pkg_set_cfg_dep_oom);

	SEND;
}
