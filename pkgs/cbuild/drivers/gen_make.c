#include "gen.h"

#include "file/make.h"
#include "log.h"
#include "mem.h"
#include "var.h"

typedef struct defines_s {
	strv_t name;
	make_act_t def;
} defines_t;

static int gen_pkg(const proj_t *proj, make_t *make, fs_t *fs, uint id, make_act_t inc, const defines_t *defines, arr_t *deps, str_t *buf,
		   strv_t build_dir)

{
	const pkg_t *pkg = proj_get_pkg(proj, id);

	path_t path = {0};
	path_init(&path, build_dir);
	strv_t pkg_dir = proj_get_str(proj, pkg->strs + PKG_DIR);
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

	make_var(make, STRV("$(PN).DIR"), MAKE_VAR_INST, &act);
	strv_t dir = proj_get_str(proj, pkg->strs + PKG_DIR);
	if (dir.len > 0) {
		make_var_add_val(make, act, MSTR(dir));
	}
	make_inc_add_act(make, inc, act);

	uint i = 0;
	const target_t *target;
	arr_foreach(&proj->targets, i, target)
	{
		if (target->pkg != id) {
			continue;
		}

		make_var(make, STRV("TN"), MAKE_VAR_INST, &act);
		make_var_add_val(make, act, MSTR(proj_get_str(proj, target->strs + TARGET_NAME)));
		make_inc_add_act(make, inc, act);

		make_var(make, STRV("$(PN).$(TN).HEADERS"), MAKE_VAR_INST, &act);
		strv_t include = proj_get_str(proj, pkg->strs + PKG_INC);
		if (include.len > 0) {
			make_var_add_val(make, act, MSTR(STRV("$(PKGINC_H)")));
		}
		make_inc_add_act(make, inc, act);
		make_var(make, STRV("$(PN).$(TN).INCLUDES"), MAKE_VAR_INST, &act);
		if (include.len > 0) {
			buf->len = 0;
			str_cat(buf, STRV("$(PKGDIR)"));
			str_cat(buf, include);
			make_var_add_val(make, act, MSTR(STRVS(*buf)));
		}
		make_inc_add_act(make, inc, act);

		switch (target->type) {
		case TARGET_TYPE_EXE:
		case TARGET_TYPE_TST: {
			make_act_t libs;
			make_var(make, STRV("$(PN).$(TN).LIBS"), MAKE_VAR_INST, &libs);
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
					str_cat(buf, STRV("."));
					str_cat(buf, proj_get_str(proj, dtarget->strs + PKG_NAME));
					str_cat(buf, STRV(")"));
					make_var_add_val(make, libs, MSTR(STRVS(*buf)));
					buf->len = buf_len;
				}
			}

			make_var(make, STRV("$(PN).$(TN).DRIVERS"), MAKE_VAR_INST, &act);
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

	strv_t values[__VAR_CNT] = {
		[VAR_ARCH]   = STRVT("$(ARCH)"),
		[VAR_CONFIG] = STRVT("$(CONFIG)"),
	};

	str_t outdir = strn(proj->outdir.data, proj->outdir.len, sizeof(proj->outdir.data));
	if (var_replace(&outdir, values)) {
		// return 1;
	}

	make_t make = {0};
	make_init(&make, 32, 32, 2, 32, ALLOC_STD);

	make_act_t root, mcurdir, march, mconfig, act;
	make_var_ext(&make, STRV("CURDIR"), &mcurdir);
	make_ext_set_val(&make, mcurdir, MSTR(STRV(".")));
	root = mcurdir;
	make_var_ext(&make, STRV("ARCH"), &march);
	make_add_act(&make, root, march);
	make_var_ext(&make, STRV("CONFIG"), &mconfig);
	make_add_act(&make, root, mconfig);

	make_var(&make, STRV("PROJDIR"), MAKE_VAR_INST, &act);

	path_t rel = {0};
	path_calc_rel(build_dir, proj_dir, &rel);
	if (rel.len > 0) {
		make_var_add_val(&make, act, MSTR(STRVS(rel)));
	}
	make_add_act(&make, root, act);
	make_var(&make, STRV("BUILDDIR"), MAKE_VAR_INST, &act);
	make_add_act(&make, root, act);
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	make_var(&make, STRV("TCC"), MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("$(CC)")));
	make_add_act(&make, root, act);
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	make_act_t mbits, if_x64, if_x86;
	make_var_var(&make, march, MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("x64")));
	make_add_act(&make, root, act);
	make_if(&make, MVAR(march), MSTR(STRV("x64")), &if_x64);
	make_add_act(&make, root, if_x64);
	make_var(&make, STRV("BITS"), MAKE_VAR_INST, &mbits);
	make_var_add_val(&make, mbits, MSTR(STRV("64")));
	make_if_add_true_act(&make, if_x64, mbits);
	make_if(&make, MVAR(march), MSTR(STRV("x86")), &if_x86);
	make_add_act(&make, root, if_x86);
	make_var_var(&make, mbits, MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("32")));
	make_if_add_true_act(&make, if_x86, act);
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	make_var_var(&make, mconfig, MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("Debug")));
	make_add_act(&make, root, act);

	make_act_t mcflags, mldflags, if_debug, if_release;
	make_if(&make, MVAR(mconfig), MSTR(STRV("Debug")), &if_debug);
	make_add_act(&make, root, if_debug);
	make_var(&make, STRV("CFLAGS"), MAKE_VAR_INST, &mcflags);
	make_var_add_val(&make, mcflags, MSTR(STRV("-Wall -Wextra -Werror -pedantic -O0 -ggdb -coverage")));
	make_if_add_true_act(&make, if_debug, mcflags);
	make_var(&make, STRV("LDFLAGS"), MAKE_VAR_INST, &mldflags);
	make_var_add_val(&make, mldflags, MSTR(STRV("-coverage")));
	make_if_add_true_act(&make, if_debug, mldflags);
	make_if(&make, MVAR(mconfig), MSTR(STRV("Release")), &if_release);
	make_add_act(&make, root, if_release);
	make_var_var(&make, mcflags, MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRV("-Wall -Wextra -Werror -pedantic")));
	make_if_add_true_act(&make, if_release, act);
	make_var_var(&make, mldflags, MAKE_VAR_INST, &act);
	make_if_add_true_act(&make, if_release, act);
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	str_t buf = strz(16);
	if (pathv_is_rel(STRVS(outdir))) {
		str_cat(&buf, STRV("$(PROJDIR)"));
	}
	str_cat(&buf, STRVN(outdir.data, outdir.len));

	make_var(&make, STRV("OUTDIR"), MAKE_VAR_INST, &act);
	make_var_add_val(&make, act, MSTR(STRVS(buf)));
	make_add_act(&make, root, act);

	static const struct {
		strv_t name;
		strv_t val;
	} outdirs[] = {
		{STRVT("INTDIR"), STRVT("$(OUTDIR)int")},
		{STRVT("LIBDIR"), STRVT("$(OUTDIR)lib")},
		{STRVT("BINDIR"), STRVT("$(OUTDIR)bin")},
		{STRVT("TSTDIR"), STRVT("$(OUTDIR)test")},
	};

	for (size_t i = 0; i < sizeof(outdirs) / sizeof(outdirs[0]); i++) {
		make_var(&make, outdirs[i].name, MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MSTR(outdirs[i].val));
		make_add_act(&make, root, act);
	}
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	make_var(&make, STRV("PKGDIR"), MAKE_VAR_REF, &act);
	make_var_add_val(&make, act, MSTR(STRV("$(PROJDIR)$($(PN).DIR)")));
	make_add_act(&make, root, act);

	enum {
		PKGDIR_SRC,
		PKGDIR_INC,
		PKGDIR_DRV,
		PKGDIR_TST,
	};

	struct {
		strv_t name;
		strv_t val;
		make_act_t var;
	} pkgdirs[] = {
		[PKGDIR_SRC] = {STRVT("PKGDIR_SRC"), STRVT("$(PKGDIR)src/")},
		[PKGDIR_INC] = {STRVT("PKGDIR_INC"), STRVT("$(PKGDIR)include/")},
		[PKGDIR_DRV] = {STRVT("PKGDIR_DRV"), STRVT("$(PKGDIR)drivers/")},
		[PKGDIR_TST] = {STRVT("PKGDIR_TST"), STRVT("$(PKGDIR)test/")},
	};

	for (size_t i = 0; i < sizeof(pkgdirs) / sizeof(pkgdirs[0]); i++) {
		make_var(&make, pkgdirs[i].name, MAKE_VAR_REF, &act);
		make_var_add_val(&make, act, MSTR(pkgdirs[i].val));
		make_add_act(&make, root, act);
		pkgdirs[i].var = act;
	}
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
		[PKGSRC_C] = {STRVT("PKGSRC_C"), STRVT("$(shell find $(PKGDIR_SRC) -type f -name '*.c')")},
		[PKGSRC_H] = {STRVT("PKGSRC_H"), STRVT("$(shell find $(PKGDIR_SRC) -type f -name '*.h')")},
		[PKGDRV_C] = {STRVT("PKGDRV_C"), STRVT("$(shell find $(PKGDIR_DRV) -type f -name '*.c')")},
		[PKGTST_C] = {STRVT("PKGTST_C"), STRVT("$(shell find $(PKGDIR_TST) -type f -name '*.c')")},
		[PKGTST_H] = {STRVT("PKGTST_H"), STRVT("$(shell find $(PKGDIR_TST) -type f -name '*.h')")},
		[PKGINC_H] = {STRVT("PKGINC_H"), STRVT("$(shell find $(PKGDIR_INC) -type f -name '*.h')")},
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
		INTDIR_SRC,
		INTDIR_DRV,
		INTDIR_TST,
	};

	struct {
		strv_t name;
		strv_t val;
		make_act_t var;
	} intdirs[] = {
		[INTDIR_SRC] = {STRVT("INTDIR_SRC"), STRVT("$(INTDIR)/$(PN)/src/")},
		[INTDIR_DRV] = {STRVT("INTDIR_DRV"), STRVT("$(INTDIR)/$(PN)/drivers/")},
		[INTDIR_TST] = {STRVT("INTDIR_TST"), STRVT("$(INTDIR)/$(PN)/test/")},
	};

	for (size_t i = 0; i < sizeof(intdirs) / sizeof(intdirs[0]); i++) {
		make_var(&make, intdirs[i].name, MAKE_VAR_REF, &act);
		make_var_add_val(&make, act, MSTR(intdirs[i].val));
		make_add_act(&make, root, act);
		intdirs[i].var = act;
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
		[PKGSRC_OBJ]  = {STRVT("PKGSRC_OBJ"), STRVT("$(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))")},
		[PKGSRC_GCDA] = {STRVT("PKGSRC_GCDA"), STRVT("$(patsubst %.o,%.gcda,$(PKGSRC_OBJ))")},
		[PKGDRV_OBJ]  = {STRVT("PKGDRV_OBJ"), STRVT("$(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))")},
		[PKGDRV_GCDA] = {STRVT("PKGDRV_GCDA"), STRVT("$(patsubst %.o,%.gcda,$(PKGDRV_OBJ))")},
		[PKGTST_OBJ]  = {STRVT("PKGTST_OBJ"), STRVT("$(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))")},
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

	enum {
		PKGEXE,
		PKGLIB,
		PKGTST,
	};

	struct {
		strv_t name;
		strv_t val;
		make_act_t var;
	} targets[] = {
		[PKGEXE] = {STRVT("PKGEXE"), STRVT("$(BINDIR)/$(PN)")},
		[PKGLIB] = {STRVT("PKGLIB"), STRVT("$(LIBDIR)/$(PN).a")},
		[PKGTST] = {STRVT("PKGTST"), STRVT("$(TSTDIR)/$(PN)")},
	};

	for (size_t i = 0; i < sizeof(targets) / sizeof(targets[0]); i++) {
		make_var(&make, targets[i].name, MAKE_VAR_REF, &act);
		make_var_add_val(&make, act, MSTR(targets[i].val));
		make_add_act(&make, root, act);
		targets[i].var = act;
	}
	make_empty(&make, &act);
	make_add_act(&make, root, act);

	make_act_t phony_all;
	make_phony(&make, &phony_all);
	make_add_act(&make, root, phony_all);
	make_rule_add_depend(&make, phony_all, MRULE(MSTR(STRV("all"))));
	make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &act);
	make_add_act(&make, root, act);

	defines_t defines[] = {
		[TARGET_TYPE_UNKNOWN] = {STRVT("unknown")},
		[TARGET_TYPE_EXE]     = {STRVT("exe")},
		[TARGET_TYPE_LIB]     = {STRVT("lib")},
		[TARGET_TYPE_TST]     = {STRVT("test")},
	};

	int types[__TARGET_TYPE_MAX] = {0};

	uint i = 0;
	target_t *target;
	arr_foreach(&proj->targets, i, target)
	{
		types[target->type] = 1;
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

		make_var(&make, STRV("$(PN).$(TN)"), MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MVAR(targets[PKGEXE].var));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_act_t def_all;
		make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &def_all);
		make_rule_add_depend(&make, def_all, MRULEACT(MSTR(STRV("$(PN).$(TN)")), STRV("/compile")));
		make_def_add_act(&make, def, def_all);

		make_act_t def_phony;
		make_phony(&make, &def_phony);
		make_def_add_act(&make, def, def_phony);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PN).$(TN)")), STRV("/compile")));
		make_act_t def_compile;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PN).$(TN)")), STRV("/compile")), 1, &def_compile);
		make_rule_add_depend(&make, def_compile, MRULE(MVAR(targets[PKGEXE].var)));
		make_def_add_act(&make, def, def_compile);

		make_act_t def_rule_target;
		make_rule(&make, MRULE(MVAR(targets[PKGEXE].var)), 1, &def_rule_target);
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PN).$(TN).DRIVERS)"))));
		make_rule_add_depend(&make, def_rule_target, MRULE(MVAR(objs[PKGSRC_OBJ].var)));
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PN).$(TN).LIBS)"))));
		make_def_add_act(&make, def, def_rule_target);

		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_target, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGSRC_OBJ) $($(PN).$(TN).DRIVERS) $($(PN).$(TN).LIBS)")),
			 &act);
		make_rule_add_act(&make, def_rule_target, act);

		make_act_t def_rule_obj;
		make_rule(&make, MRULE(MSTR(STRV("$(INTDIR_SRC)%.o"))), 1, &def_rule_obj);
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$(PKGDIR_SRC)%.c"))));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MVAR(pkgfiles[PKGSRC_H].var)));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$($(PN).$(TN).HEADERS)"))));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) -c $(PKGDIR_SRC:%=-I%) $($(PN).$(TN).INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<")),
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
		make_var(&make, STRV("$(PN).$(TN)"), MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MVAR(targets[PKGLIB].var));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_act_t def_all;
		make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &def_all);
		make_rule_add_depend(&make, def_all, MRULEACT(MSTR(STRV("$(PN).$(TN)")), STRV("/compile")));
		make_def_add_act(&make, def, def_all);

		make_act_t def_phony;
		make_phony(&make, &def_phony);
		make_def_add_act(&make, def, def_phony);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PN).$(TN)")), STRV("/compile")));
		make_act_t def_compile;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PN).$(TN)")), STRV("/compile")), 1, &def_compile);
		make_rule_add_depend(&make, def_compile, MRULE(MVAR(targets[PKGLIB].var)));
		make_def_add_act(&make, def, def_compile);

		make_act_t def_rule_target;
		make_rule(&make, MRULE(MVAR(targets[PKGLIB].var)), 1, &def_rule_target);
		make_rule_add_depend(&make, def_rule_target, MRULE(MVAR(objs[PKGSRC_OBJ].var)));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_target, act);
		make_cmd(&make, MCMD(STRV("ar rcs $$@ $(PKGSRC_OBJ)")), &act);
		make_rule_add_act(&make, def_rule_target, act);
		make_def_add_act(&make, def, def_rule_target);

		make_act_t def_rule_obj;
		make_rule(&make, MRULE(MSTR(STRV("$(INTDIR_SRC)%.o"))), 1, &def_rule_obj);
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$(PKGDIR_SRC)%.c"))));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MVAR(pkgfiles[PKGSRC_H].var)));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$($(PN).$(TN).HEADERS)"))));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) -c $(PKGDIR_SRC:%=-I%) $($(PN).$(TN).INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<")),
			 &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_def_add_act(&make, def, def_rule_obj);

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	if (types[TARGET_TYPE_TST]) {
		make_act_t def;
		make_def(&make, defines[TARGET_TYPE_TST].name, &def);
		make_add_act(&make, root, def);
		defines[TARGET_TYPE_TST].def = def;

		make_var(&make, STRV("$(PN).$(TN)"), MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MVAR(targets[PKGTST].var));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_act_t def_all;
		make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &def_all);
		make_rule_add_depend(&make, def_all, MRULEACT(MSTR(STRV("$(PN).$(TN)")), STRV("/compile")));
		make_def_add_act(&make, def, def_all);

		make_act_t def_phony;
		make_phony(&make, &def_phony);
		make_def_add_act(&make, def, def_phony);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PN).$(TN)")), STRV("/compile")));
		make_act_t def_compile;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PN).$(TN)")), STRV("/compile")), 1, &def_compile);
		make_rule_add_depend(&make, def_compile, MRULE(MVAR(targets[PKGTST].var)));
		make_def_add_act(&make, def, def_compile);

		make_act_t def_rule_target;
		make_rule(&make, MRULE(MVAR(targets[PKGTST].var)), 1, &def_rule_target);
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PN).$(TN).DRIVERS)"))));
		make_rule_add_depend(&make, def_rule_target, MRULE(MVAR(objs[PKGTST_OBJ].var)));
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PN).$(TN).LIBS)"))));
		make_def_add_act(&make, def, def_rule_target);

		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_target, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGTST_OBJ) $($(PN).$(TN).DRIVERS) $($(PN).$(TN).LIBS)")),
			 &act);
		make_rule_add_act(&make, def_rule_target, act);

		make_act_t def_rule_obj;
		make_rule(&make, MRULE(MSTR(STRV("$(INTDIR_TST)%.o"))), 1, &def_rule_obj);
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$(PKGDIR_TST)%.c"))));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MVAR(pkgfiles[PKGSRC_H].var)));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$($(PN).$(TN).HEADERS)"))));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_cmd(&make,
			 MCMD(STRV("$(TCC) -m$(BITS) -c $(PKGDIR_TST:%=-I%) $($(PN).$(TN).INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<")),
			 &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_def_add_act(&make, def, def_rule_obj);

		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	make_act_t phony;
	make_phony(&make, &phony);
	make_add_act(&make, root, phony);

	make_rule_add_depend(&make, phony, MRULE(MSTR(STRV("test"))));
	make_act_t test;
	make_rule(&make, MRULE(MSTR(STRV("test"))), 1, &test);
	make_add_act(&make, root, test);

	make_rule_add_depend(&make, phony, MRULE(MSTR(STRV("coverage"))));
	make_act_t cov;
	make_rule(&make, MRULE(MSTR(STRV("coverage"))), 1, &cov);
	make_rule_add_depend(&make, cov, MRULE(MSTR(STRV("test"))));
	make_cmd(&make, MCMD(STRV("lcov -q -c -o $(PROJDIR)bin/lcov.info -d $(INTDIR)")), &act);
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

			strv_t name = proj_get_str(proj, pkg->strs + PKG_NAME);

			buf.len = 0;
			str_cat(&buf, STRV("$(BUILDDIR)"));
			str_cat(&buf, proj_get_str(proj, pkg->strs + PKG_DIR));
			str_cat(&buf, STRV("pkg.mk"));

			make_act_t inc;
			make_inc(&make, STRVS(buf), &inc);
			make_add_act(&make, root, inc);

			gen_pkg(proj, &make, drv->fs, *id, inc, defines, &deps, &buf2, build_dir);

			make_rule_add_depend(&make, test, MRULEACT(MSTR(name), STRV("/test")));
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

	str_free(&outdir);
	str_free(&buf);

	return 0;
}

static gen_driver_t make = {
	.param = STRVT("M"),
	.desc  = "Make",
	.gen   = gen_make,
};

GEN_DRIVER(make, &make);
