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

	EXPECT_EQ(proj_set_dir(NULL, NULL, STRV_NULL, NULL), 1);

	proj_free(&proj);

	END;
}

TEST(proj_set_dir_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_dir(&proj, &fs, STRV_NULL, NULL), 1);
	log_set_quiet(0, 0);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_set_dir_exe)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	EXPECT_EQ(proj_set_dir(&proj, &fs, STRV_NULL, &tmp), 0);
	EXPECT_EQ(proj.pkgs.pkgs.cnt, 1);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_set_dir_exe_oom)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	mem_oom(1);
	EXPECT_EQ(proj_set_dir(&proj, &fs, STRV_NULL, &tmp), 1);
	mem_oom(0);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_set_dir_lib)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 2, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("include"));
	fs_mkdir(&fs, STRV("src"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	EXPECT_EQ(proj_set_dir(&proj, &fs, STRV_NULL, &tmp), 0);
	EXPECT_EQ(proj.pkgs.pkgs.cnt, 1);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_set_dir_pkgs_src)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 2, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("pkgs"));
	fs_mkdir(&fs, STRV("src"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_dir(&proj, &fs, STRV_NULL, &tmp), 1);
	log_set_quiet(0, 0);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_set_dir_exe_dep_lib)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 6, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("pkgs"));
	fs_mkdir(&fs, STRV("pkgs/exe"));
	fs_mkdir(&fs, STRV("pkgs/exe/src"));
	fs_mkdir(&fs, STRV("pkgs/lib"));
	fs_mkdir(&fs, STRV("pkgs/lib/src"));
	fs_mkdir(&fs, STRV("pkgs/lib/include"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	EXPECT_EQ(proj_set_dir(&proj, &fs, STRV_NULL, &tmp), 0);
	EXPECT_EQ(proj.pkgs.pkgs.cnt, 2);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_set_pkg)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(proj_set_pkg(NULL, NULL), NULL);
	EXPECT_NE(proj_set_pkg(&proj, NULL), NULL);

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
	EXPECT_EQ(proj_add_pkg(&proj, NULL), NULL);
	mem_oom(0);
	EXPECT_EQ(proj_add_pkg(NULL, NULL), NULL);
	EXPECT_NE(proj_add_pkg(&proj, NULL), NULL);

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

	char buf[256] = {0};
	EXPECT_EQ(proj_print(NULL, DST_BUF(buf)), 0);
	EXPECT_EQ(proj_print(&proj, DST_BUF(buf)), 26);
	EXPECT_STR(buf,
		   "[project]\n"
		   "DIR: \n"
		   "OUTDIR: \n"
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
	RUN(proj_set_dir_exe_oom);
	RUN(proj_set_dir_lib);
	RUN(proj_set_dir_pkgs_src);
	RUN(proj_set_dir_exe_dep_lib);
	RUN(proj_set_pkg);
	RUN(proj_add_pkg);
	RUN(proj_print);

	SEND;
}
