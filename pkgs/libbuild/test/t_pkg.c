#include "pkg.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(pkg_init_free)
{
	START;

	pkg_t pkg = {0};

	EXPECT_EQ(pkg_init(NULL), NULL);
	EXPECT_EQ(pkg_init(&pkg), &pkg);

	pkg_free(&pkg);
	pkg_free(NULL);

	END;
}

TEST(pkg_set_dir)
{
	START;

	pkg_t pkg = {0};
	log_set_quiet(0, 1);
	pkg_init(&pkg);
	log_set_quiet(0, 0);

	EXPECT_EQ(pkg_set_dir(NULL, STRV_NULL), 1);
	EXPECT_EQ(pkg_set_dir(&pkg, STRV("tests/min")), 0);

	EXPECT_EQ(pkg.src.len, 0);
	EXPECT_EQ(pkg.include.len, 0);

	pkg_free(&pkg);

	END;
}

TEST(pkg_print)
{
	START;

	pkg_t pkg = {0};
	log_set_quiet(0, 1);
	pkg_init(&pkg);
	log_set_quiet(0, 0);

	char buf[256] = {0};
	pkg_print(&pkg, PRINT_DST_BUF(buf, sizeof(buf), 0));
	EXPECT_STR(buf,
		   "[project.package]\n"
		   "SRC: \n"
		   "INCLUDE: \n"
		   "\n");

	pkg_free(&pkg);

	END;
}

STEST(pkg)
{
	SSTART;

	RUN(pkg_init_free);
	RUN(pkg_set_dir);
	RUN(pkg_print);

	SEND;
}
