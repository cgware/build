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

TEST(proj_add_pkg_target_exists)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg_id, target_id, pid, tid;
	pkg_t *pkg = proj_add_pkg_target(&proj, STRV(""), &pkg_id, &target_id);

	EXPECT_EQ(proj_add_pkg_target(&proj, STRV(""), &pid, &tid), pkg);
	EXPECT_EQ(pid, pkg_id);
	EXPECT_EQ(tid, target_id);

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

TEST(proj_set_ext_uri)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg_target(&proj, STRV("p1"), NULL, NULL);

	EXPECT_EQ(proj_set_ext_uri(NULL, NULL, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_ext_uri(&proj, NULL, STRV_NULL), 1);
	EXPECT_EQ(proj_set_ext_uri(&proj, pkg, STRV_NULL), 1);
	EXPECT_EQ(proj_set_ext_uri(&proj, pkg, STRV("invalid")), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_set_ext_uri(&proj, pkg, STRV("git:repo")), 0);
	EXPECT_EQ(pkg->proto, PKG_URL_GIT)
	strv_t url = proj_get_str(&proj, pkg->strs + PKG_URL);
	EXPECT_STRN(url.data, "repo", url.len);

	proj_free(&proj);

	END;
}

STEST(proj_utils)
{
	SSTART;

	RUN(proj_add_pkg_target);
	RUN(proj_add_pkg_target_exists);
	RUN(proj_add_dep_uri);
	RUN(proj_add_dep_uri_oom);
	RUN(proj_set_ext_uri);

	SEND;
}
