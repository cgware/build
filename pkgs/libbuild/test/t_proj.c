#include "proj.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(proj_init_free)
{
	START;

	proj_t proj = {0};

	EXPECT_EQ(proj_init(NULL, 0, ALLOC_STD), NULL);
	mem_oom(1);
	EXPECT_EQ(proj_init(&proj, 1, ALLOC_STD), NULL);
	mem_oom(0);
	EXPECT_EQ(proj_init(&proj, 1, ALLOC_STD), &proj);

	proj_free(&proj);
	proj_free(NULL);

	END;
}

TEST(proj_free_pkg)
{
	START;

	proj_t proj = {0};

	proj_init(&proj, 1, ALLOC_STD);

	proj.is_pkg = 1;

	proj_free(&proj);

	END;
}

TEST(proj_set_dir)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	EXPECT_EQ(proj_set_dir(NULL, STRV_NULL), 1);

	proj_free(&proj);

	END;
}

TEST(proj_set_dir_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_dir(&proj, STRV("test/empty")), 1);
	log_set_quiet(0, 0);

	proj_free(&proj);

	END;
}

TEST(proj_set_dir_exe)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	EXPECT_EQ(proj_set_dir(&proj, STRV("test/exe")), 0);

	proj_free(&proj);

	END;
}

TEST(proj_set_dir_lib)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	EXPECT_EQ(proj_set_dir(&proj, STRV("test/lib")), 0);

	proj_free(&proj);

	END;
}

TEST(proj_set_dir_pkgs_src)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_dir(&proj, STRV("test/pkgs_src")), 1);
	log_set_quiet(0, 0);

	proj_free(&proj);

	END;
}

TEST(proj_set_pkg)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(proj_set_pkg(NULL), NULL);
	EXPECT_NE(proj_set_pkg(&proj), NULL);

	proj_free(&proj);

	END;
}

TEST(proj_add_pkg)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	mem_oom(1);
	EXPECT_EQ(proj_add_pkg(&proj), NULL);
	mem_oom(0);
	EXPECT_EQ(proj_add_pkg(NULL), NULL);
	EXPECT_NE(proj_add_pkg(&proj), NULL);

	proj_free(&proj);

	END;
}

TEST(proj_print)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	pkg_t *pkg = proj_add_pkg(&proj);
	pkg_set_dir(pkg, STRV("test/empty"));

	char buf[256] = {0};
	EXPECT_EQ(proj_print(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);
	proj_print(&proj, PRINT_DST_BUF(buf, sizeof(buf), 0));
	EXPECT_STR(buf,
		   "[project]\n"
		   "DIR: \n"
		   "OUTDIR: \n"
		   "\n"
		   "[project.package]\n"
		   "SRC: \n"
		   "INCLUDE: \n"
		   "\n");

	proj_free(&proj);

	END;
}

TEST(proj_print_exe)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	pkg_t *pkg = proj_set_pkg(&proj);
	pkg_set_dir(pkg, STRV("test/exe"));

	char buf[256] = {0};
	EXPECT_EQ(proj_print(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);
	proj_print(&proj, PRINT_DST_BUF(buf, sizeof(buf), 0));
	EXPECT_STR(buf,
		   "[project]\n"
		   "DIR: \n"
		   "OUTDIR: \n"
		   "\n"
		   "[project.package]\n"
		   "SRC: test/exe/src\n"
		   "INCLUDE: \n"
		   "\n");

	proj_free(&proj);

	END;
}

STEST(proj)
{
	SSTART;

	RUN(proj_init_free);
	RUN(proj_free_pkg);
	RUN(proj_set_dir);
	RUN(proj_set_dir_empty);
	RUN(proj_set_dir_exe);
	RUN(proj_set_dir_lib);
	RUN(proj_set_dir_pkgs_src);
	RUN(proj_set_pkg);
	RUN(proj_add_pkg);
	RUN(proj_print);
	RUN(proj_print_exe);

	SEND;
}
