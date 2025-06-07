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

TEST(pkgs_add)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add(&pkgs, NULL);

	EXPECT_EQ(pkgs_add(NULL, NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(pkgs_add(&pkgs, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(pkgs_add(&pkgs, NULL), NULL);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_add_strs_oom)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	mem_oom(1);
	pkgs.strs.used = pkgs.strs.size - sizeof(size_t) * 0;
	EXPECT_EQ(pkgs_add(&pkgs, NULL), NULL);
	pkgs.strs.used = pkgs.strs.size - sizeof(size_t) * 1;
	EXPECT_EQ(pkgs_add(&pkgs, NULL), NULL);
	pkgs.strs.used = pkgs.strs.size - sizeof(size_t) * 2;
	EXPECT_EQ(pkgs_add(&pkgs, NULL), NULL);
	pkgs.strs.used = pkgs.strs.size - sizeof(size_t) * 3;
	EXPECT_EQ(pkgs_add(&pkgs, NULL), NULL);
	mem_oom(0);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_set_str)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	EXPECT_EQ(pkgs_set_str(NULL, 0, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(pkgs_set_str(&pkgs, 0, STRV_NULL), 1);
	log_set_quiet(0, 0);

	pkgs_add(&pkgs, NULL);
	pkgs_add(&pkgs, NULL);

	EXPECT_EQ(pkgs_set_str(&pkgs, 0, STRV("")), 0);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_set_uri)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	EXPECT_EQ(pkgs_set_uri(NULL, NULL, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(pkgs_set_uri(&pkgs, pkg, STRV_NULL), 1);
	EXPECT_EQ(pkgs_set_uri(&pkgs, pkg, STRV("invalid")), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(pkgs_set_uri(&pkgs, pkg, STRV("git:repo")), 0);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_get_name)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);
	pkgs_set_str(&pkgs, pkg->strs[PKG_NAME], STRV("pkg"));

	strv_t name;

	name = pkgs_get_name(NULL, 0);
	EXPECT_EQ(name.data, NULL);

	log_set_quiet(0, 1);
	name = pkgs_get_name(&pkgs, pkgs.pkgs.cnt);
	EXPECT_EQ(name.data, NULL);
	log_set_quiet(0, 0);

	name = pkgs_get_name(&pkgs, 0);
	EXPECT_STRN(name.data, "pkg", name.len);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_get)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg = pkgs_add(&pkgs, NULL);

	EXPECT_EQ(pkgs_get(NULL, 0), NULL)
	EXPECT_EQ(pkgs_get(&pkgs, 0), pkg);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_find)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	uint pkg, found;
	pkg_t *data = pkgs_add(&pkgs, &pkg);

	EXPECT_EQ(pkgs_find(NULL, STRV_NULL, NULL), NULL)
	EXPECT_EQ(pkgs_find(&pkgs, STRV_NULL, NULL), NULL);
	EXPECT_EQ(pkgs_find(&pkgs, STRV("asd"), NULL), NULL);
	EXPECT_EQ(pkgs_find(&pkgs, STRV(""), &found), data);
	EXPECT_EQ(found, pkg);

	pkgs_free(&pkgs);

	END;
}

TEST(pkgs_get_build_order)
{
	START;

	pkgs_t pkgs = {0};
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkg_t *pkg;
	list_node_t target;

	pkg = pkgs_add(&pkgs, NULL);

	arr_t deps = {0};
	log_set_quiet(0, 1);
	arr_init(&deps, 0, sizeof(uint), ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(pkgs_get_build_order(&pkgs, &deps), 0);

	pkg_add_target(pkg, &pkgs.targets, STRV("target0"), &target);

	targets_add_dep(&pkgs.targets, target, STRV("target1"));

	pkg = pkgs_add(&pkgs, NULL);
	pkg_add_target(pkg, &pkgs.targets, STRV("target1"), &target);

	pkg = pkgs_add(&pkgs, NULL);
	pkg_add_target(pkg, &pkgs.targets, STRV("target2"), &target);

	deps.cnt = 0;

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
	pkgs_init(&pkgs, 1, ALLOC_STD);

	pkgs_add(&pkgs, NULL);

	char buf[256] = {0};
	EXPECT_EQ(pkgs_print(&pkgs, DST_BUF(buf)), 48);
	EXPECT_STR(buf,
		   "[package]\n"
		   "ID: 0\n"
		   "NAME: \n"
		   "URL: \n"
		   "DIR: \n"
		   "SRC: \n"
		   "INC: \n"
		   "\n");

	pkgs_free(&pkgs);

	END;
}

STEST(pkgs)
{
	SSTART;

	RUN(pkgs_init_free);
	RUN(pkgs_add);
	RUN(pkgs_add_strs_oom);
	RUN(pkgs_set_str);
	RUN(pkgs_set_uri);
	RUN(pkgs_get_name);
	RUN(pkgs_get);
	RUN(pkgs_find);
	RUN(pkgs_get_build_order);
	RUN(pkgs_print);

	SEND;
}
