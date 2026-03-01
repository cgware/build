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

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("pkg"));

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_exe(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_ext(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &pkg_id);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI, STRV("url"));
	pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	pkg->uri.ext   = PKG_URI_EXT_ZIP;

	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_EXT;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_proj_test(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("pkg"));
	target->type = TARGET_TYPE_TST;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	pkg_t *pkg;

	pkg = proj_add_pkg(&com->proj, NULL);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_NAME, STRV("pkg"));

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_exe(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);
	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_SRC, STRV("src"));
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_exe_inc_priv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id, tgt;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, &tgt);
	proj_add_inc_priv(&com->proj, tgt, STRV("src"));
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_exe_out(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_OUT, STRV("exes"));
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_exe_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 2, ALLOC_STD);

	uint pkg_id, exe, lib;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, &exe);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("exe"));
	target->type = TARGET_TYPE_EXE;

	target = proj_add_target(&com->proj, pkg_id, &lib);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("lib"));
	proj_set_str(&com->proj, target->strs + TGT_STR_INC, STRV("include"));
	target->type = TARGET_TYPE_LIB;

	proj_add_dep(&com->proj, exe, lib);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_exe_drv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 2, ALLOC_STD);

	uint pkg_id, exe_id, drv_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, &exe_id);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("exe"));
	target->type = TARGET_TYPE_EXE;

	target = proj_add_target(&com->proj, pkg_id, &drv_id);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("drv"));
	proj_set_str(&com->proj, target->strs + TGT_STR_INC, STRV("include"));
	target->type = TARGET_TYPE_DRV;

	proj_add_dep(&com->proj, exe_id, drv_id);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);
	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_SRC, STRV("src"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_inc(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_INC, STRV("include"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_inc_priv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id, tgt;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, &tgt);
	proj_add_inc_priv(&com->proj, tgt, STRV("src"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_out(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_OUT, STRV("libs"));
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_lib_drv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 2, ALLOC_STD);

	uint pkg_id, lib_id, drv_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, &lib_id);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("lib"));
	target->type = TARGET_TYPE_LIB;

	target = proj_add_target(&com->proj, pkg_id, &drv_id);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("drv"));
	proj_set_str(&com->proj, target->strs + TGT_STR_INC, STRV("include"));
	target->type = TARGET_TYPE_DRV;

	proj_add_dep(&com->proj, lib_id, drv_id);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_drv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);
	proj_set_str(&com->proj, com->proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_SRC, STRV("drivers"));
	target->type = TARGET_TYPE_DRV;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_drv_inc(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id, tgt;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, &tgt);
	proj_set_str(&com->proj, target->strs + TGT_STR_INC, STRV("include"));
	target->type = TARGET_TYPE_DRV;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_drv_inc_priv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id, tgt;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, &tgt);
	proj_add_inc_priv(&com->proj, tgt, STRV("drivers"));
	target->type = TARGET_TYPE_DRV;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_drv_out(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_OUT, STRV("libs"));
	target->type = TARGET_TYPE_DRV;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_drv_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 4, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 3, ALLOC_STD);

	uint pkg_id, lib, drv;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);

	target = proj_add_target(&com->proj, pkg_id, &lib);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("lib"));
	proj_set_str(&com->proj, target->strs + TGT_STR_INC, STRV("include"));
	target->type = TARGET_TYPE_LIB;

	target = proj_add_target(&com->proj, pkg_id, &drv);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("drv"));
	target->type = TARGET_TYPE_DRV;

	proj_add_dep(&com->proj, drv, lib);

	gen_driver_t d = *gen_find_param(p);

	d.fs = &com->fs;

	return d.gen(&d, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_test(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_SRC, STRV("test"));
	target->type = TARGET_TYPE_TST;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_test_inc_priv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id, tgt;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, &tgt);
	proj_add_inc_priv(&com->proj, tgt, STRV("test"));
	target->type = TARGET_TYPE_TST;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_test_out(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_OUT, STRV("tests"));
	target->type = TARGET_TYPE_TST;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_test_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id, lib, tst;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, &lib);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("lib"));
	proj_set_str(&com->proj, target->strs + TGT_STR_INC, STRV("include"));
	target->type = TARGET_TYPE_LIB;

	target = proj_add_target(&com->proj, pkg_id, &tst);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("test"));
	target->type = TARGET_TYPE_TST;

	proj_add_dep(&com->proj, tst, lib);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_test_drv(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 2, ALLOC_STD);

	uint pkg_id, tst_id, drv_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target	     = proj_add_target(&com->proj, pkg_id, &tst_id);
	target->type = TARGET_TYPE_TST;
	target	     = proj_add_target(&com->proj, pkg_id, &drv_id);
	target->type = TARGET_TYPE_DRV;

	proj_add_dep(&com->proj, tst_id, drv_id);

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
	proj_set_str(&com->proj, pkg->strs + PKG_STR_NAME, STRV("a"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_PATH, STRV("a"));
	target	     = proj_add_target(&com->proj, a, NULL);
	target->type = TARGET_TYPE_EXE;

	pkg = proj_add_pkg(&com->proj, &b);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_NAME, STRV("b"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_PATH, STRV("b"));
	target	     = proj_add_target(&com->proj, b, NULL);
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
	proj_set_str(&com->proj, pkg->strs + PKG_STR_NAME, STRV("lib"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_PATH, STRV("lib"));
	target	     = proj_add_target(&com->proj, lib, &lib);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_NAME, STRV("exe"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_PATH, STRV("exe"));
	target	     = proj_add_target(&com->proj, exe, &exe);
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
	proj_set_str(&com->proj, pkg->strs + PKG_STR_NAME, STRV("base"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_PATH, STRV("base"));
	target	     = proj_add_target(&com->proj, base, &base);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, &lib1);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_NAME, STRV("lib1"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_PATH, STRV("lib1"));
	target	     = proj_add_target(&com->proj, lib1, &lib1);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, &lib2);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_NAME, STRV("lib2"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_PATH, STRV("lib2"));
	target	     = proj_add_target(&com->proj, lib2, &lib2);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&com->proj, &exe);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_NAME, STRV("exe"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_PATH, STRV("exe"));
	target	     = proj_add_target(&com->proj, exe, &exe);
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
	target_t *target;

	proj_add_pkg(&com->proj, &ext);
	target	     = proj_add_target(&com->proj, ext, NULL);
	target->type = TARGET_TYPE_EXT;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_uri(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &pkg_id);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI, STRV("url"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_FILE, STRV("file"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_NAME, STRV("name"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_DIR, STRV("main"));

	target	     = proj_add_target(&com->proj, pkg_id, NULL);
	target->type = TARGET_TYPE_EXT;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_cmd(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);

	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_PREP, STRV("prep"));
	proj_set_str(&com->proj, target->strs + TGT_STR_CONF, STRV("conf"));
	proj_set_str(&com->proj, target->strs + TGT_STR_COMP, STRV("comp"));
	proj_set_str(&com->proj, target->strs + TGT_STR_INST, STRV("inst"));
	proj_set_str(&com->proj, target->strs + TGT_STR_TGT, STRV("out"));
	target->type = TARGET_TYPE_EXT;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_out(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_OUT, STRV("exts"));
	target->type	 = TARGET_TYPE_EXT;
	target->out_type = TARGET_TGT_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_inc(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target = proj_add_target(&com->proj, pkg_id, NULL);
	proj_set_str(&com->proj, target->strs + TGT_STR_INC, STRV("include"));
	target->type	 = TARGET_TYPE_EXT;
	target->out_type = TARGET_TGT_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target		 = proj_add_target(&com->proj, pkg_id, NULL);
	target->type	 = TARGET_TYPE_EXT;
	target->out_type = TARGET_TGT_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_exe(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg_id);
	target		 = proj_add_target(&com->proj, pkg_id, NULL);
	target->type	 = TARGET_TYPE_EXT;
	target->out_type = TARGET_TGT_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_zip(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &pkg_id);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI, STRV("url"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_FILE, STRV("file"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_NAME, STRV("name"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_VER, STRV("1.0"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_DIR, STRV("main"));
	pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	pkg->uri.ext   = PKG_URI_EXT_ZIP;

	target	     = proj_add_target(&com->proj, pkg_id, NULL);
	target->type = TARGET_TYPE_EXT;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_tar(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&com->proj, &pkg_id);
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI, STRV("url"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_FILE, STRV("file"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_NAME, STRV("name"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_VER, STRV("1.0"));
	proj_set_str(&com->proj, pkg->strs + PKG_STR_URI_DIR, STRV("main"));
	pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	pkg->uri.ext   = PKG_URI_EXT_TAR;

	target	     = proj_add_target(&com->proj, pkg_id, NULL);
	target->type = TARGET_TYPE_EXT;

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_dep_lib(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 2, ALLOC_STD);

	uint pkg, ext, lib;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg);

	target = proj_add_target(&com->proj, pkg, &lib);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("lib"));
	target->type = TARGET_TYPE_LIB;

	target = proj_add_target(&com->proj, pkg, &ext);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("ext"));
	target->type = TARGET_TYPE_EXT;

	proj_add_dep(&com->proj, ext, lib);

	gen_driver_t drv = *gen_find_param(p);

	drv.fs = &com->fs;

	return drv.gen(&drv, &com->proj, STRV_NULL, STRV_NULL);
}

int t_gen_pkg_ext_dep_exe(t_gen_common_t *com, strv_t p)
{
	fs_init(&com->fs, 2, 1, ALLOC_STD);

	proj_init(&com->proj, 1, 2, ALLOC_STD);

	uint pkg, ext, lib;
	target_t *target;

	proj_add_pkg(&com->proj, &pkg);

	target = proj_add_target(&com->proj, pkg, &lib);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("exe"));
	target->type = TARGET_TYPE_EXE;

	target = proj_add_target(&com->proj, pkg, &ext);
	proj_set_str(&com->proj, target->strs + TGT_STR_NAME, STRV("ext"));
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
