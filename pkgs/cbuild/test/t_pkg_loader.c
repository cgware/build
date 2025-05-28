#include "pkg_loader.h"

#include "log.h"
#include "mem.h"
#include "pkgs.h"
#include "test.h"

TEST(pkg_load_empty)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, &fs, STRV_NULL, NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(pkg_load(0, &fs, STRV("test/empty"), &pkgs, &targets, ALLOC_STD), 0);

	strv_t dir = strvbuf_get(&pkgs.strs, pkg->dir);
	strv_t src = strvbuf_get(&pkgs.strs, pkg->src);
	strv_t inc = strvbuf_get(&pkgs.strs, pkg->inc);
	EXPECT_STRN(dir.data, "test/empty", dir.len);
	EXPECT_EQ(src.len, 0);
	EXPECT_EQ(inc.len, 0);
	EXPECT_EQ(targets.targets.cnt, 0);

	targets_free(&targets);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_empty_cfg)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, &fs, STRV_NULL, NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(pkg_load(0, &fs, STRV("test/empty_cfg"), &pkgs, &targets, ALLOC_STD), 0);

	strv_t dir = strvbuf_get(&pkgs.strs, pkg->dir);
	strv_t src = strvbuf_get(&pkgs.strs, pkg->src);
	strv_t inc = strvbuf_get(&pkgs.strs, pkg->inc);
	EXPECT_STRN(dir.data, "test/empty_cfg", dir.len);
	EXPECT_EQ(src.len, 0);
	EXPECT_EQ(inc.len, 0);
	EXPECT_EQ(targets.targets.cnt, 0);

	targets_free(&targets);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_exe)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, &fs, STRV("test/exe"), &pkgs, &targets, ALLOC_STD), 0);

	strv_t src = strvbuf_get(&pkgs.strs, pkg->src);
	EXPECT_STRN(src.data, "test/exe" SEP "src", src.len);
	EXPECT_EQ(targets.targets.cnt, 1);

	target_t *target = targets_get(&targets, pkg->targets);

	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	targets_free(&targets);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_lib)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	mem_oom(1);
	targets.targets.cnt = targets.targets.cap;
	EXPECT_EQ(pkg_load(0, &fs, STRV("test/lib"), &pkgs, &targets, ALLOC_STD), 1);
	targets.targets.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(pkg_load(0, &fs, STRV("test/lib"), &pkgs, &targets, ALLOC_STD), 0);

	strv_t inc = strvbuf_get(&pkgs.strs, pkg->inc);
	EXPECT_STRN(inc.data, "test/lib" SEP "include", inc.len);
	EXPECT_EQ(targets.targets.cnt, 1);

	target_t *target = targets_get(&targets, pkg->targets);

	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	targets_free(&targets);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_load_exe_dep_lib)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, &fs, STRV("test/exe_dep_lib/pkgs/exe"), &pkgs, &targets, ALLOC_STD), 0);

	EXPECT_EQ(targets.targets.cnt, 2);
	EXPECT_EQ(targets.deps.cnt, 1);

	targets_free(&targets);
	pkgs_free(&pkgs);
	fs_free(&fs);

	END;
}

TEST(pkg_set_cfg)
{
	START;

	EXPECT_EQ(pkg_set_cfg(0, NULL, 0, NULL, NULL), 1);

	END;
}

TEST(pkg_set_cfg_invalid_id)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_set_cfg(-1, &cfg, -1, &pkgs, NULL), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	pkgs_free(&pkgs);

	END;
}

TEST(pkg_set_cfg_target_oom)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root;

	cfg_root(&cfg, &root);

	mem_oom(1);
	EXPECT_EQ(pkg_set_cfg(0, &cfg, root, &pkgs, &targets), 1);
	mem_oom(0);

	cfg_free(&cfg);
	targets_free(&targets);
	pkgs_free(&pkgs);

	END;
}

TEST(pkg_set_cfg_wrong_dep_type)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, deps, in;

	cfg_root(&cfg, &root);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, root, deps);
	cfg_int(&cfg, STRV_NULL, 0, &in);
	cfg_add_var(&cfg, deps, in);

	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_set_cfg(0, &cfg, root, &pkgs, &targets), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	targets_free(&targets);
	pkgs_free(&pkgs);

	END;
}

TEST(pkg_set_cfg_dep_oom)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, deps, lit;

	cfg_root(&cfg, &root);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, root, deps);
	cfg_lit(&cfg, STRV_NULL, STRV("lib"), &lit);
	cfg_add_var(&cfg, deps, lit);

	mem_oom(1);
	EXPECT_EQ(pkg_set_cfg(0, &cfg, root, &pkgs, &targets), 1);
	mem_oom(0);

	cfg_free(&cfg);
	targets_free(&targets);
	pkgs_free(&pkgs);

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
