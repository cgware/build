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

	EXPECT_EQ(proj_add_pkg(NULL, NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(proj_add_pkg(&proj, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(proj_add_pkg(&proj, NULL), NULL);

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
	EXPECT_EQ(proj_add_pkg(&proj, NULL), NULL);
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
	pkg_t *pkg = proj_add_pkg(&proj, &id);

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
	pkg_t *data = proj_add_pkg(&proj, &pkg);

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
	proj_add_pkg(&proj, &pkg);

	EXPECT_EQ(proj_add_target(NULL, proj.pkgs.cnt, NULL), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_add_target(&proj, proj.pkgs.cnt, NULL), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(proj_add_target(&proj, pkg, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(proj_add_target(&proj, pkg, NULL), NULL);

	proj_free(&proj);

	END;
}

TEST(proj_add_target_oom_strs)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg;
	proj_add_pkg(&proj, &pkg);

	proj.strs.off.cnt = proj.strs.off.cap;
	mem_oom(1);
	EXPECT_EQ(proj_add_target(&proj, pkg, NULL), NULL);
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
	proj_add_pkg(&proj, &pkg);

	uint id;
	target_t *target = proj_add_target(&proj, pkg, &id);

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
	proj_add_pkg(&proj, &pkg1);
	proj_add_pkg(&proj, &pkg2);

	EXPECT_EQ(proj_find_target(NULL, proj.pkgs.cnt, STRV_NULL, NULL), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_find_target(&proj, proj.pkgs.cnt, STRV_NULL, NULL), NULL);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_find_target(&proj, pkg2, STRV_NULL, NULL), NULL);

	proj_add_target(&proj, pkg1, NULL);

	uint target, found;
	target_t *data = proj_add_target(&proj, pkg2, &target);

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

	pkg_t *pkg = proj_add_pkg(&proj, NULL);

	EXPECT_EQ(proj_set_str(NULL, proj.strs.off.cnt, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_str(&proj, proj.strs.off.cnt, STRV_NULL), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_set_str(&proj, pkg->strs + PKG_STR_NAME, STRV_NULL), 0);

	proj_free(&proj);

	END;
}

TEST(proj_get_str)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);

	EXPECT_EQ(proj_get_str(NULL, proj.strs.off.cnt).data, NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_str(&proj, proj.strs.off.cnt).data, NULL);
	log_set_quiet(0, 0);
	strv_t name = proj_get_str(&proj, pkg->strs + PKG_STR_NAME);
	EXPECT_STRN(name.data, "", name.len);

	proj_free(&proj);

	END;
}

TEST(proj_add_dep)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	uint pkg1, pkg2;
	proj_add_pkg(&proj, &pkg1);
	proj_add_pkg(&proj, &pkg2);

	uint target1, target2;
	proj_add_target(&proj, pkg1, &target1);
	proj_add_target(&proj, pkg2, &target2);

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
	proj_init(&proj, 5, 5, ALLOC_STD);

	uint p1, p2, p3, p4, p5;
	uint t1, t2, t3, t4, t5;

	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);
	proj_add_pkg(&proj, &p4);
	proj_add_pkg(&proj, &p5);

	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);
	proj_add_target(&proj, p3, &t3);
	proj_add_target(&proj, p4, &t4);
	proj_add_target(&proj, p5, &t5);

	arr_t deps = {0};
	arr_init(&deps, 5, sizeof(uint), ALLOC_STD);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_deps(&proj, proj.targets.cnt, &deps), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_get_deps(&proj, t2, &deps), 0);

	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t2, t3);
	proj_add_dep(&proj, t2, t4);
	proj_add_dep(&proj, t3, t4);

	EXPECT_EQ(proj_get_deps(NULL, proj.targets.cnt, NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_deps(&proj, proj.targets.cnt, NULL), 1);
	log_set_quiet(0, 0);

	EXPECT_EQ(proj_get_deps(&proj, t4, &deps), 0);
	EXPECT_EQ(deps.cnt, 0);

	EXPECT_EQ(proj_get_deps(&proj, t2, &deps), 0);
	EXPECT_EQ(deps.cnt, 2);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), t3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), t4);

	arr_free(&deps);
	proj_free(&proj);

	END;
}

TEST(proj_get_pdeps)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 4, 4, ALLOC_STD);

	uint p1, p2, p3;
	uint t1, t2, t3;

	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);

	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);
	proj_add_target(&proj, p3, &t3);

	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t1, t3);
	proj_add_dep(&proj, t3, t2);

	arr_t deps = {0};
	arr_init(&deps, 2, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_get_deps(&proj, t1, &deps), 0);

	EXPECT_EQ(deps.cnt, 2);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), t3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), t2);

	arr_free(&deps);
	proj_free(&proj);

	END;
}

TEST(proj_get_rdeps)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 4, 4, ALLOC_STD);

	uint p1, p2, p3, p4;
	uint t1, t2, t3, t4;

	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);
	proj_add_pkg(&proj, &p4);

	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);
	proj_add_target(&proj, p3, &t3);
	proj_add_target(&proj, p4, &t4);

	proj_add_dep(&proj, t2, t1);
	proj_add_dep(&proj, t3, t1);
	proj_add_dep(&proj, t4, t2);
	proj_add_dep(&proj, t4, t3);

	arr_t deps = {0};
	arr_init(&deps, 3, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_get_deps(&proj, t4, &deps), 0);

	EXPECT_EQ(deps.cnt, 3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), t2);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), t3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 2), t1);

	arr_free(&deps);
	proj_free(&proj);

	END;
}

TEST(proj_get_pkg_build_order)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 3, 4, ALLOC_STD);

	uint p1, p2, p3;
	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);

	uint t1, t2, t3, t4;
	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p1, &t2);
	proj_add_target(&proj, p2, &t3);
	proj_add_target(&proj, p3, &t4);

	proj_add_dep(&proj, t1, t3);
	proj_add_dep(&proj, t1, t2);
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

TEST(proj_get_tgt_build_order)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 3, 4, ALLOC_STD);

	uint p1, p2, p3;
	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);

	uint t1, t2, t3, t4;
	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p1, &t2);
	proj_add_target(&proj, p2, &t3);
	proj_add_target(&proj, p3, &t4);

	proj_add_dep(&proj, t1, t3);
	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t2, t3);
	proj_add_dep(&proj, t3, t4);

	arr_t order = {0};
	arr_init(&order, 2, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_get_tgt_build_order(NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(proj_get_tgt_build_order(&proj, &order, ALLOC_STD), 0);

	EXPECT_EQ(order.cnt, 4);
	EXPECT_EQ(*(uint *)arr_get(&order, 0), 3);
	EXPECT_EQ(*(uint *)arr_get(&order, 1), 2);
	EXPECT_EQ(*(uint *)arr_get(&order, 2), 1);
	EXPECT_EQ(*(uint *)arr_get(&order, 3), 0);

	proj_add_dep(&proj, t4, t3);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_get_tgt_build_order(&proj, &order, ALLOC_STD), 1);
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

	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);

	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);

	proj_add_dep(&proj, t1, t2);

	char buf[512] = {0};
	EXPECT_EQ(proj_print(NULL, DST_BUF(buf)), 0);
	EXPECT_EQ(proj_print(&proj, DST_BUF(buf)), 374);
	EXPECT_STR(buf,
		   "[project]\n"
		   "NAME: \n"
		   "OUTDIR: \n"
		   "\n"
		   "[pkg]\n"
		   "NAME: \n"
		   "PATH: \n"
		   "SRC: \n"
		   "INC: \n"
		   "TEST: \n"
		   "URI: \n"
		   "URI_FILE: \n"
		   "URI_NAME: \n"
		   "URI_VER: \n"
		   "URI_DIR: \n"
		   "\n"
		   "[target]\n"
		   "NAME: \n"
		   "TYPE: UNKNOWN\n"
		   "PREP: \n"
		   "CONF: \n"
		   "COMP: \n"
		   "INST: \n"
		   "OUT: \n"
		   "TGT: \n"
		   "TYPE: 0\n"
		   "DEPS: :\n"
		   "\n"
		   "[pkg]\n"
		   "NAME: \n"
		   "PATH: \n"
		   "SRC: \n"
		   "INC: \n"
		   "TEST: \n"
		   "URI: \n"
		   "URI_FILE: \n"
		   "URI_NAME: \n"
		   "URI_VER: \n"
		   "URI_DIR: \n"
		   "\n"
		   "[target]\n"
		   "NAME: \n"
		   "TYPE: UNKNOWN\n"
		   "PREP: \n"
		   "CONF: \n"
		   "COMP: \n"
		   "INST: \n"
		   "OUT: \n"
		   "TGT: \n"
		   "TYPE: 0\n"
		   "DEPS:\n")

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
	RUN(proj_get_pdeps);
	RUN(proj_get_rdeps);
	RUN(proj_get_pkg_build_order);
	RUN(proj_get_tgt_build_order);
	RUN(proj_print);

	SEND;
}
