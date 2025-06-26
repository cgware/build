#include "proj_cfg.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(proj_cfg_null)
{
	START;

	EXPECT_EQ(proj_cfg(NULL, NULL, 0, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);

	END;
}

TEST(proj_cfg_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root;
	cfg_root(&cfg, &root);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_oom)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	mem_oom(1);
	EXPECT_EQ(proj_cfg(&proj, NULL, 0, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	mem_oom(0);

	proj_free(&proj);

	END;
}

TEST(proj_cfg_src)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root;
	cfg_root(&cfg, &root);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, &fs, NULL, STRV_NULL, STRV_NULL, STRV(""), &buf, ALLOC_STD), 0);
	pkg_t *pkg = proj_get_pkg(&proj, 0);
	strv_t src = proj_get_str(&proj, pkg->strs + PKG_SRC);
	EXPECT_STRN(src.data, "src", src.len);
	target_t *target = proj_get_target(&proj, 0);
	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	cfg_free(&cfg);
	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_cfg_include)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("include"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root;
	cfg_root(&cfg, &root);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, &fs, NULL, STRV_NULL, STRV_NULL, STRV(""), &buf, ALLOC_STD), 0);
	pkg_t *pkg = proj_get_pkg(&proj, 0);
	strv_t inc = proj_get_str(&proj, pkg->strs + PKG_INC);
	EXPECT_STRN(inc.data, "include", inc.len);
	target_t *target = proj_get_target(&proj, 0);
	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	cfg_free(&cfg);
	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_cfg_test)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("test"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root;
	cfg_root(&cfg, &root);

	char tmp[128] = {0};
	str_t buf     = STRB(tmp, 0);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, &fs, NULL, STRV_NULL, STRV_NULL, STRV(""), &buf, ALLOC_STD), 0);
	pkg_t *pkg  = proj_get_pkg(&proj, 0);
	strv_t test = proj_get_str(&proj, pkg->strs + PKG_TST);
	EXPECT_STRN(test.data, "test", test.len);
	target_t *target = proj_get_target(&proj, 1);
	EXPECT_EQ(target->type, TARGET_TYPE_TST);

	cfg_free(&cfg);
	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_cfg_pkg_empty)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, pkg;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &pkg);
	cfg_add_var(&cfg, root, pkg);

	mem_oom(1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	mem_oom(0);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV("p"), NULL, ALLOC_STD), 0);
	EXPECT_NE(proj_find_pkg(&proj, STRV("p"), NULL), NULL);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_add)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cpkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &cpkg);
	cfg_add_var(&cfg, root, cpkg);
	cfg_lit(&cfg, STRV("name"), STRV(""), &var);
	cfg_add_var(&cfg, cpkg, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_oom)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cpkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &cpkg);
	cfg_add_var(&cfg, root, cpkg);
	cfg_lit(&cfg, STRV("name"), STRV(""), &var);
	cfg_add_var(&cfg, cpkg, var);

	mem_oom(1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	mem_oom(0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_exist)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	proj_add_pkg(&proj, STRV(""), NULL)->inited = 1;

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cpkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &cpkg);
	cfg_add_var(&cfg, root, cpkg);
	cfg_lit(&cfg, STRV("name"), STRV(""), &var);
	cfg_add_var(&cfg, cpkg, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_not_inited)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	proj_add_pkg(&proj, STRV("p"), NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cpkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &cpkg);
	cfg_add_var(&cfg, root, cpkg);
	cfg_lit(&cfg, STRV("name"), STRV("p"), &var);
	cfg_add_var(&cfg, cpkg, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 0);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_rename)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cpkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &cpkg);
	cfg_add_var(&cfg, root, cpkg);
	cfg_lit(&cfg, STRV("name"), STRV("p"), &var);
	cfg_add_var(&cfg, cpkg, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);
	pkg_t *pkg  = proj_get_pkg(&proj, 0);
	strv_t name = proj_get_str(&proj, pkg->strs + PKG_NAME);
	EXPECT_STRN(name.data, "p", name.len);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_no_name)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cpkg;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &cpkg);
	cfg_add_var(&cfg, root, cpkg);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_no_src)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, pkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &pkg);
	cfg_add_var(&cfg, root, pkg);
	cfg_str(&cfg, STRV("src"), STRV("src"), &var);
	cfg_add_var(&cfg, pkg, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_src)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 10, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cpkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &cpkg);
	cfg_add_var(&cfg, root, cpkg);
	cfg_str(&cfg, STRV("src"), STRV("src"), &var);
	cfg_add_var(&cfg, cpkg, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, &fs, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);
	pkg_t *pkg = proj_get_pkg(&proj, 0);
	strv_t src = proj_get_str(&proj, pkg->strs + PKG_SRC);
	EXPECT_STRN(src.data, "src", src.len);
	target_t *target = proj_get_target(&proj, 0);
	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	cfg_free(&cfg);
	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_cfg_pkg_no_inc)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, pkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &pkg);
	cfg_add_var(&cfg, root, pkg);
	cfg_str(&cfg, STRV("include"), STRV("include"), &var);
	cfg_add_var(&cfg, pkg, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_inc)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 10, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("include"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cpkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &cpkg);
	cfg_add_var(&cfg, root, cpkg);
	cfg_str(&cfg, STRV("include"), STRV("include"), &var);
	cfg_add_var(&cfg, cpkg, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, &fs, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);
	pkg_t *pkg = proj_get_pkg(&proj, 0);
	strv_t inc = proj_get_str(&proj, pkg->strs + PKG_INC);
	EXPECT_STRN(inc.data, "include", inc.len);
	target_t *target = proj_get_target(&proj, 0);
	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	cfg_free(&cfg);
	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_cfg_pkg_no_test)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, pkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &pkg);
	cfg_add_var(&cfg, root, pkg);
	cfg_str(&cfg, STRV("test"), STRV("test"), &var);
	cfg_add_var(&cfg, pkg, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_test)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 10, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("test"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, cpkg, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &cpkg);
	cfg_add_var(&cfg, root, cpkg);
	cfg_str(&cfg, STRV("test"), STRV("test"), &var);
	cfg_add_var(&cfg, cpkg, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, &fs, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);
	pkg_t *pkg  = proj_get_pkg(&proj, 0);
	strv_t test = proj_get_str(&proj, pkg->strs + PKG_TST);
	EXPECT_STRN(test.data, "test", test.len);
	target_t *target = proj_get_target(&proj, 1);
	EXPECT_EQ(target->type, TARGET_TYPE_TST);

	cfg_free(&cfg);
	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(proj_cfg_pkg_deps_type)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, pkg, deps, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &pkg);
	cfg_add_var(&cfg, root, pkg);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, pkg, deps);
	cfg_str(&cfg, STRV_NULL, STRV("p"), &var);
	cfg_add_var(&cfg, deps, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_deps_oom)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, pkg, deps, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &pkg);
	cfg_add_var(&cfg, root, pkg);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, pkg, deps);
	cfg_lit(&cfg, STRV_NULL, STRV("p"), &var);
	cfg_add_var(&cfg, deps, var);

	mem_oom(1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	mem_oom(0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_deps)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, pkg, deps, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &pkg);
	cfg_add_var(&cfg, root, pkg);
	cfg_arr(&cfg, STRV("deps"), &deps);
	cfg_add_var(&cfg, pkg, deps);
	cfg_lit(&cfg, STRV_NULL, STRV("p"), &var);
	cfg_add_var(&cfg, deps, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);

	char buf[64] = {0};
	proj_print_deps(&proj, DST_BUF(buf));
	EXPECT_STR(buf,
		   "\n"
		   "  : p:p\n"
		   "p\n"
		   "  p:\n");

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg)
{
	SSTART;

	RUN(proj_cfg_pkg_empty);
	RUN(proj_cfg_pkg_add);
	RUN(proj_cfg_pkg_oom);
	RUN(proj_cfg_pkg_exist);
	RUN(proj_cfg_pkg_not_inited);
	RUN(proj_cfg_pkg_rename);
	RUN(proj_cfg_pkg_no_name);
	RUN(proj_cfg_pkg_no_src);
	RUN(proj_cfg_pkg_src);
	RUN(proj_cfg_pkg_no_inc);
	RUN(proj_cfg_pkg_inc);
	RUN(proj_cfg_pkg_no_test);
	RUN(proj_cfg_pkg_test);
	RUN(proj_cfg_pkg_deps_type);
	RUN(proj_cfg_pkg_deps_oom);
	RUN(proj_cfg_pkg_deps);

	SEND;
}

TEST(proj_cfg_target_no_pkg)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("target"), &var);
	cfg_add_var(&cfg, root, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &var);
	cfg_add_var(&cfg, root, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_add)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var, ctarget;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &ctarget);
	cfg_add_var(&cfg, root, ctarget);
	cfg_lit(&cfg, STRV("name"), STRV("target"), &var);
	cfg_add_var(&cfg, ctarget, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);
	target_t *target = proj_get_target(&proj, 0);
	strv_t name	 = proj_get_str(&proj, target->strs + TARGET_NAME);
	EXPECT_STRN(name.data, "", name.len);
	target = proj_get_target(&proj, 1);
	name   = proj_get_str(&proj, target->strs + TARGET_NAME);
	EXPECT_STRN(name.data, "target", name.len);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_oom)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var, ctarget;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &ctarget);
	cfg_add_var(&cfg, root, ctarget);
	cfg_lit(&cfg, STRV("name"), STRV("target"), &var);
	cfg_add_var(&cfg, ctarget, var);

	mem_oom(1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	mem_oom(0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_exist)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var, target;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &target);
	cfg_add_var(&cfg, root, target);
	cfg_lit(&cfg, STRV("name"), STRV(""), &var);
	cfg_add_var(&cfg, target, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_not_inited)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	proj_add_pkg(&proj, STRV("p"), NULL);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var, target;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &target);
	cfg_add_var(&cfg, root, target);
	cfg_lit(&cfg, STRV("name"), STRV("p"), &var);
	cfg_add_var(&cfg, target, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_rename)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var, ctarget;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &ctarget);
	cfg_add_var(&cfg, root, ctarget);
	cfg_lit(&cfg, STRV("name"), STRV("target"), &var);
	cfg_add_var(&cfg, ctarget, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);
	target_t *target = proj_get_target(&proj, 0);
	strv_t name	 = proj_get_str(&proj, target->strs + TARGET_NAME);
	EXPECT_STRN(name.data, "target", name.len);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_no_name)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &var);
	cfg_add_var(&cfg, root, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_type_exe)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var, ctarget;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &ctarget);
	cfg_add_var(&cfg, root, ctarget);
	cfg_lit(&cfg, STRV("type"), STRV("EXE"), &var);
	cfg_add_var(&cfg, ctarget, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);
	target_t *target = proj_get_target(&proj, 0);
	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_type_lib)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var, ctarget;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &ctarget);
	cfg_add_var(&cfg, root, ctarget);
	cfg_lit(&cfg, STRV("type"), STRV("LIB"), &var);
	cfg_add_var(&cfg, ctarget, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 0);
	target_t *target = proj_get_target(&proj, 0);
	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_type_unknown)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, var, ctarget;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("pkg"), &var);
	cfg_add_var(&cfg, root, var);
	cfg_tbl(&cfg, STRV("target"), &ctarget);
	cfg_add_var(&cfg, root, ctarget);
	cfg_lit(&cfg, STRV("type"), STRV("UNKNOWN"), &var);
	cfg_add_var(&cfg, ctarget, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV(""), NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

STEST(proj_cfg_target)
{
	SSTART;

	RUN(proj_cfg_target_no_pkg);
	RUN(proj_cfg_target_empty);
	RUN(proj_cfg_target_add);
	RUN(proj_cfg_target_oom);
	RUN(proj_cfg_target_exist);
	RUN(proj_cfg_target_not_inited);
	RUN(proj_cfg_target_rename);
	RUN(proj_cfg_target_no_name);
	RUN(proj_cfg_target_type_exe);
	RUN(proj_cfg_target_type_lib);
	RUN(proj_cfg_target_type_unknown);

	SEND;
}

TEST(proj_cfg_ext_type)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, ext, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &ext);
	cfg_add_var(&cfg, root, ext);
	cfg_lit(&cfg, STRV("pkg"), STRV("url"), &var);
	cfg_add_var(&cfg, ext, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_ext_oom)
{
	START;

	proj_t proj = {0};
	log_set_quiet(0, 1);
	proj_init(&proj, 0, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, ext, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &ext);
	cfg_add_var(&cfg, root, ext);
	cfg_str(&cfg, STRV("pkg"), STRV("git:repo"), &var);
	cfg_add_var(&cfg, ext, var);

	mem_oom(1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	mem_oom(0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_ext_format)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, ext, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &ext);
	cfg_add_var(&cfg, root, ext);
	cfg_str(&cfg, STRV("pkg"), STRV("git"), &var);
	cfg_add_var(&cfg, ext, var);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_ext_uri)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	cfg_t cfg = {0};
	cfg_init(&cfg, 1, 1, ALLOC_STD);

	cfg_var_t root, ext, var;
	cfg_root(&cfg, &root);
	cfg_tbl(&cfg, STRV("ext"), &ext);
	cfg_add_var(&cfg, root, ext);
	cfg_str(&cfg, STRV("pkg"), STRV("git:repo"), &var);
	cfg_add_var(&cfg, ext, var);

	EXPECT_EQ(proj_cfg(&proj, &cfg, root, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD), 0);

	cfg_free(&cfg);
	proj_free(&proj);

	END;
}

STEST(proj_cfg_ext)
{
	SSTART;

	RUN(proj_cfg_ext_type);
	RUN(proj_cfg_ext_oom);
	RUN(proj_cfg_ext_format);
	RUN(proj_cfg_ext_uri);

	SEND;
}

STEST(proj_cfg)
{
	SSTART;

	RUN(proj_cfg_null);
	RUN(proj_cfg_empty);
	RUN(proj_cfg_oom);
	RUN(proj_cfg_src);
	RUN(proj_cfg_include);
	RUN(proj_cfg_test);
	RUN(proj_cfg_pkg);
	RUN(proj_cfg_target);
	RUN(proj_cfg_ext);

	SEND;
}
