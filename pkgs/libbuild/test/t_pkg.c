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

TEST(pkg_set_source)
{
	START;

	pkg_t pkg = {0};
	log_set_quiet(0, 1);
	pkg_init(&pkg);
	log_set_quiet(0, 0);

	EXPECT_EQ(pkg_set_source(NULL, STRV_NULL), 1);
	EXPECT_EQ(pkg_set_source(&pkg, STRV("tests/no_folders")), 0);

	EXPECT_EQ(pkg.src.len, 0);
	EXPECT_EQ(pkg.include.len, 0);

	pkg_free(&pkg);

	END;
}

STEST(pkg)
{
	SSTART;

	RUN(pkg_init_free);
	RUN(pkg_set_source);

	SEND;
}
