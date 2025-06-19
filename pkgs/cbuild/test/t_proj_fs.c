#include "proj_fs.h"

#include "log.h"
#include "test.h"

TEST(proj_fs_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	EXPECT_EQ(proj_fs(NULL, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(proj_fs(&proj, NULL, NULL, STRV_NULL, STRV_NULL, STRV("p"), NULL, ALLOC_STD), 0);

	EXPECT_STRN(proj.name.data, "p", proj.name.len);
	EXPECT_STRN(proj.outdir.data, "bin" SEP "${ARCH}-${CONFIG}" SEP, proj.outdir.len)

	proj_free(&proj);

	END;
}

TEST(proj_fs_cfg_empty)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkfile(&fs, STRV("build.cfg"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_EQ(proj_fs(&proj, &fs, NULL, STRV_NULL, STRV_NULL, STRV_NULL, &buf, ALLOC_STD), 0);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_fs_pkgs)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("pkgs"));
	fs_mkdir(&fs, STRV("pkgs/p1"));
	fs_mkdir(&fs, STRV("pkgs/p2"));

	void *f;
	fs_open(&fs, STRV("pkgs/p1/build.cfg"), "w", &f);
	fs_write(&fs,
		 f,
		 STRV("[pkg]\n"
		      "name = a\n"
		      "deps = [p2]\n"));
	fs_close(&fs, f);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_EQ(proj_fs(&proj, &fs, NULL, STRV_NULL, STRV_NULL, STRV(""), &buf, ALLOC_STD), 0);
	EXPECT_EQ(proj.pkgs.cnt, 2);
	pkg_t *p1    = proj_get_pkg(&proj, 0);
	pkg_t *p2    = proj_get_pkg(&proj, 1);
	strv_t name1 = proj_get_str(&proj, p1->strs + PKG_NAME);
	strv_t name2 = proj_get_str(&proj, p2->strs + PKG_NAME);
	strv_t dir1  = proj_get_str(&proj, p1->strs + PKG_DIR);
	strv_t dir2  = proj_get_str(&proj, p2->strs + PKG_DIR);

	EXPECT_STRN(name1.data, "a", name1.len);
	EXPECT_STRN(name2.data, "p2", name2.len);
	EXPECT_STRN(dir1.data, "pkgs" SEP "p1" SEP, dir1.len);
	EXPECT_STRN(dir2.data, "pkgs" SEP "p2" SEP, dir2.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_fs_child)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	EXPECT_EQ(proj_fs_child(NULL, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);

	proj_free(&proj);

	END;
}

TEST(proj_fs_git_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	EXPECT_EQ(proj_fs_git(NULL, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(proj_fs_git(&proj, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 0);

	proj_free(&proj);

	END;
}

TEST(proj_fs_git)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	proc_t proc = {0};
	proc_init(&proc, 32, 1);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);

	EXPECT_EQ(proj_fs_git(&proj, &fs, &proc, STRV_NULL, STRV("pkg"), STRV("pkg"), STRV("repo"), &buf, ALLOC_STD), 0);

	EXPECT_EQ(fs_isfile(&fs, STRV("tmp/.gitignore")), 1);
	EXPECT_EQ(fs_isdir(&fs, STRV("tmp/ext/pkg")), 1);
	EXPECT_STRN(proc.buf.data, "git clone repo tmp" SEP "ext" SEP "pkg\n", proc.buf.len);

	proj_free(&proj);
	proc_free(&proc);
	fs_free(&fs);

	END;
}

STEST(proj_fs)
{
	SSTART;

	RUN(proj_fs_empty);
	RUN(proj_fs_cfg_empty);
	RUN(proj_fs_pkgs);
	RUN(proj_fs_child);
	RUN(proj_fs_git_empty);
	RUN(proj_fs_git);

	SEND;
}
