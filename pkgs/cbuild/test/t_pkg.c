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

TEST(pkg_print)
{
	START;

	pkg_t pkg = {0};
	pkg_init(&pkg, 0);

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	char buf[256] = {0};
	EXPECT_EQ(pkg_print(&pkg, &targets, PRINT_DST_BUF(buf, sizeof(buf), 0)), 42);
	EXPECT_STR(buf,
		   "[package]\n"
		   "TYPE: UNKNOWN\n"
		   "DIR: \n"
		   "SRC: \n"
		   "INC: \n");

	targets_free(&targets);
	pkg_free(&pkg);

	END;
}

STEST(pkg)
{
	SSTART;

	RUN(pkg_init_free);
	RUN(pkg_print);

	SEND;
}
