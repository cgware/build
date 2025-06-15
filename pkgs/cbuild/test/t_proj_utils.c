#include "proj_utils.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(proj_add_pkg_target)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	EXPECT_EQ(proj_add_pkg_target(NULL, STRV_NULL, NULL, NULL), NULL);
	mem_oom(1);
	proj.targets.cnt = proj.targets.cap;
	EXPECT_EQ(proj_add_pkg_target(&proj, STRV_NULL, NULL, NULL), NULL);
	proj.pkgs.cnt = proj.pkgs.cap;
	EXPECT_EQ(proj_add_pkg_target(&proj, STRV_NULL, NULL, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(proj_add_pkg_target(&proj, STRV_NULL, NULL, NULL), NULL);

	proj_free(&proj);

	END;
}

TEST(proj_add_dep_uri)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 2, 3, ALLOC_STD);
	log_set_quiet(0, 0);

	uint p1, t1;
	proj_add_pkg_target(&proj, STRV("p1"), &p1, &t1);

	EXPECT_EQ(proj_add_dep_uri(NULL, proj.targets.cnt, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_add_dep_uri(&proj, proj.targets.cnt, STRV_NULL), 1);
	EXPECT_EQ(proj_add_dep_uri(&proj, t1, STRV_NULL), 1);
	log_set_quiet(0, 0);

	EXPECT_EQ(proj_add_dep_uri(&proj, t1, STRV("p2")), 0);
	EXPECT_EQ(proj_add_dep_uri(&proj, t1, STRV("p2:t2")), 0);
	char buf[64] = {0};
	proj_print_deps(&proj, DST_BUF(buf));
	EXPECT_STR(buf,
		   "p1\n"
		   "  p1: p2:p2 p2:t2\n"
		   "p2\n"
		   "  p2:\n"
		   "  t2:\n");

	proj_free(&proj);

	END;
}

TEST(proj_add_dep_uri_oom)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 2, 2, ALLOC_STD);
	log_set_quiet(0, 0);

	uint p1, t1;
	proj_add_pkg_target(&proj, STRV("p1"), &p1, &t1);

	mem_oom(1);
	proj.targets.cnt = proj.targets.cap - 1;
	EXPECT_EQ(proj_add_dep_uri(&proj, t1, STRV("p:t")), 1);
	proj.pkgs.cnt = proj.pkgs.cap;
	EXPECT_EQ(proj_add_dep_uri(&proj, t1, STRV("p")), 1);
	mem_oom(0);

	proj_free(&proj);

	END;
}

STEST(proj_utils)
{
	SSTART;

	RUN(proj_add_pkg_target);
	RUN(proj_add_dep_uri);
	RUN(proj_add_dep_uri_oom);

	SEND;
}
