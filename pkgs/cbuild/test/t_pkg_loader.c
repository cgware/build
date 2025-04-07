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

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, STRV_NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(pkg_load(0, STRV("test/empty"), &pkgs, ALLOC_STD), 0);

	EXPECT_EQ(pkg->src.len, 0);
	EXPECT_EQ(pkg->inc.len, 0);

	pkgs_free(&pkgs);

	END;
}

TEST(pkg_load_exe)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, STRV("test/exe"), &pkgs, ALLOC_STD), 0);

	EXPECT_EQ(pkg->type, PKG_TYPE_EXE);

	pkgs_free(&pkgs);

	END;
}

TEST(pkg_load_lib)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, STRV("test/lib"), &pkgs, ALLOC_STD), 0);

	EXPECT_EQ(pkg->type, PKG_TYPE_LIB);

	pkgs_free(&pkgs);

	END;
}

TEST(pkg_load_exe_dep_lib)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkg_load(0, STRV("test/exe_dep_lib/pkgs/exe"), &pkgs, ALLOC_STD), 0);

	EXPECT_EQ(pkgs.pkgs.cnt, 2);
	EXPECT_EQ(pkgs.deps.cnt, 1);

	pkgs_free(&pkgs);

	END;
}

TEST(pkg_set_cfg)
{
	START;

	EXPECT_EQ(pkg_set_cfg(0, NULL, CFG_VAR_END, NULL), 1);

	END;
}

TEST(pkg_set_cfg_wrong_dep_type)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root = CFG_ROOT(&cfg);
	cfg_var_t deps = cfg_add_var(&cfg, root, CFG_ARR(&cfg, STRV("deps")));
	cfg_add_var(&cfg, deps, CFG_INT(&cfg, STRV_NULL, 0));

	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_set_cfg(0, &cfg, root, &pkgs), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	pkgs_free(&pkgs);

	END;
}

STEST(pkg_loader)
{
	SSTART;

	RUN(pkg_load_empty);
	RUN(pkg_load_exe);
	RUN(pkg_load_lib);
	RUN(pkg_load_exe_dep_lib);
	RUN(pkg_set_cfg);
	RUN(pkg_set_cfg_wrong_dep_type);

	SEND;
}
