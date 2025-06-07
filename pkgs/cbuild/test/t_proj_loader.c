#include "proj_loader.h"

#include "log.h"
#include "mem.h"
#include "proj.h"
#include "test.h"

TEST(proj_load)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	EXPECT_EQ(proj_load(NULL, NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD, NULL), 1);

	proj_free(&proj);

	END;
}

TEST(proj_load_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_load(&fs, NULL, STRV_NULL, STRV_NULL, &proj, ALLOC_STD, NULL), 1);
	log_set_quiet(0, 0);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_load_empty_cfg)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkfile(&fs, STRV("proj.cfg"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_load(&fs, NULL, STRV_NULL, STRV_NULL, &proj, ALLOC_STD, &tmp), 1);
	log_set_quiet(0, 0);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_load_src)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	EXPECT_EQ(proj_load(&fs, NULL, STRV_NULL, STRV_NULL, &proj, ALLOC_STD, &tmp), 0);
	EXPECT_EQ(proj.pkgs.pkgs.cnt, 1);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_load_pkgs_src)
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
	EXPECT_EQ(proj_load(&fs, NULL, STRV_NULL, STRV_NULL, &proj, ALLOC_STD, &tmp), 1);
	log_set_quiet(0, 0);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_set_cfg)
{
	START;

	EXPECT_EQ(proj_set_cfg(NULL, NULL, 0, NULL, NULL, ALLOC_STD, NULL), 1);

	END;
}

TEST(proj_set_cfg_ext)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 6, 1, ALLOC_STD);

	proc_t proc = {0};
	proc_init(&proc, 28, 1);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, ext, cpkg;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &ext);
	cfg_add_var(&cfg, root, ext);
	cfg_str(&cfg, STRV("pkg"), STRV("git:repo"), &cpkg);
	cfg_add_var(&cfg, ext, cpkg);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	EXPECT_EQ(proj_set_cfg(&proj, &cfg, root, &fs, &proc, ALLOC_STD, &tmp), 0);

	EXPECT_STRN(proc.buf.data, "git clone repo tmp" SEP "ext" SEP "pkg\n", proc.buf.len);

	cfg_free(&cfg);
	proj_free(&proj);
	proc_free(&proc);
	fs_free(&fs);

	END;
}

TEST(proj_set_cfg_ext_format)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 6, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, ext, pkg;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &ext);
	cfg_add_var(&cfg, root, ext);
	cfg_int(&cfg, STRV("pkg"), 0, &pkg);
	cfg_add_var(&cfg, ext, pkg);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_cfg(&proj, &cfg, root, &fs, NULL, ALLOC_STD, &tmp), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj.pkgs.pkgs.cnt, 0);

	cfg_free(&cfg);
	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_set_cfg_ext_invalid)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 6, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, ext, pkg;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &ext);
	cfg_add_var(&cfg, root, ext);
	cfg_str(&cfg, STRV("pkg"), STRV("invalid"), &pkg);
	cfg_add_var(&cfg, ext, pkg);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_cfg(&proj, &cfg, root, &fs, NULL, ALLOC_STD, &tmp), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj.pkgs.pkgs.cnt, 1);

	cfg_free(&cfg);
	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_set_cfg_ext_oom)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	fs_t fs = {0};
	fs_init(&fs, 6, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, ext, cpkg;

	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &ext);
	cfg_add_var(&cfg, root, ext);
	cfg_str(&cfg, STRV("pkg"), STRV("git:repo"), &cpkg);
	cfg_add_var(&cfg, ext, cpkg);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	mem_oom(1);
	EXPECT_EQ(proj_set_cfg(&proj, &cfg, root, &fs, NULL, ALLOC_STD, &tmp), 1);
	mem_oom(0);

	cfg_free(&cfg);
	proj_free(&proj);
	fs_free(&fs);

	END;
}

STEST(proj_loader)
{
	SSTART;

	RUN(proj_load);
	RUN(proj_load_empty);
	RUN(proj_load_empty_cfg);
	RUN(proj_load_src);
	RUN(proj_load_pkgs_src);
	RUN(proj_set_cfg);
	RUN(proj_set_cfg_ext);
	RUN(proj_set_cfg_ext_format);
	RUN(proj_set_cfg_ext_invalid);
	RUN(proj_set_cfg_ext_oom);

	SEND;
}
