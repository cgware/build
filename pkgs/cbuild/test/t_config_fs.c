#include "config_fs.h"

#include "path.h"
#include "test.h"

TEST(config_fs_empty)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	config_dir_t *dir;

	EXPECT_EQ(config_fs(NULL, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD, DST_NONE()), NULL);
	EXPECT_NE(dir = config_fs(&config, NULL, NULL, STRV_NULL, STRV("path"), STRV("name"), NULL, ALLOC_STD, DST_NONE()), NULL);

	strv_t val = {0};

	val = config_get_str(&config, dir->strs + CONFIG_DIR_NAME);
	EXPECT_STRN(val.data, "name", val.len);
	val = config_get_str(&config, dir->strs + CONFIG_DIR_PATH);
	EXPECT_STRN(val.data, "path", val.len)

	config_free(&config);

	END;
}

TEST(config_fs_src)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 3, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);
	config_dir_t *dir;

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_NE(dir = config_fs(&config, &fs, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()), NULL);

	strv_t val = config_get_str(&config, dir->strs + CONFIG_DIR_SRC);
	EXPECT_STRN(val.data, "src", val.len);

	config_free(&config);
	fs_free(&fs);

	END;
}

TEST(config_fs_include)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 3, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("include"));

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);
	config_dir_t *dir;

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_NE(dir = config_fs(&config, &fs, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()), NULL);

	strv_t val = config_get_str(&config, dir->strs + CONFIG_DIR_INC);
	EXPECT_STRN(val.data, "include", val.len);

	config_free(&config);
	fs_free(&fs);

	END;
}

TEST(config_fs_test)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 3, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("test"));

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);
	config_dir_t *dir;

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_NE(dir = config_fs(&config, &fs, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()), NULL);

	strv_t val = config_get_str(&config, dir->strs + CONFIG_DIR_TST);
	EXPECT_STRN(val.data, "test", val.len);

	config_free(&config);
	fs_free(&fs);

	END;
}

TEST(config_fs_pkgs)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 3, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("pkgs"));
	fs_mkdir(&fs, STRV("pkgs/p1"));
	fs_mkdir(&fs, STRV("pkgs/p2"));

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);
	config_dir_t *dir;

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_NE(dir = config_fs(&config, &fs, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()), NULL);
	EXPECT_EQ(config.dirs.cnt, 3);

	const config_dir_t *d1 = config_get_dir(&config, 1);
	const config_dir_t *d2 = config_get_dir(&config, 2);

	strv_t val;

	val = config_get_str(&config, d1->strs + CONFIG_DIR_PATH);
	EXPECT_STRN(val.data, "pkgs" SEP "p1", val.len);
	val = config_get_str(&config, d2->strs + CONFIG_DIR_PATH);
	EXPECT_STRN(val.data, "pkgs" SEP "p2", val.len);

	config_free(&config);
	fs_free(&fs);

	END;
}

TEST(config_fs_build_cfg)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkfile(&fs, STRV("build.cfg"));

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);
	config_dir_t *dir;

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_NE(dir = config_fs(&config, &fs, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()), NULL);
	EXPECT_EQ(config.dirs.cnt, 1);

	config_free(&config);
	fs_free(&fs);

	END;
}

TEST(config_fs_main)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));
	fs_mkfile(&fs, STRV("src/main.c"));

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);
	config_dir_t *dir;

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_NE(dir = config_fs(&config, &fs, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD, DST_STD()), NULL);

	EXPECT_EQB(dir->has_main, 1);

	config_free(&config);
	fs_free(&fs);

	END;
}

STEST(config_fs)
{
	SSTART;

	RUN(config_fs_empty);
	RUN(config_fs_src);
	RUN(config_fs_include);
	RUN(config_fs_test);
	RUN(config_fs_pkgs);
	RUN(config_fs_build_cfg);
	RUN(config_fs_main);

	SEND;
}
