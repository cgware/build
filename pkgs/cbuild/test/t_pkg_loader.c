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

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, STRV_NULL, NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(pkg_load(0, STRV("test/empty"), &pkgs, &targets, ALLOC_STD), 0);

	EXPECT_STRN(pkg->dir.data, "test/empty", pkg->dir.len);
	EXPECT_EQ(pkg->src.len, 0);
	EXPECT_EQ(pkg->inc.len, 0);
	EXPECT_EQ(targets.targets.cnt, 0);

	targets_free(&targets);
	pkgs_free(&pkgs);

	END;
}

TEST(pkg_load_empty_cfg)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, STRV_NULL, NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(pkg_load(0, STRV("test/empty_cfg"), &pkgs, &targets, ALLOC_STD), 0);

	EXPECT_STRN(pkg->dir.data, "test/empty_cfg", pkg->dir.len);
	EXPECT_EQ(pkg->src.len, 0);
	EXPECT_EQ(pkg->inc.len, 0);
	EXPECT_EQ(targets.targets.cnt, 0);

	targets_free(&targets);
	pkgs_free(&pkgs);

	END;
}

TEST(pkg_load_exe)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, STRV("test/exe"), &pkgs, &targets, ALLOC_STD), 0);

	EXPECT_STRN(pkg->src.data, "test/exe/src", pkg->src.len);
	EXPECT_EQ(targets.targets.cnt, 0);
	EXPECT_NEB(pkg->is_target, 0);

	target_t *target = targets_get(&targets, pkg->targets);

	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	targets_free(&targets);
	pkgs_free(&pkgs);

	END;
}

TEST(pkg_load_lib)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, STRV("test/lib"), &pkgs, &targets, ALLOC_STD), 0);

	EXPECT_STRN(pkg->inc.data, "test/lib/include", pkg->inc.len);
	EXPECT_EQ(targets.targets.cnt, 0);
	EXPECT_NEB(pkg->is_target, 0);

	target_t *target = targets_get(&targets, pkg->targets);

	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	targets_free(&targets);
	pkgs_free(&pkgs);

	END;
}

TEST(pkg_load_exe_dep_lib)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, STRV("test/exe_dep_lib/pkgs/exe"), &pkgs, &targets, ALLOC_STD), 0);

	EXPECT_EQ(targets.targets.cnt, 2);
	EXPECT_EQ(targets.deps.cnt, 1);

	targets_free(&targets);
	pkgs_free(&pkgs);

	END;
}

TEST(pkg_set_cfg)
{
	START;

	EXPECT_EQ(pkg_set_cfg(0, NULL, CFG_VAR_END, NULL, NULL), 1);

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

	cfg_var_t root = CFG_ROOT(&cfg);
	cfg_var_t deps = cfg_add_var(&cfg, root, CFG_ARR(&cfg, STRV("deps")));
	cfg_add_var(&cfg, deps, CFG_INT(&cfg, STRV_NULL, 0));

	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_set_cfg(0, &cfg, root, &pkgs, &targets), 1);
	log_set_quiet(0, 0);

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
	RUN(pkg_set_cfg_wrong_dep_type);

	SEND;
}
