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

TEST(pkgs_add_pkg_exist)
{
	START;

	pkgs_t pkgs = {0};
	log_set_quiet(0, 1);
	pkgs_init(&pkgs, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	pkg_t *pkg = pkgs_add_pkg(&pkgs, STRV(""), NULL);

	uint id;
	EXPECT_EQ(pkgs_add_pkg(&pkgs, STRV(""), &id), pkg);
	EXPECT_EQ(id, 0);

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

TEST(pkgs_get_build_order)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_t *pkg;
	list_node_t target;

	pkg = pkgs_add_pkg(&pkgs, STRV("pkg0"), NULL);

	arr_t deps = {0};
	log_set_quiet(0, 1);
	arr_init(&deps, 0, sizeof(uint), ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(pkgs_get_build_order(&pkgs, &targets, &deps), 0);

	pkg_add_target(pkg, &targets, STRV("target0"), &target);

	targets_add_dep(&targets, target, STRV("target1"));

	pkg = pkgs_add_pkg(&pkgs, STRV("pkg1"), NULL);
	pkg_add_target(pkg, &targets, STRV("target1"), &target);

	pkg = pkgs_add_pkg(&pkgs, STRV("pkg2"), NULL);
	pkg_add_target(pkg, &targets, STRV("target2"), &target);

	deps.cnt = 0;

	EXPECT_EQ(pkgs_get_build_order(NULL, NULL, NULL), 1);
	mem_oom(1);
	EXPECT_EQ(pkgs_get_build_order(&pkgs, &targets, &deps), 1);
	mem_oom(0);
	EXPECT_EQ(pkgs_get_build_order(&pkgs, &targets, &deps), 0);

	EXPECT_EQ(deps.cnt, 3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), 1);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), 0);
	EXPECT_EQ(*(uint *)arr_get(&deps, 2), 2);

	arr_free(&deps);
	targets_free(&targets);
	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_print)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkgs_add_pkg(&pkgs, STRV(""), NULL);

	char buf[256] = {0};
	EXPECT_EQ(pkgs_print(&pkgs, &targets, DST_BUF(buf)), 35);
	EXPECT_STR(buf,
		   "[package]\n"
		   "ID: 0\n"
		   "DIR: \n"
		   "SRC: \n"
		   "INC: \n"
		   "\n");

	targets_free(&targets);
	pkgs_free(&pkgs);

	END;
}

STEST(pkgs)
{
	SSTART;

	RUN(pkgs_init_free);
	RUN(pkgs_add_pkg);
	RUN(pkgs_add_pkg_pkgs_oom);
	RUN(pkgs_add_pkg_exist);
	RUN(pkgs_get_pkg_name);
	RUN(pkgs_get_pkg);
	RUN(pkgs_get_build_order);
	RUN(pkgs_print);

	SEND;
}
