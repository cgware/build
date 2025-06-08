#include "proj_gen.h"

#include "log.h"
#include "test.h"

TEST(proj_gen_folders)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 10, 1, ALLOC_STD);

	fs_mkpath(&fs, STRV_NULL, STRV("."));

	gen_driver_t drv = *gen_find_param(STRV("N"));

	drv.fs = &fs;

	EXPECT_EQ(proj_gen(NULL, NULL, STRV_NULL, STRV_NULL), 1);
	EXPECT_EQ(proj_gen(NULL, &drv, STRV("."), STRV(".")), 0);

	EXPECT_EQ(fs_isfile(&fs, STRV("./bin/.gitignore")), 1);
	EXPECT_EQ(fs_isfile(&fs, STRV("./tmp/.gitignore")), 1);
	EXPECT_EQ(fs_isdir(&fs, STRV("./tmp/build")), 1);

	fs_free(&fs);

	END;
}

STEST(proj_gen)
{
	SSTART;

	RUN(proj_gen_folders);

	SEND;
}
