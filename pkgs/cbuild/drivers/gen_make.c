#include "gen.h"

#include "file/make.h"
#include "log.h"
#include "mem.h"
#include "path.h"
#include "vars.h"

typedef struct defines_s {
	strv_t name;
	make_act_t def;
} defines_t;

static void resolve_var(const vars_t *vars, strv_t var, const strv_t *values, str_t *buf)
{
	buf->len = 0;
	str_cat(buf, var);
	vars_replace(vars, buf, values);
}

static int gen_pkg(const proj_t *proj, const vars_t *vars, make_t *make, fs_t *fs, uint id, make_act_t inc, const defines_t *protos_defs,
		   const defines_t *exts_defs, const defines_t *defines, arr_t *deps, str_t *buf, strv_t build_dir)

{
	const pkg_t *pkg = proj_get_pkg(proj, id);

	path_t path = {0};
	path_init(&path, build_dir);
	strv_t pkg_dir = proj_get_str(proj, pkg->strs + PKG_PATH);
	if (pkg_dir.len > 0) {
		fs_mkpath(fs, STRVS(path), pkg_dir);
	}
	path_push(&path, pkg_dir);
	path_push(&path, STRV("pkg.mk"));

	log_info("cbuild", "gen_make", NULL, "generating package: '%.*s'", path.len, path.data);

	make_act_t act;
	make_var(make, STRV("PN"), MAKE_VAR_INST, &act);
	make_var_add_val(make, act, MSTR(proj_get_str(proj, pkg->strs + PKG_NAME)));
	make_inc_add_act(make, inc, act);

	make_var(make, STRV("$(PN)_DIR"), MAKE_VAR_INST, &act);
	strv_t dir = proj_get_str(proj, pkg->strs + PKG_PATH);
	if (dir.len > 0) {
		path_t tmp = {0};
		path_init(&tmp, dir);
		path_push(&tmp, STRV(""));
		make_var_add_val(make, act, MSTR(STRVS(tmp)));
	}
	make_inc_add_act(make, inc, act);

	strv_t uri = proj_get_str(proj, pkg->strs + PKG_URI_STR);
	if (uri.len > 0) {
		make_var(make, STRV("$(PN)_URI"), MAKE_VAR_INST, &act);
		make_var_add_val(make, act, MSTR(uri));
		make_inc_add_act(make, inc, act);

		strv_t uri_file = proj_get_str(proj, pkg->strs + PKG_URI_NAME);
		buf->len	= 0;
		str_cat(buf, uri_file);
		if (uri_file.len > 0) {
			switch (pkg->uri.ext) {
			case PKG_URI_EXT_ZIP:
				str_cat(buf, STRV(".zip"));
				break;
			default:
				break;
			}
		}
		make_var(make, STRV("$(PN)_DLFILE"), MAKE_VAR_INST, &act);
		make_var_add_val(make, act, MSTR(STRVS(*buf)));

		make_inc_add_act(make, inc, act);

		strv_t uri_root = proj_get_str(proj, pkg->strs + PKG_URI_DIR);
		if (uri_root.len > 0) {
			make_var(make, STRV("$(PN)_DLROOT"), MAKE_VAR_INST, &act);
			make_var_add_val(make, act, MSTR(uri_root));
			make_inc_add_act(make, inc, act);
		}

		make_eval_def(make, protos_defs[pkg->uri.proto].def, &act);
		make_inc_add_act(make, inc, act);

		make_eval_def(make, exts_defs[pkg->uri.ext].def, &act);
		make_inc_add_act(make, inc, act);
	}

	strv_t svalues[__VARS_CNT] = {
		[ARCH]	 = STRVT("$(ARCH)"),
		[CONFIG] = STRVT("$(CONFIG)"),
	};

	uint i = pkg->targets;
	const target_t *target;
	list_foreach(&proj->targets, i, target)
	{
		make_var(make, STRV("TN"), MAKE_VAR_INST, &act);
		make_var_add_val(make, act, MSTR(proj_get_str(proj, target->strs + TARGET_NAME)));
		make_inc_add_act(make, inc, act);

		if (target->type != TARGET_TYPE_EXT) {

			make_var(make, STRV("$(PN)_$(TN)_HEADERS"), MAKE_VAR_INST, &act);
			strv_t include = proj_get_str(proj, pkg->strs + PKG_INC);
			if (include.len > 0) {
				make_var_add_val(make, act, MSTR(STRV("$(PKGINC_H)")));
			}
			make_inc_add_act(make, inc, act);
			make_var(make, STRV("$(PN)_$(TN)_INCLUDES"), MAKE_VAR_INST, &act);
			if (include.len > 0) {
				buf->len = 0;
				str_cat(buf, STRV("$(DIR_PKG)"));
				str_cat(buf, include);
				make_var_add_val(make, act, MSTR(STRVS(*buf)));
			}
			make_inc_add_act(make, inc, act);
		}

		switch (target->type) {
		case TARGET_TYPE_EXE:
		case TARGET_TYPE_TST: {
			make_act_t libs;
			make_var(make, STRV("$(PN)_$(TN)_LIBS"), MAKE_VAR_INST, &libs);
			make_inc_add_act(make, inc, libs);

			deps->cnt = 0;
			proj_get_deps(proj, i, deps);
			if (deps->cnt > 0) {
				buf->len = 0;

				str_cat(buf, STRV("$("));
				size_t buf_len = buf->len;

				uint i = 0;
				const uint *dep;
				arr_foreach(deps, i, dep)
				{
					const target_t *dtarget = proj_get_target(proj, *dep);
					const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
					str_cat(buf, proj_get_str(proj, dpkg->strs + PKG_NAME));
					str_cat(buf, STRV("_"));
					str_cat(buf, proj_get_str(proj, dtarget->strs + PKG_NAME));
					str_cat(buf, STRV(")"));
					make_var_add_val(make, libs, MSTR(STRVS(*buf)));
					buf->len = buf_len;
				}
			}

			make_var(make, STRV("$(PN)_$(TN)_DRIVERS"), MAKE_VAR_INST, &act);
			make_inc_add_act(make, inc, act);
			break;
		}
		case TARGET_TYPE_EXT: {
			make_var(make, STRV("$(PN)_$(TN)_CMD"), MAKE_VAR_INST, &act);
			strv_t cmd = proj_get_str(proj, target->strs + TARGET_CMD);
			resolve_var(vars, cmd, svalues, buf);
			if (buf->len > 0) {
				make_var_add_val(make, act, MSTR(STRVS(*buf)));
			}
			make_inc_add_act(make, inc, act);

			make_var(make, STRV("$(PN)_$(TN)_OUT"), MAKE_VAR_INST, &act);
			strv_t out = proj_get_str(proj, target->strs + TARGET_OUT);
			resolve_var(vars, out, svalues, buf);
			var_convert(buf, '{', '}', '(', ')');
			if (buf->len > 0) {
				make_var_add_val(make, act, MSTR(STRVS(*buf)));
			}
			make_inc_add_act(make, inc, act);
			break;
		}
		default:
			break;
		}

		make_eval_def(make, defines[target->type].def, &act);
		make_inc_add_act(make, inc, act);
	}

	void *file;
	fs_open(fs, STRVS(path), "w", &file);
	make_inc_print(make, inc, DST_FS(fs, file));
	fs_close(fs, file);

	return 0;
}

static int gen_make(const gen_driver_t *drv, const proj_t *proj, strv_t proj_dir, strv_t build_dir)
{
	if (drv == NULL || proj == NULL) {
		return 1;
	}

	path_t path = {0};
	path_init(&path, build_dir);
	path_push(&path, STRV("Makefile"));

	log_info("cbuild", "gen_make", NULL, "generating project: '%.*s'", path.len, path.data);

	vars_t vars = {0};
	vars_init(&vars);

	make_t make = {0};
	make_init(&make, 32, 32, 2, 32, ALLOC_STD);

	make_act_t root, mcurdir, march, mconfig, mopen, act;
	make_var_ext(&make, STRV("CURDIR"), &mcurdir);
	make_ext_set_val(&make, mcurdir, MSTR(STRV(".")));
	root = mcurdir;
	make_var_ext(&make, STRV("ARCH"), &march);
	make_add_act(&make, root, march);
	make_var_ext(&make, STRV("CONFIG"), &mconfig);
	make_add_act(&make, root, mconfig);
	make_var_ext(&make, STRV("OPEN"), &mopen);
	make_add_act(&make, root, mopen);

	make_var_var(&make, march, MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("x64")));
	make_add_act(&make, root, act);

	make_var_var(&make, mconfig, MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("Debug")));
	make_add_act(&make, root, act);

	make_var_var(&make, mopen, MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("1")));
	make_add_act(&make, root, act);
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	path_t tmp = {0};
	str_t buf  = strz(16);

	make_act_t mvars[__VARS_CNT] = {0};

	for (int i = 0; i < __VARS_CNT; i++) {
		strv_t val = vars.vars[i].val;
		switch (i) {
		case DIR_PROJ: {
			path_calc_rel(build_dir, proj_dir, &tmp);
			val = STRVS(tmp);
			break;
		}
		case DIR_OUT: {
			strv_t poutdir = proj_get_str(proj, proj->outdir);
			if (poutdir.len == 0) {
				break;
			}

			buf.len = 0;
			if (pathv_is_rel(poutdir)) {
				str_cat(&buf, STRV("$(DIR_PROJ)"));
			}
			str_cat(&buf, STRVS(poutdir));
			val = STRVS(buf);
			break;
		}
		default:
			break;
		}

		if (val.data == NULL) {
			continue;
		}

		buf.len = 0;
		str_cat(&buf, val);

		var_convert(&buf, '{', '}', '(', ')');

		make_var_type_t type = (vars.vars[i].deps & ((1 << PN) | (1 << TN))) ? MAKE_VAR_REF : MAKE_VAR_INST;
		make_var(&make, vars.vars[i].name, type, &mvars[i]);
		if (buf.len > 0) {
			make_var_add_val(&make, mvars[i], MSTR(STRVS(buf)));
		}
		make_add_act(&make, root, mvars[i]);
	}

	make_var(&make, STRV("EXT_LIB"), MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV(".a")));
	make_add_act(&make, root, act);
	make_var(&make, STRV("EXT_EXE"), MAKE_VAR_INST, &act);
	make_add_act(&make, root, act);

	make_act_t mbits, if_x64, if_x86;
	make_ifeq(&make, MVAR(march), MSTR(STRV("x64")), &if_x64);
	make_add_act(&make, root, if_x64);
	make_var(&make, STRV("BITS"), MAKE_VAR_INST, &mbits);
	make_var_add_val(&make, mbits, MSTR(STRV("64")));
	make_if_add_true_act(&make, if_x64, mbits);
	make_ifeq(&make, MVAR(march), MSTR(STRV("x86")), &if_x86);
	make_add_act(&make, root, if_x86);
	make_var_var(&make, mbits, MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("32")));
	make_if_add_true_act(&make, if_x86, act);
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	make_act_t mcflags, mldflags, if_debug, if_release;
	make_ifeq(&make, MVAR(mconfig), MSTR(STRV("Debug")), &if_debug);
	make_add_act(&make, root, if_debug);
	make_var(&make, STRV("CFLAGS"), MAKE_VAR_INST, &mcflags);
	make_var_add_val(&make, mcflags, MSTR(STRV("-Wall -Wextra -Werror -pedantic -O0 -ggdb -coverage")));
	make_if_add_true_act(&make, if_debug, mcflags);
	make_var(&make, STRV("LDFLAGS"), MAKE_VAR_INST, &mldflags);
	make_var_add_val(&make, mldflags, MSTR(STRV("-coverage")));
	make_if_add_true_act(&make, if_debug, mldflags);
	make_ifeq(&make, MVAR(mconfig), MSTR(STRV("Release")), &if_release);
	make_add_act(&make, root, if_release);
	make_var_var(&make, mcflags, MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("-Wall -Wextra -Werror -pedantic")));
	make_if_add_true_act(&make, if_release, act);
	make_var_var(&make, mldflags, MAKE_VAR_INST, &act);
	make_if_add_true_act(&make, if_release, act);
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	make_var(&make, STRV("TCC"), MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("$(CC)")));
	make_add_act(&make, root, act);
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	enum {
		PKGSRC_C,
		PKGSRC_H,
		PKGDRV_C,
		PKGTST_C,
		PKGTST_H,
		PKGINC_H,
	};

	struct {
		strv_t name;
		strv_t val;
		make_act_t var;
	} pkgfiles[] = {
		[PKGSRC_C] = {STRVT("PKGSRC_C"), STRVT("$(shell find $(DIR_PKG_SRC) -type f -name '*.c')")},
		[PKGSRC_H] = {STRVT("PKGSRC_H"), STRVT("$(shell find $(DIR_PKG_SRC) -type f -name '*.h')")},
		[PKGDRV_C] = {STRVT("PKGDRV_C"), STRVT("$(shell find $(DIR_PKG_DRV) -type f -name '*.c')")},
		[PKGTST_C] = {STRVT("PKGTST_C"), STRVT("$(shell find $(DIR_PKG_TST) -type f -name '*.c')")},
		[PKGTST_H] = {STRVT("PKGTST_H"), STRVT("$(shell find $(DIR_PKG_TST) -type f -name '*.h')")},
		[PKGINC_H] = {STRVT("PKGINC_H"), STRVT("$(shell find $(DIR_PKG_INC) -type f -name '*.h')")},
	};

	for (size_t i = 0; i < sizeof(pkgfiles) / sizeof(pkgfiles[0]); i++) {
		make_var(&make, pkgfiles[i].name, MAKE_VAR_REF, &act);
		make_var_add_val(&make, act, MSTR(pkgfiles[i].val));
		make_add_act(&make, root, act);
		pkgfiles[i].var = act;
	}
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	enum {
		PKGSRC_OBJ,
		PKGSRC_GCDA,
		PKGDRV_OBJ,
		PKGDRV_GCDA,
		PKGTST_OBJ,
		PKGTST_GCDA,
	};

	struct {
		strv_t name;
		strv_t val;
		make_act_t var;
	} objs[] = {
		[PKGSRC_OBJ]  = {STRVT("PKGSRC_OBJ"), STRVT("$(patsubst $(DIR_PKG_SRC)%.c,$(DIR_OUT_INT_SRC)%.o,$(PKGSRC_C))")},
		[PKGSRC_GCDA] = {STRVT("PKGSRC_GCDA"), STRVT("$(patsubst %.o,%.gcda,$(PKGSRC_OBJ))")},
		[PKGDRV_OBJ]  = {STRVT("PKGDRV_OBJ"), STRVT("$(patsubst $(DIR_PKG_DRV)%.c,$(DIR_OUT_INT_DRV)%.o,$(PKGDRV_C))")},
		[PKGDRV_GCDA] = {STRVT("PKGDRV_GCDA"), STRVT("$(patsubst %.o,%.gcda,$(PKGDRV_OBJ))")},
		[PKGTST_OBJ]  = {STRVT("PKGTST_OBJ"), STRVT("$(patsubst $(DIR_PKG_TST)%.c,$(DIR_OUT_INT_TST)%.o,$(PKGTST_C))")},
		[PKGTST_GCDA] = {STRVT("PKGTST_GCDA"), STRVT("$(patsubst %.o,%.gcda,$(PKGTST_OBJ))")},
	};

	for (size_t i = 0; i < sizeof(objs) / sizeof(objs[0]); i++) {
		make_var(&make, objs[i].name, MAKE_VAR_REF, &act);
		make_var_add_val(&make, act, MSTR(objs[i].val));
		make_add_act(&make, root, act);
		objs[i].var = act;
	}
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	make_act_t gcda;
	make_var(&make, STRV("GCDA"), MAKE_VAR_INST, &gcda);
	make_add_act(&make, root, gcda);

	make_empty(&make, &act);
	make_add_act(&make, root, act);

	make_act_t phony;
	make_phony(&make, &phony);
	make_add_act(&make, root, phony);
	make_rule_add_depend(&make, phony, MRULE(MSTR(STRV("all"))));
	make_rule_add_depend(&make, phony, MRULE(MSTR(STRV("test"))));
	make_rule_add_depend(&make, phony, MRULE(MSTR(STRV("cov"))));

	make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &act);
	make_add_act(&make, root, act);

	defines_t protos_defs[] = {
		[PKG_URI_PROTO_UNKNOWN] = {STRVT("fetch_unknown")},
		[PKG_URI_PROTO_GIT]	= {STRVT("fetch_git")},
		[PKG_URI_PROTO_HTTPS]	= {STRVT("fetch_wget")},
	};

	defines_t exts_defs[] = {
		[PKG_URI_EXT_UNKNOWN] = {STRVT("ext_unknown")},
		[PKG_URI_EXT_ZIP]     = {STRVT("ext_zip")},
	};

	defines_t defines[] = {
		[TARGET_TYPE_UNKNOWN] = {STRVT("unknown")},
		[TARGET_TYPE_EXE]     = {STRVT("exe")},
		[TARGET_TYPE_LIB]     = {STRVT("lib")},
		[TARGET_TYPE_EXT]     = {STRVT("ext")},
		[TARGET_TYPE_TST]     = {STRVT("test")},
	};

	int protos[__PKG_URI_PROTO_MAX] = {0};
	int exts[__PKG_URI_EXT_MAX]	= {0};
	int types[__TARGET_TYPE_MAX]	= {0};

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		if (proj_get_str(proj, pkg->strs + PKG_URI_STR).len > 0) {
			protos[pkg->uri.proto] = 1;
			exts[pkg->uri.ext]     = 1;
		}
	}

	i = 0;
	const target_t *target;
	list_foreach_all(&proj->targets, i, target)
	{
		types[target->type] = 1;
	}

	if (protos[PKG_URI_PROTO_HTTPS]) {
		make_act_t def;
		make_def(&make, protos_defs[PKG_URI_PROTO_HTTPS].name, &def);
		make_add_act(&make, root, def);
		protos_defs[PKG_URI_PROTO_HTTPS].def = def;

		make_act_t dl;
		make_rule(&make, MRULE(MSTR(STRV("$(DIR_TMP_DL_PKG)$(PKG_DLFILE)"))), 1, &dl);
		make_def_add_act(&make, def, dl);

		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, dl, act);
		make_cmd(&make, MCMD(STRV("wget $(PKG_URI) -O $$@")), &act);
		make_rule_add_act(&make, dl, act);

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	if (exts[PKG_URI_EXT_UNKNOWN]) {
		make_act_t def;
		make_def(&make, exts_defs[PKG_URI_EXT_UNKNOWN].name, &def);
		make_add_act(&make, root, def);
		exts_defs[PKG_URI_EXT_UNKNOWN].def = def;

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	if (exts[PKG_URI_EXT_ZIP]) {
		make_act_t def;
		make_def(&make, exts_defs[PKG_URI_EXT_ZIP].name, &def);
		make_add_act(&make, root, def);
		exts_defs[PKG_URI_EXT_ZIP].def = def;

		make_act_t ext;
		make_rule(&make, MRULE(MSTR(STRV("$(DIR_TMP_EXT_PKG)"))), 1, &ext);
		make_rule_add_depend(&make, ext, MRULE(MSTR(STRV("$(DIR_TMP_DL_PKG)$(PKG_DLFILE)"))));
		make_def_add_act(&make, def, ext);

		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, ext, act);
		make_cmd(&make, MCMD(STRV("unzip $$< -d $$@")), &act);
		make_rule_add_act(&make, ext, act);

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	if (types[TARGET_TYPE_UNKNOWN]) {
		make_act_t def;
		make_def(&make, defines[TARGET_TYPE_UNKNOWN].name, &def);
		make_add_act(&make, root, def);
		defines[TARGET_TYPE_UNKNOWN].def = def;

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	if (types[TARGET_TYPE_EXE]) {
		make_act_t def;
		make_def(&make, defines[TARGET_TYPE_EXE].name, &def);
		make_add_act(&make, root, def);
		defines[TARGET_TYPE_EXE].def = def;

		make_var(&make, STRV("$(PN)_$(TN)"), MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MVAR(mvars[DIR_OUT_BIN_FILE]));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_var(&make, STRV("GCDA"), MAKE_VAR_APP, &act);
		make_var_add_val(&make, act, MVAR(objs[PKGSRC_GCDA].var));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_act_t def_all;
		make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &def_all);
		make_rule_add_depend(&make, def_all, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")));
		make_def_add_act(&make, def, def_all);

		make_act_t def_phony;
		make_phony(&make, &def_phony);
		make_def_add_act(&make, def, def_phony);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")));
		make_act_t def_compile;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")), 1, &def_compile);
		make_rule_add_depend(&make, def_compile, MRULE(MVAR(mvars[DIR_OUT_BIN_FILE])));
		make_def_add_act(&make, def, def_compile);

		make_act_t def_rule_target;
		make_rule(&make, MRULE(MVAR(mvars[DIR_OUT_BIN_FILE])), 1, &def_rule_target);
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PN)_$(TN)_DRIVERS)"))));
		make_rule_add_depend(&make, def_rule_target, MRULE(MVAR(objs[PKGSRC_OBJ].var)));
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PN)_$(TN)_LIBS)"))));
		make_def_add_act(&make, def, def_rule_target);

		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_target, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGSRC_OBJ) $($(PN)_$(TN)_DRIVERS) $($(PN)_$(TN)_LIBS)")),
			 &act);
		make_rule_add_act(&make, def_rule_target, act);

		make_act_t def_rule_obj;
		make_rule(&make, MRULE(MSTR(STRV("$(DIR_OUT_INT_SRC)%.o"))), 1, &def_rule_obj);
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$(DIR_PKG_SRC)%.c"))));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MVAR(pkgfiles[PKGSRC_H].var)));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$($(PN)_$(TN)_HEADERS)"))));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) -c $(DIR_PKG_SRC:%=-I%) $($(PN)_$(TN)_INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<")),
			 &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_def_add_act(&make, def, def_rule_obj);

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	if (types[TARGET_TYPE_LIB]) {
		make_act_t def;
		make_def(&make, defines[TARGET_TYPE_LIB].name, &def);
		make_add_act(&make, root, def);
		defines[TARGET_TYPE_LIB].def = def;
		make_var(&make, STRV("$(PN)_$(TN)"), MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MVAR(mvars[DIR_OUT_LIB_FILE]));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_var(&make, STRV("GCDA"), MAKE_VAR_APP, &act);
		make_var_add_val(&make, act, MVAR(objs[PKGSRC_GCDA].var));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_act_t def_all;
		make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &def_all);
		make_rule_add_depend(&make, def_all, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")));
		make_def_add_act(&make, def, def_all);

		make_act_t def_phony;
		make_phony(&make, &def_phony);
		make_def_add_act(&make, def, def_phony);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")));
		make_act_t def_compile;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")), 1, &def_compile);
		make_rule_add_depend(&make, def_compile, MRULE(MVAR(mvars[DIR_OUT_LIB_FILE])));
		make_def_add_act(&make, def, def_compile);

		make_act_t def_rule_target;
		make_rule(&make, MRULE(MVAR(mvars[DIR_OUT_LIB_FILE])), 1, &def_rule_target);
		make_rule_add_depend(&make, def_rule_target, MRULE(MVAR(objs[PKGSRC_OBJ].var)));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_target, act);
		make_cmd(&make, MCMD(STRV("ar rcs $$@ $(PKGSRC_OBJ)")), &act);
		make_rule_add_act(&make, def_rule_target, act);
		make_def_add_act(&make, def, def_rule_target);

		make_act_t def_rule_obj;
		make_rule(&make, MRULE(MSTR(STRV("$(DIR_OUT_INT_SRC)%.o"))), 1, &def_rule_obj);
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$(DIR_PKG_SRC)%.c"))));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MVAR(pkgfiles[PKGSRC_H].var)));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$($(PN)_$(TN)_HEADERS)"))));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) -c $(DIR_PKG_SRC:%=-I%) $($(PN)_$(TN)_INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<")),
			 &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_def_add_act(&make, def, def_rule_obj);

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	if (types[TARGET_TYPE_EXT]) {
		make_act_t def;
		make_def(&make, defines[TARGET_TYPE_EXT].name, &def);
		make_add_act(&make, root, def);
		defines[TARGET_TYPE_EXT].def = def;
		make_var(&make, STRV("$(PN)_$(TN)"), MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MVAR(mvars[DIR_OUT_EXT_FILE]));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_act_t def_all;
		make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &def_all);
		make_rule_add_depend(&make, def_all, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")));
		make_def_add_act(&make, def, def_all);

		make_act_t def_phony;
		make_phony(&make, &def_phony);
		make_def_add_act(&make, def, def_phony);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")));
		make_act_t def_compile;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")), 1, &def_compile);
		make_rule_add_depend(&make, def_compile, MRULE(MVAR(mvars[DIR_OUT_EXT_FILE])));
		make_def_add_act(&make, def, def_compile);

		make_act_t def_rule_target;
		make_rule(&make, MRULE(MVAR(mvars[DIR_OUT_EXT_FILE])), 1, &def_rule_target);
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$(DIR_TMP_EXT_PKG)"))));
		make_def_add_act(&make, def, def_rule_target);

		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_target, act);

		make_cmd(&make, MCMD(STRV("cd $(DIR_TMP_EXT_PKG_ROOT) && $(TGT_CMD)")), &act);
		make_rule_add_act(&make, def_rule_target, act);

		make_cmd(&make, MCMD(STRV("cp $(DIR_TMP_EXT_PKG_ROOT_OUT) $(DIR_OUT_EXT_PKG)")), &act);
		make_rule_add_act(&make, def_rule_target, act);

		make_cmd(&make, MCMD(STRV("touch $$@")), &act);
		make_rule_add_act(&make, def_rule_target, act);

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	if (types[TARGET_TYPE_TST]) {
		make_act_t def;
		make_def(&make, defines[TARGET_TYPE_TST].name, &def);
		make_add_act(&make, root, def);
		defines[TARGET_TYPE_TST].def = def;

		make_var(&make, STRV("$(PN)_$(TN)"), MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MVAR(mvars[DIR_OUT_TST_FILE]));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_var(&make, STRV("GCDA"), MAKE_VAR_APP, &act);
		make_var_add_val(&make, act, MVAR(objs[PKGTST_GCDA].var));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_act_t def_all;
		make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &def_all);
		make_rule_add_depend(&make, def_all, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")));
		make_def_add_act(&make, def, def_all);

		make_act_t def_test;
		make_rule(&make, MRULE(MSTR(STRV("test"))), 1, &def_test);
		make_rule_add_depend(&make, def_test, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/test")));
		make_def_add_act(&make, def, def_test);

		make_act_t def_cov;
		make_rule(&make, MRULE(MSTR(STRV("cov"))), 1, &def_cov);
		make_rule_add_depend(&make, def_cov, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/cov")));
		make_def_add_act(&make, def, def_cov);

		make_act_t def_phony;
		make_phony(&make, &def_phony);
		make_def_add_act(&make, def, def_phony);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")));
		make_act_t def_compile;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/compile")), 1, &def_compile);
		make_rule_add_depend(&make, def_compile, MRULE(MVAR(mvars[DIR_OUT_TST_FILE])));
		make_def_add_act(&make, def, def_compile);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/test")));
		make_act_t def_run_test;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/test")), 1, &def_run_test);
		make_rule_add_depend(&make, def_run_test, MRULE(MVAR(mvars[DIR_OUT_TST_FILE])));
		make_cmd(&make, MCMD(STRV("$(DIR_OUT_TST_FILE)")), &act);
		make_rule_add_act(&make, def_run_test, act);
		make_def_add_act(&make, def, def_run_test);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/cov")));
		make_act_t def_run_cov;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PN)_$(TN)")), STRV("/cov")), 1, &def_run_cov);
		make_rule_add_depend(&make, def_run_cov, MRULE(MSTR(STRV("precov"))));
		make_rule_add_depend(&make, def_run_cov, MRULE(MVAR(mvars[DIR_OUT_TST_FILE])));
		make_cmd(&make, MCMD(STRV("$(DIR_OUT_TST_FILE)")), &act);
		make_rule_add_act(&make, def_run_cov, act);
		make_def_add_act(&make, def, def_run_cov);

		make_act_t def_rule_target;
		make_rule(&make, MRULE(MVAR(mvars[DIR_OUT_TST_FILE])), 1, &def_rule_target);
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PN)_$(TN)_DRIVERS)"))));
		make_rule_add_depend(&make, def_rule_target, MRULE(MVAR(objs[PKGTST_OBJ].var)));
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PN)_$(TN)_LIBS)"))));
		make_def_add_act(&make, def, def_rule_target);

		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_target, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGTST_OBJ) $($(PN)_$(TN)_DRIVERS) $($(PN)_$(TN)_LIBS)")),
			 &act);
		make_rule_add_act(&make, def_rule_target, act);

		make_act_t def_rule_obj;
		make_rule(&make, MRULE(MSTR(STRV("$(DIR_OUT_INT_TST)%.o"))), 1, &def_rule_obj);
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$(DIR_PKG_TST)%.c"))));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MVAR(pkgfiles[PKGSRC_H].var)));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$($(PN)_$(TN)_HEADERS)"))));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) -c $(DIR_PKG_TST:%=-I%) $($(PN)_$(TN)_INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<")),
			 &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_def_add_act(&make, def, def_rule_obj);

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	make_act_t precov;
	make_rule(&make, MRULE(MSTR(STRV("precov"))), 1, &precov);
	make_cmd(&make, MCMD(STRV("@rm -fv $(GCDA)")), &act);
	make_rule_add_act(&make, precov, act);
	make_add_act(&make, root, precov);

	make_act_t cov;
	make_rule(&make, MRULE(MSTR(STRV("cov"))), 1, &cov);

	make_cmd(&make, MCMD(STRV("@if [ -n \"$(GCDA)\" ]; then \\")), &act);
	make_rule_add_act(&make, cov, act);
	make_cmd(&make, MCMD(STRV("\tmkdir -pv $(DIR_TMP_COV); \\")), &act);
	make_rule_add_act(&make, cov, act);
	make_cmd(&make, MCMD(STRV("\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT_INT); \\")), &act);
	make_rule_add_act(&make, cov, act);
	make_cmd(&make, MCMD(STRV("\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\")), &act);
	make_rule_add_act(&make, cov, act);
	make_cmd(&make, MCMD(STRV("\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\")), &act);
	make_rule_add_act(&make, cov, act);
	make_cmd(&make, MCMD(STRV("fi")), &act);
	make_rule_add_act(&make, cov, act);

	make_add_act(&make, root, cov);

	if (proj->pkgs.cnt > 0) {
		str_t buf2 = strz(16);

		arr_t order = {0};
		arr_init(&order, proj->targets.cnt, sizeof(uint), ALLOC_STD);
		proj_get_pkg_build_order(proj, &order, ALLOC_STD);

		arr_t deps = {0};
		arr_init(&deps, 1, sizeof(uint), ALLOC_STD);

		i = 0;
		const uint *id;
		arr_foreach(&order, i, id)
		{
			const pkg_t *pkg = proj_get_pkg(proj, *id);

			path_init_s(&tmp, proj_get_str(proj, pkg->strs + PKG_PATH), '/');
			path_push_s(&tmp, STRV("pkg.mk"), '/');

			buf.len = 0;
			str_cat(&buf, STRV("$(DIR_BUILD)"));
			str_cat(&buf, STRVS(tmp));

			make_act_t inc;
			make_inc(&make, STRVS(buf), &inc);
			make_add_act(&make, root, inc);

			gen_pkg(proj, &vars, &make, drv->fs, *id, inc, protos_defs, exts_defs, defines, &deps, &buf2, build_dir);
		}

		arr_free(&deps);
		arr_free(&order);

		make_empty(&make, &act);
		make_add_act(&make, root, act);
		str_free(&buf2);
	}

	void *file;
	fs_open(drv->fs, STRVS(path), "w", &file);
	make_print(&make, root, DST_FS(drv->fs, file));
	fs_close(drv->fs, file);

	make_free(&make);

	str_free(&buf);

	return 0;
}

static gen_driver_t make = {
	.param = STRVT("M"),
	.desc  = "Make",
	.gen   = gen_make,
};

GEN_DRIVER(make, &make);
