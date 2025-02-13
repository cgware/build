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

	proj_add_pkg(&proj);

	char buf[256] = {0};
	EXPECT_EQ(proj_print(NULL, PRINT_DST_BUF(buf, sizeof(buf), 0)), 0);
	proj_print(&proj, PRINT_DST_BUF(buf, sizeof(buf), 0));
	EXPECT_STR(buf,
		   "[project]\n"
		   "BUILDDIR: \n"
		   "OUTDIR: \n"
		   "\n"
		   "[project.package]\n"
		   "SRC: \n"
		   "INCLUDE: \n"
		   "\n");

	proj_free(&proj);

	END;
}

STEST(proj)
{
	SSTART;

	RUN(proj_init_free);
	RUN(proj_add_pkg);
	RUN(proj_print);

	SEND;
}
