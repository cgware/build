#include "pkg.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(pkg_init_free)
{
	START;

	pkg_t pkg = {0};

	EXPECT_EQ(pkg_init(NULL, 0), NULL);
	EXPECT_EQ(pkg_init(&pkg, 0), &pkg);

	pkg_free(&pkg);
	pkg_free(NULL);

	END;
}

TEST(pkg_add_target)
{
	START;

	pkg_t pkg = {0};
	pkg_init(&pkg, 0);

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(pkg_add_target(NULL, NULL, STRV_NULL, NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(pkg_add_target(&pkg, &targets, STRV("target"), NULL), NULL);
	mem_oom(0);
	EXPECT_NE(pkg_add_target(&pkg, &targets, STRV("target"), NULL), NULL);

	pkg.targets = targets.targets.cnt;
	log_set_quiet(0, 1);
	EXPECT_EQ(pkg_add_target(&pkg, &targets, STRV("target"), NULL), NULL);
	log_set_quiet(0, 0);

	targets_free(&targets);
	pkg_free(&pkg);

	END;
}

TEST(pkg_print)
{
	START;

	pkg_t pkg = {0};
	pkg_init(&pkg, 0);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	pkg_add_target(&pkg, &targets, STRV("target"), NULL);

	char buf[256] = {0};
	EXPECT_EQ(pkg_print(&pkg, &targets, DST_BUF(buf)), 50);
	EXPECT_STR(buf,
		   "[target]\n"
		   "TYPE: UNKNOWN\n"
		   "NAME: target\n"
		   "FILE: \n"
		   "DEPS:\n"
		   "\n");

	targets_free(&targets);
	pkg_free(&pkg);

	END;
}

STEST(pkg)
{
	SSTART;

	RUN(pkg_init_free);
	RUN(pkg_add_target);
	RUN(pkg_print);

	SEND;
}
