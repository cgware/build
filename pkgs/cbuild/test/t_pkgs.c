#include "pkgs.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(pkgs_init_free)
{
	START;

	pkgs_t pkgs = {0};

	EXPECT_EQ(pkgs_init(NULL, 0, ALLOC_STD), NULL);
	mem_oom(1);
	EXPECT_EQ(pkgs_init(&pkgs, 1, ALLOC_STD), NULL);
	mem_oom(0);
	EXPECT_EQ(pkgs_init(&pkgs, 1, ALLOC_STD), &pkgs);

	pkgs_free(&pkgs);
	pkgs_free(NULL);

	END;
}

TEST(pkgs_add_pkg)
{
	START;

	pkgs_t pkgs = {0};
	log_set_quiet(0, 1);
	pkgs_init(&pkgs, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(pkgs_add_pkg(NULL, STRV_NULL, NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(pkgs_add_pkg(&pkgs, STRV_NULL, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(pkgs_add_pkg(&pkgs, STRV_NULL, NULL), NULL);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_add_pkg_pkgs_oom)
{
	START;

	pkgs_t pkgs = {0};
	log_set_quiet(0, 1);
	pkgs_init(&pkgs, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	strbuf_free(&pkgs.names);
	strbuf_init(&pkgs.names, 1, 8, ALLOC_STD);

	mem_oom(1);
	EXPECT_EQ(pkgs_add_pkg(&pkgs, STRV_NULL, NULL), NULL);
	mem_oom(0);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_add_pkg_deps_oom)
{
	START;

	pkgs_t pkgs = {0};
	log_set_quiet(0, 1);
	pkgs_init(&pkgs, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	strbuf_free(&pkgs.names);
	strbuf_init(&pkgs.names, 1, 8, ALLOC_STD);

	arr_free(&pkgs.pkgs);
	arr_init(&pkgs.pkgs, 1, sizeof(pkg_t), ALLOC_STD);

	mem_oom(1);
	// EXPECT_EQ(pkgs_add_pkg(&pkgs, STRV_NULL, NULL), NULL);
	mem_oom(0);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_add_pkg_exist)
{
	START;

	pkgs_t pkgs = {0};
	log_set_quiet(0, 1);
	pkgs_init(&pkgs, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkgs_add_pkg(&pkgs, STRV(""), NULL), pkg);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_get_pkg_name)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV("pkg"), NULL);

	strv_t name;

	name = pkgs_get_pkg_name(NULL, 0);
	EXPECT_EQ(name.data, NULL);

	name = pkgs_get_pkg_name(&pkgs, 0);
	EXPECT_STRN(name.data, "pkg", name.len);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_get_pkg)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV("pkg"), NULL);

	EXPECT_EQ(pkgs_get_pkg(NULL, 0), NULL)
	EXPECT_EQ(pkgs_get_pkg(&pkgs, 0), pkg);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_add_dep)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	EXPECT_EQ(pkgs_add_dep(NULL, 0, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(pkgs_add_dep(&pkgs, ARR_END, STRV_NULL), 1);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(pkgs_add_dep(&pkgs, 0, STRV_NULL), 1);
	mem_oom(0);
	EXPECT_EQ(pkgs_add_dep(&pkgs, 0, STRV("")), 0);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_add_dep_oom)
{
	START;

	pkgs_t pkgs = {0};
	log_set_quiet(0, 1);
	pkgs_init(&pkgs, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	mem_oom(1);
	EXPECT_EQ(pkgs_add_dep(&pkgs, 0, STRV("")), 1);
	mem_oom(0);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_get_pkg_deps)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV("0"), NULL);

	pkgs_add_dep(&pkgs, 0, STRV("01"));
	pkgs_add_dep(&pkgs, 0, STRV("02"));

	pkgs_add_dep(&pkgs, 1, STRV("013"));
	pkgs_add_dep(&pkgs, 1, STRV("014"));

	pkgs_add_dep(&pkgs, 2, STRV("014"));
	pkgs_add_dep(&pkgs, 2, STRV("025"));

	arr_t deps = {0};
	log_set_quiet(0, 1);
	arr_init(&deps, 0, sizeof(uint), ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(pkgs_get_pkg_deps(NULL, 0, NULL), 1);
	mem_oom(1);
	EXPECT_EQ(pkgs_get_pkg_deps(&pkgs, 0, &deps), 1);
	mem_oom(0);
	EXPECT_EQ(pkgs_get_pkg_deps(&pkgs, 0, &deps), 0);

	EXPECT_EQ(deps.cnt, 5);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), 3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), 4);
	EXPECT_EQ(*(uint *)arr_get(&deps, 2), 1);
	EXPECT_EQ(*(uint *)arr_get(&deps, 3), 5);
	EXPECT_EQ(*(uint *)arr_get(&deps, 4), 2);

	arr_free(&deps);
	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_get_build_order)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV("0"), NULL);
	pkgs_add_dep(&pkgs, 0, STRV("01"));
	pkgs_add_pkg(&pkgs, STRV("1"), NULL);

	arr_t deps = {0};
	log_set_quiet(0, 1);
	arr_init(&deps, 0, sizeof(uint), ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(pkgs_get_build_order(NULL, NULL), 1);
	mem_oom(1);
	EXPECT_EQ(pkgs_get_build_order(&pkgs, &deps), 1);
	mem_oom(0);
	EXPECT_EQ(pkgs_get_build_order(&pkgs, &deps), 0);

	EXPECT_EQ(deps.cnt, 3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), 1);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), 0);
	EXPECT_EQ(*(uint *)arr_get(&deps, 2), 2);

	arr_free(&deps);
	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_print)
{
	START;

	pkgs_t pkgs = {0};
	log_set_quiet(0, 1);
	pkgs_init(&pkgs, 1, ALLOC_STD);
	log_set_quiet(0, 0);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);
	pkgs_add_dep(&pkgs, 0, STRV("dep"));

	char buf[256] = {0};
	EXPECT_EQ(pkgs_print(&pkgs, PRINT_DST_BUF(buf, sizeof(buf), 0)), 102);
	EXPECT_STR(buf,
		   "[package]\n"
		   "TYPE: UNKNOWN\n"
		   "DIR: \n"
		   "SRC: \n"
		   "INC: \n"
		   "DEPS: dep\n"
		   "\n"
		   "[package]\n"
		   "TYPE: UNKNOWN\n"
		   "DIR: \n"
		   "SRC: \n"
		   "INC: \n"
		   "DEPS:\n"
		   "\n");

	pkgs_free(&pkgs);

	END;
}

STEST(pkgs)
{
	SSTART;

	RUN(pkgs_init_free);
	RUN(pkgs_add_pkg);
	RUN(pkgs_add_pkg_pkgs_oom);
	RUN(pkgs_add_pkg_deps_oom);
	RUN(pkgs_add_pkg_exist);
	RUN(pkgs_get_pkg_name);
	RUN(pkgs_get_pkg);
	RUN(pkgs_add_dep);
	RUN(pkgs_add_dep_oom);
	RUN(pkgs_get_pkg_deps);
	RUN(pkgs_get_build_order);
	RUN(pkgs_print);

	SEND;
}
