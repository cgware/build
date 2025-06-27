#include "t_gen_common.h"

int t_gen_proj_build_dir(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 3, 1, ALLOC_STD);

	fs_mkpath(&com->fs, STRV_NULL, STRV("tmp/build"));

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV(""), STRV("tmp/build/"));
}

int t_gen_proj_empty(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 1, 1, ALLOC_STD);

	fs_mkpath(&com->fs, STRV_NULL, STRV_NULL);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_name(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 1, 1, ALLOC_STD);

	fs_mkpath(&com->fs, STRV_NULL, STRV_NULL);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	com->proj.name = STRV("proj");

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_unknown(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg;
	proj_add_pkg(&com->proj, STRV_NULL, &pkg);
	proj_add_target(&com->proj, pkg, STRV("pkg"), NULL);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_exe(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint exe;
	target_t *target;

	proj_add_pkg(&com->proj, STRV_NULL, &exe);
	target	     = proj_add_target(&com->proj, exe, STRV("pkg"), &exe);
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint exe;
	target_t *target;

	proj_add_pkg(&com->proj, STRV_NULL, &exe);
	target	     = proj_add_target(&com->proj, exe, STRV("pkg"), &exe);
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_test(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint exe;
	target_t *target;

	proj_add_pkg(&com->proj, STRV_NULL, &exe);
	target	     = proj_add_target(&com->proj, exe, STRV("pkg"), &exe);
	target->type = TARGET_TYPE_TST;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_exe(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	path_init(&com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, STRV("pkg"), &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_SRC, STRV("src"));
	target	     = proj_add_target(&com->proj, exe, STRV("pkg"), &exe);
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	path_init(&com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint lib;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, STRV("pkg"), &lib);
	proj_set_str(&com->proj, pkg->strs + PKG_SRC, STRV("src"));
	proj_set_str(&com->proj, pkg->strs + PKG_INC, STRV("include"));
	target	     = proj_add_target(&com->proj, lib, STRV("pkg"), &lib);
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_test(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	path_init(&com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint pkg_id, lib, tst;
	pkg_t *pkg;
	target_t *target;

	pkg	     = proj_add_pkg(&com->proj, STRV("lib"), &pkg_id);
	target	     = proj_add_target(&com->proj, pkg_id, STRV("lib"), &lib);
	target->type = TARGET_TYPE_LIB;

	proj_set_str(&com->proj, pkg->strs + PKG_TST, STRV("test"));
	target	     = proj_add_target(&com->proj, pkg_id, STRV("test"), &tst);
	target->type = TARGET_TYPE_TST;

	proj_add_dep(&com->proj, tst, lib);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_multi(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 6, 1, ALLOC_STD);

	fs_mkpath(&com->fs, STRV_NULL, STRV("./a"));
	fs_mkpath(&com->fs, STRV_NULL, STRV("./b"));

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint a, b;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, STRV("a"), &a);
	proj_set_str(&com->proj, pkg->strs + PKG_DIR, STRV("a"));
	target	     = proj_add_target(&com->proj, a, STRV("a"), &a);
	target->type = TARGET_TYPE_EXE;

	pkg = proj_add_pkg(&com->proj, STRV("b"), &b);
	proj_set_str(&com->proj, pkg->strs + PKG_DIR, STRV("b"));
	target	     = proj_add_target(&com->proj, b, STRV("b"), &b);
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV("."), STRV("."));
}

int t_gen_pkg_depends(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 6, 1, ALLOC_STD);

	fs_mkpath(&com->fs, STRV_NULL, STRV("./lib"));
	fs_mkpath(&com->fs, STRV_NULL, STRV("./exe"));

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint lib, exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, STRV("lib"), &lib);
	proj_set_str(&com->proj, pkg->strs + PKG_DIR, STRV("lib"));
	target	     = proj_add_target(&com->proj, lib, STRV("lib"), &lib);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, STRV("exe"), &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_DIR, STRV("exe"));
	target	     = proj_add_target(&com->proj, exe, STRV("exe"), &exe);
	target->type = TARGET_TYPE_EXE;

	proj_add_dep(&com->proj, exe, lib);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV("."), STRV("."));
}

int t_gen_pkg_rdepends(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 6, 1, ALLOC_STD);

	fs_mkpath(&com->fs, STRV_NULL, STRV("./lib"));
	fs_mkpath(&com->fs, STRV_NULL, STRV("./exe"));

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint base, lib1, lib2, exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, STRV("base"), &base);
	proj_set_str(&com->proj, pkg->strs + PKG_DIR, STRV("base"));
	target	     = proj_add_target(&com->proj, base, STRV("base"), &base);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, STRV("lib1"), &lib1);
	proj_set_str(&com->proj, pkg->strs + PKG_DIR, STRV("lib1"));
	target	     = proj_add_target(&com->proj, lib1, STRV("lib1"), &lib1);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, STRV("lib2"), &lib2);
	proj_set_str(&com->proj, pkg->strs + PKG_DIR, STRV("lib2"));
	target	     = proj_add_target(&com->proj, lib2, STRV("lib2"), &lib2);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, STRV("exe"), &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_DIR, STRV("exe"));
	target	     = proj_add_target(&com->proj, exe, STRV("exe"), &exe);
	target->type = TARGET_TYPE_EXE;

	proj_add_dep(&com->proj, lib1, base);
	proj_add_dep(&com->proj, lib2, base);
	proj_add_dep(&com->proj, exe, lib1);
	proj_add_dep(&com->proj, exe, lib2);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV("."), STRV("."));
}

void t_gen_free(t_gen_common_t *com)
{
	proj_free(&com->proj);
	fs_free(&com->fs);
}
