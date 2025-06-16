#include "proj.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(proj_init_free)
{
	START;

	proj_t proj = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_init(NULL, 0, 0, ALLOC_STD), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(proj_init(&proj, 1, 0, ALLOC_STD), NULL);
	mem_oom(0);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_init(&proj, 1, 0, ALLOC_STD), &proj);
	log_set_quiet(0, 0);

	proj_free(&proj);
	proj_free(NULL);

	END;
}

TEST(proj_add_pkg)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(proj_add_pkg(NULL, STRV_NULL, NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(proj_add_pkg(&proj, STRV_NULL, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(proj_add_pkg(&proj, STRV_NULL, NULL), NULL);

	proj_free(&proj);

	END;
}

TEST(proj_add_pkg_oom_strs)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 1, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	proj.strs.off.cnt = proj.strs.off.cap;
	mem_oom(1);
	EXPECT_EQ(proj_add_pkg(&proj, STRV_NULL, NULL), NULL);
	mem_oom(0);

	proj_free(&proj);

	END;
}

TEST(proj_get_pkg)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 1, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	uint id;
	pkg_t *pkg = proj_add_pkg(&proj, STRV_NULL, &id);

	EXPECT_EQ(proj_get_pkg(NULL, id), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_pkg(&proj, proj.pkgs.cnt), NULL);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_get_pkg(&proj, id), pkg);

	proj_free(&proj);

	END;
}

TEST(proj_find_pkg)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 1, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	uint pkg, found;
	pkg_t *data = proj_add_pkg(&proj, STRV_NULL, &pkg);

	EXPECT_EQ(proj_find_pkg(NULL, STRV_NULL, NULL), NULL);
	EXPECT_EQ(proj_find_pkg(&proj, STRV_NULL, NULL), NULL);
	EXPECT_EQ(proj_find_pkg(&proj, STRV("asd"), NULL), NULL);
	EXPECT_EQ(proj_find_pkg(&proj, STRV(""), &found), data);
	EXPECT_EQ(found, pkg);

	proj_free(&proj);

	END;
}

TEST(proj_add_target)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	uint pkg;
	proj_add_pkg(&proj, STRV_NULL, &pkg);

	EXPECT_EQ(proj_add_target(NULL, proj.pkgs.cnt, STRV_NULL, NULL), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_add_target(&proj, proj.pkgs.cnt, STRV_NULL, NULL), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(proj_add_target(&proj, pkg, STRV_NULL, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(proj_add_target(&proj, pkg, STRV_NULL, NULL), NULL);

	proj_free(&proj);

	END;
}

TEST(proj_add_target_oom_strs)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg;
	proj_add_pkg(&proj, STRV_NULL, &pkg);

	proj.strs.off.cnt = proj.strs.off.cap;
	mem_oom(1);
	EXPECT_EQ(proj_add_target(&proj, pkg, STRV_NULL, NULL), NULL);
	mem_oom(0);

	proj_free(&proj);

	END;
}

TEST(proj_get_target)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg;
	proj_add_pkg(&proj, STRV_NULL, &pkg);

	uint id;
	target_t *target = proj_add_target(&proj, pkg, STRV_NULL, &id);

	EXPECT_EQ(proj_get_target(NULL, id), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_target(&proj, proj.targets.cnt), NULL);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_get_target(&proj, id), target);

	proj_free(&proj);

	END;
}

TEST(proj_find_target)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	uint pkg1, pkg2;
	proj_add_pkg(&proj, STRV_NULL, &pkg1);
	proj_add_pkg(&proj, STRV_NULL, &pkg2);

	proj_add_target(&proj, pkg1, STRV_NULL, NULL);

	uint target, found;
	target_t *data = proj_add_target(&proj, pkg2, STRV_NULL, &target);

	EXPECT_EQ(proj_find_target(NULL, proj.pkgs.cnt, STRV_NULL, NULL), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_find_target(&proj, proj.pkgs.cnt, STRV_NULL, NULL), NULL);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_find_target(&proj, pkg2, STRV_NULL, NULL), NULL);
	EXPECT_EQ(proj_find_target(&proj, pkg2, STRV("asd"), NULL), NULL);
	EXPECT_EQ(proj_find_target(&proj, pkg2, STRV(""), &found), data);
	EXPECT_EQ(found, target);

	proj_free(&proj);

	END;
}

TEST(proj_set_str)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, STRV_NULL, NULL);

	EXPECT_EQ(proj_set_str(NULL, proj.strs.off.cnt, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_str(&proj, proj.strs.off.cnt, STRV_NULL), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_set_str(&proj, pkg->strs + PKG_NAME, STRV_NULL), 0);

	proj_free(&proj);

	END;
}

TEST(proj_get_str)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, STRV("p"), NULL);

	EXPECT_EQ(proj_get_str(NULL, proj.strs.off.cnt).data, NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_str(&proj, proj.strs.off.cnt).data, NULL);
	log_set_quiet(0, 0);
	strv_t name = proj_get_str(&proj, pkg->strs + PKG_NAME);
	EXPECT_STRN(name.data, "p", name.len);

	proj_free(&proj);

	END;
}

TEST(proj_add_dep)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	uint pkg1, pkg2;
	proj_add_pkg(&proj, STRV_NULL, &pkg1);
	proj_add_pkg(&proj, STRV_NULL, &pkg2);

	uint target1, target2;
	proj_add_target(&proj, pkg1, STRV_NULL, &target1);
	proj_add_target(&proj, pkg2, STRV_NULL, &target2);

	EXPECT_EQ(proj_add_dep(NULL, proj.targets.cnt, proj.targets.cnt), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_add_dep(&proj, proj.targets.cnt, proj.targets.cnt), 1);
	log_set_quiet(0, 0);
	proj.deps.cnt = proj.deps.cap;
	mem_oom(1);
	EXPECT_EQ(proj_add_dep(&proj, target1, target2), 1);
	mem_oom(0);
	EXPECT_EQ(proj_add_dep(&proj, target1, target2), 0);

	proj_free(&proj);

	END;
}

TEST(proj_get_deps)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	uint p1, p2, p3, p4, p5;
	uint t1, t2, t3, t4, t5;

	proj_add_pkg(&proj, STRV("p1"), &p1);
	proj_add_pkg(&proj, STRV("p2"), &p2);
	proj_add_pkg(&proj, STRV("p3"), &p3);
	proj_add_pkg(&proj, STRV("p4"), &p4);
	proj_add_pkg(&proj, STRV("p5"), &p5);

	proj_add_target(&proj, p1, STRV("t1"), &t1);
	proj_add_target(&proj, p2, STRV("t2"), &t2);
	proj_add_target(&proj, p3, STRV("t3"), &t3);
	proj_add_target(&proj, p4, STRV("t4"), &t4);
	proj_add_target(&proj, p5, STRV("t5"), &t5);

	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t2, t3);
	proj_add_dep(&proj, t2, t4);
	proj_add_dep(&proj, t3, t4);

	arr_t deps = {0};
	arr_init(&deps, 5, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_get_deps(NULL, proj.targets.cnt, NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_deps(&proj, proj.targets.cnt, NULL), 1);
	log_set_quiet(0, 0);

	EXPECT_EQ(proj_get_deps(&proj, t4, &deps), 0);
	EXPECT_EQ(deps.cnt, 0);

	EXPECT_EQ(proj_get_deps(&proj, t2, &deps), 0);
	EXPECT_EQ(deps.cnt, 2);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), t4);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), t3);

	proj_add_dep(&proj, t4, t3);
	EXPECT_EQ(proj_get_deps(&proj, t2, &deps), 0);
	EXPECT_EQ(deps.cnt, 2);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), t4);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), t3);

	arr_free(&deps);
	proj_free(&proj);

	END;
}

TEST(proj_print_deps)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	uint pkg1, pkg2;
	proj_add_pkg(&proj, STRV("p1"), &pkg1);
	proj_add_pkg(&proj, STRV("p2"), &pkg2);

	uint target1, target2;
	proj_add_target(&proj, pkg1, STRV("t1"), &target1);
	proj_add_target(&proj, pkg2, STRV("t2"), &target2);

	proj_add_dep(&proj, target1, target2);

	char buf[32] = {0};
	EXPECT_EQ(proj_print_deps(NULL, DST_BUF(buf)), 0);
	EXPECT_EQ(proj_print_deps(&proj, DST_BUF(buf)), 24);
	EXPECT_STR(buf,
		   "p1\n"
		   "  t1: p2:t2\n"
		   "p2\n"
		   "  t2:\n");

	proj_free(&proj);

	END;
}

TEST(proj_get_pkg_build_order)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	uint p1, p2, p3;
	proj_add_pkg(&proj, STRV("p1"), &p1);
	proj_add_pkg(&proj, STRV("p2"), &p2);
	proj_add_pkg(&proj, STRV("p3"), &p3);

	uint t1, t2, t3, t4;
	proj_add_target(&proj, p1, STRV("p1t1"), &t1);
	proj_add_target(&proj, p1, STRV("p2t1"), &t2);
	proj_add_target(&proj, p2, STRV("p3t1"), &t3);
	proj_add_target(&proj, p3, STRV("p3t1"), &t4);

	proj_add_dep(&proj, t1, t3);
	proj_add_dep(&proj, t2, t3);
	proj_add_dep(&proj, t3, t4);

	arr_t order = {0};
	arr_init(&order, 2, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_get_pkg_build_order(NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(proj_get_pkg_build_order(&proj, &order, ALLOC_STD), 0);

	EXPECT_EQ(order.cnt, 3);
	EXPECT_EQ(*(uint *)arr_get(&order, 0), 2);
	EXPECT_EQ(*(uint *)arr_get(&order, 1), 1);
	EXPECT_EQ(*(uint *)arr_get(&order, 2), 0);

	proj_add_dep(&proj, t4, t3);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_pkg_build_order(&proj, &order, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	arr_free(&order);
	proj_free(&proj);

	END;
}

TEST(proj_get_target_build_order)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	uint p1, p2, p3, p4, p5;
	uint t1, t2, t3, t4, t5;

	proj_add_pkg(&proj, STRV("p1"), &p1);
	proj_add_pkg(&proj, STRV("p2"), &p2);

	proj_add_target(&proj, p1, STRV("t1"), &t1);
	proj_add_target(&proj, p2, STRV("t2"), &t2);

	arr_t order = {0};
	arr_init(&order, 5, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_get_target_build_order(&proj, &order, ALLOC_STD), 0);
	EXPECT_EQ(order.cnt, 2);
	EXPECT_EQ(*(uint *)arr_get(&order, 0), 0);
	EXPECT_EQ(*(uint *)arr_get(&order, 1), 1);

	proj_add_pkg(&proj, STRV("p3"), &p3);
	proj_add_pkg(&proj, STRV("p4"), &p4);
	proj_add_pkg(&proj, STRV("p5"), &p5);

	proj_add_target(&proj, p3, STRV("t3"), &t3);
	proj_add_target(&proj, p4, STRV("t4"), &t4);
	proj_add_target(&proj, p5, STRV("t5"), &t5);

	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t2, t3);
	proj_add_dep(&proj, t2, t4);
	proj_add_dep(&proj, t3, t4);

	EXPECT_EQ(proj_get_target_build_order(NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(proj_get_target_build_order(&proj, &order, ALLOC_STD), 0);
	EXPECT_EQ(order.cnt, 5);
	EXPECT_EQ(*(uint *)arr_get(&order, 0), 3);
	EXPECT_EQ(*(uint *)arr_get(&order, 1), 4);
	EXPECT_EQ(*(uint *)arr_get(&order, 2), 2);
	EXPECT_EQ(*(uint *)arr_get(&order, 3), 1);
	EXPECT_EQ(*(uint *)arr_get(&order, 4), 0);

	proj_add_dep(&proj, t2, t1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_target_build_order(&proj, &order, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	arr_free(&order);
	proj_free(&proj);

	END;
}

TEST(proj_print)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	uint p1, p2;
	uint t1, t2;

	proj_add_pkg(&proj, STRV("p1"), &p1);
	proj_add_pkg(&proj, STRV("p2"), &p2);

	proj_add_target(&proj, p1, STRV("t1"), &t1);
	proj_add_target(&proj, p2, STRV("t2"), &t2);

	proj_add_dep(&proj, t1, t2);

	char buf[256] = {0};
	EXPECT_EQ(proj_print(NULL, DST_BUF(buf)), 0);
	EXPECT_EQ(proj_print(&proj, DST_BUF(buf)), 212);
	EXPECT_STR(buf,
		   "[project]\n"
		   "DIR: \n"
		   "OUTDIR: \n"
		   "\n"
		   "[package]\n"
		   "NAME: p1\n"
		   "URL: \n"
		   "DIR: \n"
		   "SRC: \n"
		   "INC: \n"
		   "\n"
		   "[target]\n"
		   "TYPE: UNKNOWN\n"
		   "NAME: t1\n"
		   "FILE: \n"
		   "DEPS: p2:t2\n"
		   "\n"
		   "[package]\n"
		   "NAME: p2\n"
		   "URL: \n"
		   "DIR: \n"
		   "SRC: \n"
		   "INC: \n"
		   "\n"
		   "[target]\n"
		   "TYPE: UNKNOWN\n"
		   "NAME: t2\n"
		   "FILE: \n"
		   "DEPS:\n"
		   "\n")

	proj_free(&proj);

	END;
}

STEST(proj)
{
	SSTART;

	RUN(proj_init_free);
	RUN(proj_add_pkg);
	RUN(proj_add_pkg_oom_strs);
	RUN(proj_get_pkg);
	RUN(proj_find_pkg);
	RUN(proj_add_target);
	RUN(proj_add_target_oom_strs);
	RUN(proj_get_target);
	RUN(proj_find_target);
	RUN(proj_set_str);
	RUN(proj_get_str);
	RUN(proj_add_dep);
	RUN(proj_get_deps);
	RUN(proj_print_deps);
	RUN(proj_get_pkg_build_order);
	RUN(proj_get_target_build_order);
	RUN(proj_print);

	SEND;
}
