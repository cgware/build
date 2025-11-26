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

	proj_set_str(&com->proj, com->proj.name, STRV("proj"));

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_unknown(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg);
	target = proj_add_target(&com->proj, pkg, NULL);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));

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

	proj_add_pkg(&com->proj, &exe);
	target = proj_add_target(&com->proj, exe, &exe);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
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

	proj_add_pkg(&com->proj, &exe);
	target = proj_add_target(&com->proj, exe, &exe);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_ext(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_URI_STR, STRV("url"));
	pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	pkg->uri.ext   = PKG_URI_EXT_ZIP;

	target = proj_add_target(&com->proj, exe, &exe);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_EXT;

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

	proj_add_pkg(&com->proj, &exe);
	target = proj_add_target(&com->proj, exe, &exe);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_TST;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_exe(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("pkg"));
	proj_set_str(&com->proj, pkg->strs + PKG_SRC, STRV("src"));
	target = proj_add_target(&com->proj, exe, &exe);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_exe_drv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("pkg"));
	proj_set_str(&com->proj, pkg->strs + PKG_SRC, STRV("src"));
	proj_set_str(&com->proj, pkg->strs + PKG_DRV, STRV("drivers"));
	target = proj_add_target(&com->proj, exe, &exe);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_exe_drv_inc(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("pkg"));
	proj_set_str(&com->proj, pkg->strs + PKG_SRC, STRV("src"));
	proj_set_str(&com->proj, pkg->strs + PKG_INC, STRV("include"));
	proj_set_str(&com->proj, pkg->strs + PKG_DRV, STRV("drivers"));
	target = proj_add_target(&com->proj, exe, &exe);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint lib;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &lib);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("pkg"));
	proj_set_str(&com->proj, pkg->strs + PKG_SRC, STRV("src"));
	target = proj_add_target(&com->proj, lib, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_inc(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint lib;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &lib);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("pkg"));
	proj_set_str(&com->proj, pkg->strs + PKG_SRC, STRV("src"));
	proj_set_str(&com->proj, pkg->strs + PKG_INC, STRV("include"));
	target = proj_add_target(&com->proj, lib, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_drv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint lib;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &lib);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("pkg"));
	proj_set_str(&com->proj, pkg->strs + PKG_SRC, STRV("src"));
	proj_set_str(&com->proj, pkg->strs + PKG_DRV, STRV("drivers"));
	target = proj_add_target(&com->proj, lib, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_drv_inc(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint lib;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &lib);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("pkg"));
	proj_set_str(&com->proj, pkg->strs + PKG_SRC, STRV("src"));
	proj_set_str(&com->proj, pkg->strs + PKG_INC, STRV("include"));
	proj_set_str(&com->proj, pkg->strs + PKG_DRV, STRV("drivers"));
	target = proj_add_target(&com->proj, lib, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_test(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint pkg_id, lib, tst;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &pkg_id);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("lib"));
	target = proj_add_target(&com->proj, pkg_id, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("lib"));
	target->type = TARGET_TYPE_LIB;

	proj_set_str(&com->proj, pkg->strs + PKG_TST, STRV("test"));
	target = proj_add_target(&com->proj, pkg_id, &tst);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("test"));
	target->type = TARGET_TYPE_TST;

	proj_add_dep(&com->proj, tst, lib);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_test_inc(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint pkg_id, lib, tst;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &pkg_id);
	proj_set_str(&com->proj, pkg->strs + PKG_INC, STRV("include"));

	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("lib"));
	target = proj_add_target(&com->proj, pkg_id, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("lib"));
	target->type = TARGET_TYPE_LIB;

	proj_set_str(&com->proj, pkg->strs + PKG_TST, STRV("test"));
	target = proj_add_target(&com->proj, pkg_id, &tst);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("test"));
	target->type = TARGET_TYPE_TST;

	proj_add_dep(&com->proj, tst, lib);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_test_drv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint pkg_id, lib, tst;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &pkg_id);
	proj_set_str(&com->proj, pkg->strs + PKG_DRV, STRV("drivers"));

	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("lib"));
	target = proj_add_target(&com->proj, pkg_id, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("lib"));
	target->type = TARGET_TYPE_LIB;

	proj_set_str(&com->proj, pkg->strs + PKG_TST, STRV("test"));
	target = proj_add_target(&com->proj, pkg_id, &tst);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("test"));
	target->type = TARGET_TYPE_TST;

	proj_add_dep(&com->proj, tst, lib);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_test_drv_inc(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint pkg_id, lib, tst;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &pkg_id);
	proj_set_str(&com->proj, pkg->strs + PKG_INC, STRV("include"));
	proj_set_str(&com->proj, pkg->strs + PKG_DRV, STRV("drivers"));

	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("lib"));
	target = proj_add_target(&com->proj, pkg_id, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("lib"));
	target->type = TARGET_TYPE_LIB;

	proj_set_str(&com->proj, pkg->strs + PKG_TST, STRV("test"));
	target = proj_add_target(&com->proj, pkg_id, &tst);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("test"));
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

	pkg = proj_add_pkg(&com->proj, &a);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("a"));
	proj_set_str(&com->proj, pkg->strs + PKG_PATH, STRV("a"));
	target = proj_add_target(&com->proj, a, &a);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("a"));
	target->type = TARGET_TYPE_EXE;

	pkg = proj_add_pkg(&com->proj, &b);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("b"));
	proj_set_str(&com->proj, pkg->strs + PKG_PATH, STRV("b"));
	target = proj_add_target(&com->proj, b, &b);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("b"));
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

	pkg = proj_add_pkg(&com->proj, &lib);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("lib"));
	proj_set_str(&com->proj, pkg->strs + PKG_PATH, STRV("lib"));
	target = proj_add_target(&com->proj, lib, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("lib"));
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("exe"));
	proj_set_str(&com->proj, pkg->strs + PKG_PATH, STRV("exe"));
	target = proj_add_target(&com->proj, exe, &exe);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("exe"));
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

	pkg = proj_add_pkg(&com->proj, &base);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("base"));
	proj_set_str(&com->proj, pkg->strs + PKG_PATH, STRV("base"));
	target = proj_add_target(&com->proj, base, &base);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("base"));
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, &lib1);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("lib1"));
	proj_set_str(&com->proj, pkg->strs + PKG_PATH, STRV("lib1"));
	target = proj_add_target(&com->proj, lib1, &lib1);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("lib1"));
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, &lib2);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("lib2"));
	proj_set_str(&com->proj, pkg->strs + PKG_PATH, STRV("lib2"));
	target = proj_add_target(&com->proj, lib2, &lib2);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("lib2"));
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("exe"));
	proj_set_str(&com->proj, pkg->strs + PKG_PATH, STRV("exe"));
	target = proj_add_target(&com->proj, exe, &exe);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("exe"));
	target->type = TARGET_TYPE_EXE;

	proj_add_dep(&com->proj, lib1, base);
	proj_add_dep(&com->proj, lib2, base);
	proj_add_dep(&com->proj, exe, lib1);
	proj_add_dep(&com->proj, exe, lib2);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV("."), STRV("."));
}

int t_gen_pkg_ext_unknown(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint ext;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &ext);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("pkg"));
	proj_set_str(&com->proj, pkg->strs + PKG_URI_STR, STRV("url"));
	proj_set_str(&com->proj, pkg->strs + PKG_URI_DIR, STRV("main"));
	proj_set_str(&com->proj, pkg->strs + PKG_URI_NAME, STRV("archive"));
	pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	pkg->uri.ext   = PKG_URI_EXT_UNKNOWN;

	target = proj_add_target(&com->proj, ext, NULL);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	proj_set_str(&com->proj, target->strs + TARGET_CMD, STRV("cmd"));
	proj_set_str(&com->proj, target->strs + TARGET_OUT, STRV("out"));
	proj_set_str(&com->proj, target->strs + TARGET_DST, STRV("dst"));
	target->type = TARGET_TYPE_EXT;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_zip(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint ext;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &ext);
	proj_set_str(&com->proj, pkg->strs + PKG_NAME, STRV("pkg"));
	proj_set_str(&com->proj, pkg->strs + PKG_URI_STR, STRV("url"));
	proj_set_str(&com->proj, pkg->strs + PKG_URI_DIR, STRV("main"));
	proj_set_str(&com->proj, pkg->strs + PKG_URI_NAME, STRV("archive"));
	pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	pkg->uri.ext   = PKG_URI_EXT_ZIP;

	target = proj_add_target(&com->proj, ext, NULL);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("pkg"));
	proj_set_str(&com->proj, target->strs + TARGET_CMD, STRV("cmd"));
	proj_set_str(&com->proj, target->strs + TARGET_OUT, STRV("out"));
	proj_set_str(&com->proj, target->strs + TARGET_DST, STRV("dst"));
	target->type = TARGET_TYPE_EXT;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_deps(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 2, ALLOC_STD);

	uint pkg, ext, lib;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg);

	target = proj_add_target(&com->proj, pkg, &lib);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("lib"));
	target->type = TARGET_TYPE_LIB;

	target = proj_add_target(&com->proj, pkg, &ext);
	proj_set_str(&com->proj, target->strs + TARGET_NAME, STRV("ext"));
	target->type = TARGET_TYPE_EXT;

	proj_add_dep(&com->proj, ext, lib);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

void t_gen_free(t_gen_common_t *com)
{
	proj_free(&com->proj);
	fs_free(&com->fs);
}
