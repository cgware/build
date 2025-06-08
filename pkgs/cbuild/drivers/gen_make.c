#include "gen.h"

#include "file/make.h"
#include "log.h"
#include "mem.h"
#include "var.h"

typedef struct defines_s {
	strv_t name;
	make_act_t def;
} defines_t;

static int gen_pkg(uint id, strv_t name, make_t *make, make_act_t inc, const defines_t *defines, const pkgs_t *pkgs, arr_t *deps,
		   str_t *buf, fs_t *fs, strv_t build_dir)

{
	const pkg_t *pkg = pkgs_get(pkgs, id);

	make_act_t act;
	make_var(make, STRV("PKG"), MAKE_VAR_INST, &act);
	make_var_add_val(make, act, MSTR(name));
	make_inc_add_act(make, inc, act);

	make_var(make, STRV("$(PKG)_DIR"), MAKE_VAR_INST, &act);
	strv_t dir = strvbuf_get(&pkgs->strs, pkg->strs[PKG_DIR]);
	if (dir.len > 0) {
		make_var_add_val(make, act, MSTR(dir));
	}
	make_inc_add_act(make, inc, act);
	make_var(make, STRV("$(PKG)_HEADERS"), MAKE_VAR_INST, &act);
	strv_t include = strvbuf_get(&pkgs->strs, pkg->strs[PKG_INC]);
	if (include.len > 0) {
		make_var_add_val(make, act, MSTR(STRV("$(PKGINC_H)")));
	}
	make_inc_add_act(make, inc, act);
	make_var(make, STRV("$(PKG)_INCLUDES"), MAKE_VAR_INST, &act);
	if (include.len > 0) {
		buf->len = 0;
		str_cat(buf, STRV("$(PKGDIR)"));
		str_cat(buf, include);
		make_var_add_val(make, act, MSTR(STRVS(*buf)));
	}
	make_inc_add_act(make, inc, act);
	make_act_t libs;
	make_var(make, STRV("$(PKG)_LIBS"), MAKE_VAR_INST, &libs);
	make_inc_add_act(make, inc, libs);
	if (pkg->has_targets) {
		targets_get_deps(&pkgs->targets, pkg->targets, deps);

		buf->len = 0;

		str_cat(buf, STRV("$("));
		size_t buf_len = buf->len;

		uint i = 0;
		uint *dep;
		arr_foreach(deps, i, dep)
		{
			str_cat(buf, pkgs_get_name(pkgs, *dep));
			str_cat(buf, STRV(")"));
			make_var_add_val(make, libs, MSTR(STRVS(*buf)));
			buf->len = buf_len;
		}
	}

	make_var(make, STRV("$(PKG)_DRIVERS"), MAKE_VAR_INST, &act);
	make_inc_add_act(make, inc, act);

	if (pkg->has_targets) {
		const target_t *target = targets_get(&pkgs->targets, pkg->targets);
		if (target != NULL) {
			make_eval_def(make, defines[target->type].def, &act);
			make_inc_add_act(make, inc, act);
		}
	}

	path_t make_path = {0};
	path_init(&make_path, build_dir);
	fs_mkpath(fs, STRVS(make_path), strvbuf_get(&pkgs->strs, pkg->strs[PKG_DIR]));
	path_push(&make_path, strvbuf_get(&pkgs->strs, pkg->strs[PKG_DIR]));
	path_push(&make_path, STRV("pkg.mk"));

	void *file;
	fs_open(fs, STRVS(make_path), "w", &file);
	make_inc_print(make, inc, DST_FS(fs, file));
	fs_close(fs, file);

	return 0;
}

static int gen_make(const gen_driver_t *drv, const proj_t *proj, strv_t proj_dir, strv_t build_dir)
{
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
	make_var_add_val(&make, act, MSTR(STRV("$(PROJDIR)$($(PKG)_DIR)")));
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
		[INTDIR_SRC] = {STRVT("INTDIR_SRC"), STRVT("$(INTDIR)/$(PKG)/src/")},
		[INTDIR_DRV] = {STRVT("INTDIR_DRV"), STRVT("$(INTDIR)/$(PKG)/drivers/")},
		[INTDIR_TST] = {STRVT("INTDIR_TST"), STRVT("$(INTDIR)/$(PKG)/test/")},
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
		[PKGEXE] = {STRVT("PKGEXE"), STRVT("$(BINDIR)/$(PKG)")},
		[PKGLIB] = {STRVT("PKGLIB"), STRVT("$(LIBDIR)/$(PKG).a")},
		[PKGTST] = {STRVT("PKGTST"), STRVT("$(TSTDIR)/$(PKG)")},
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
		[TARGET_TYPE_EXE] = {STRVT("exe")},
		[TARGET_TYPE_LIB] = {STRVT("lib")},
	};

	{
		make_act_t def;
		make_def(&make, defines[TARGET_TYPE_EXE].name, &def);
		make_add_act(&make, root, def);
		defines[TARGET_TYPE_EXE].def = def;

		make_var(&make, STRV("$(PKG)"), MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MVAR(targets[PKGEXE].var));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_act_t def_all;
		make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &def_all);
		make_rule_add_depend(&make, def_all, MRULEACT(MSTR(STRV("$(PKG)")), STRV("/compile")));
		make_def_add_act(&make, def, def_all);

		make_act_t def_phony;
		make_phony(&make, &def_phony);
		make_def_add_act(&make, def, def_phony);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PKG)")), STRV("/compile")));
		make_act_t def_compile;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PKG)")), STRV("/compile")), 1, &def_compile);
		make_rule_add_depend(&make, def_compile, MRULE(MVAR(targets[PKGEXE].var)));
		make_def_add_act(&make, def, def_compile);

		make_act_t def_rule_target;
		make_rule(&make, MRULE(MVAR(targets[PKGEXE].var)), 1, &def_rule_target);
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PKG)_DRIVERS)"))));
		make_rule_add_depend(&make, def_rule_target, MRULE(MVAR(objs[PKGSRC_OBJ].var)));
		make_rule_add_depend(&make, def_rule_target, MRULE(MSTR(STRV("$($(PKG)_LIBS)"))));
		make_def_add_act(&make, def, def_rule_target);

		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_target, act);
		make_cmd(&make, MCMD(STRV("$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGSRC_OBJ) $($(PKG)_DRIVERS) $($(PKG)_LIBS)")), &act);
		make_rule_add_act(&make, def_rule_target, act);

		make_act_t def_rule_obj;
		make_rule(&make, MRULE(MSTR(STRV("$(INTDIR_SRC)%.o"))), 1, &def_rule_obj);
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$(PKGDIR_SRC)%.c"))));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MVAR(pkgfiles[PKGSRC_H].var)));
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$($(PKG)_HEADERS)"))));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_cmd(&make, MCMD(STRV("$(TCC) -m$(BITS) -c $(PKGDIR_SRC:%=-I%) $($(PKG)_INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_def_add_act(&make, def, def_rule_obj);
	}

	make_empty(&make, &act);
	make_add_act(&make, root, act);

	{
		make_act_t def;
		make_def(&make, defines[TARGET_TYPE_LIB].name, &def);
		make_add_act(&make, root, def);
		defines[TARGET_TYPE_LIB].def = def;
		make_var(&make, STRV("$(PKG)"), MAKE_VAR_INST, &act);
		make_var_add_val(&make, act, MVAR(targets[PKGLIB].var));
		make_def_add_act(&make, def, act);
		make_empty(&make, &act);
		make_def_add_act(&make, def, act);

		make_act_t def_all;
		make_rule(&make, MRULE(MSTR(STRV("all"))), 1, &def_all);
		make_rule_add_depend(&make, def_all, MRULEACT(MSTR(STRV("$(PKG)")), STRV("/compile")));
		make_def_add_act(&make, def, def_all);

		make_act_t def_phony;
		make_phony(&make, &def_phony);
		make_def_add_act(&make, def, def_phony);

		make_rule_add_depend(&make, def_phony, MRULEACT(MSTR(STRV("$(PKG)")), STRV("/compile")));
		make_act_t def_compile;
		make_rule(&make, MRULEACT(MSTR(STRV("$(PKG)")), STRV("/compile")), 1, &def_compile);
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
		make_rule_add_depend(&make, def_rule_obj, MRULE(MSTR(STRV("$($(PKG)_HEADERS)"))));
		make_cmd(&make, MCMD(STRV("@mkdir -pv $$(@D)")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_cmd(&make, MCMD(STRV("$(TCC) -m$(BITS) -c $(PKGDIR_SRC:%=-I%) $($(PKG)_INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<")), &act);
		make_rule_add_act(&make, def_rule_obj, act);
		make_def_add_act(&make, def, def_rule_obj);
	}

	make_empty(&make, &act);
	make_add_act(&make, root, act);

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

	str_t buf2 = strz(16);

	arr_t order = {0};
	arr_init(&order, proj->pkgs.pkgs.cnt, sizeof(uint), ALLOC_STD);

	pkgs_get_build_order(&proj->pkgs, &order);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(uint), ALLOC_STD);

	uint i = 0;
	const uint *id;
	arr_foreach(&order, i, id)
	{
		const pkg_t *pkg = pkgs_get(&proj->pkgs, *id);

		strv_t name = pkgs_get_name(&proj->pkgs, *id);

		buf.len = 0;
		str_cat(&buf, STRV("$(BUILDDIR)"));
		str_cat(&buf, strvbuf_get(&proj->pkgs.strs, pkg->strs[PKG_DIR]));
		str_cat(&buf, STRV("pkg.mk"));

		make_act_t inc;
		make_inc(&make, STRVS(buf), &inc);
		make_add_act(&make, root, inc);

		gen_pkg(*id, name, &make, inc, defines, &proj->pkgs, &deps, &buf2, drv->fs, build_dir);

		make_rule_add_depend(&make, test, MRULEACT(MSTR(name), STRV("/test")));
	}

	arr_free(&deps);
	arr_free(&order);

	if (proj->pkgs.pkgs.cnt > 0) {
		make_empty(&make, &act);
		make_add_act(&make, root, act);
	}

	path_t make_path = {0};
	path_init(&make_path, build_dir);
	path_push(&make_path, STRV("Makefile"));

	void *file;
	fs_open(drv->fs, STRVS(make_path), "w", &file);
	make_print(&make, root, DST_FS(drv->fs, file));
	fs_close(drv->fs, file);

	make_free(&make);

	str_free(&outdir);
	str_free(&buf);
	str_free(&buf2);

	return 0;
}

static gen_driver_t make = {
	.param = STRVT("M"),
	.desc  = "Make",
	.gen   = gen_make,
};

GEN_DRIVER(make, &make);
