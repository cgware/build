#include "gen.h"

#include "file/make.h"
#include "log.h"
#include "mem.h"
#include "var.h"

#include <stdio.h>

static int gen_pkg(const pkg_t *pkg, print_dst_ex_t dst, str_t buf)
{
	make_t make = {0};
	make_init(&make, 32, 32, 2, 32, ALLOC_STD);

	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STRV("PKGNAME"), MAKE_VAR_INST, NULL)), MSTR(pkg->name));

	buf.len = 0;
	str_cat(&buf, pkg->name);
	size_t len = buf.len;
	str_cat(&buf, STRV("_HEADERS"));
	make_add_act(&make, make_create_var(&make, STRV_STR(buf), MAKE_VAR_INST, NULL));
	buf.len = len;
	str_cat(&buf, STRV("_INCLUDES"));
	make_add_act(&make, make_create_var(&make, STRV_STR(buf), MAKE_VAR_INST, NULL));
	buf.len = len;
	str_cat(&buf, STRV("_LIBS"));
	make_add_act(&make, make_create_var(&make, STRV_STR(buf), MAKE_VAR_INST, NULL));
	buf.len = len;
	str_cat(&buf, STRV("_DRIVERS"));
	make_add_act(&make, make_create_var(&make, STRV_STR(buf), MAKE_VAR_INST, NULL));

	make_add_act(&make, make_create_cmd(&make, MCMD(STRV("$(eval $(call pkg/exe,exe))"))));

	path_t make_path = pkg->dir;
	path_child(&make_path, STRV("pkg.mk"));

	dst.dst.dst = c_dopen(dst, make_path.data, "w");
	dst.dst.off += make_print(&make, dst.dst);
	c_dclose(dst);

	make_vars_t vars = {0};
	make_vars_init(&make, &vars, ALLOC_STD);

	make_vars_eval(&make, &vars);
	make_vars_print(&vars, PRINT_DST_STD());
	make_vars_free(&vars);

	make_free(&make);
	return 0;
}

static int gen_make(const gen_driver_t *drv, const proj_t *proj)
{
	char buff[256] = {0};
	str_t buf      = strb(buff, sizeof(buff), 0);

	strv_t values[__VAR_CNT] = {
		[VAR_ARCH]   = STRVS("$(ARCH)"),
		[VAR_CONFIG] = STRVS("$(CONFIG)"),
	};

	path_t outdir = proj->outdir;

	str_t outdir_str = strb(outdir.data, sizeof(outdir.data) - 1, outdir.len);
	if (var_replace(&outdir_str, values)) {
		return 1;
	}

	outdir.len = outdir_str.len;

	make_t make = {0};
	make_init(&make, 32, 32, 2, 32, ALLOC_STD);

	uint mcurdir = MAKE_END;
	uint march   = MAKE_END;
	uint mconfig = MAKE_END;
	make_add_act(&make, make_create_var_ext(&make, STRV("CURDIR"), &mcurdir));
	make_add_act(&make, make_create_var_ext(&make, STRV("ARCH"), &march));
	make_add_act(&make, make_create_var_ext(&make, STRV("CONFIG"), &mconfig));
	make_ext_set_val(&make, mcurdir, MSTR(STRV(".")));
	make_ext_set_val(&make, march, MSTR(STRV("x64")));

	make_var_add_val(
		&make, make_add_act(&make, make_create_var(&make, STRV("BUILDDIR"), MAKE_VAR_INST, NULL)), MSTR(STRV("$(CURDIR)/")));
	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STRV("SRCDIR"), MAKE_VAR_INST, NULL)), MSTR(STRV("$(CURDIR)/")));
	make_add_act(&make, make_create_empty(&make));
	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STRV("TCC"), MAKE_VAR_INST, NULL)), MSTR(STRV("$(CC)")));
	make_add_act(&make, make_create_empty(&make));

	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STRV("ARCH"), MAKE_VAR_INST, &march)), MSTR(STRV("x64")));
	uint mbits	 = MAKE_END;
	make_if_t if_x64 = make_add_act(&make, make_create_if(&make, MVAR(march), MSTR(STRV("x64"))));
	make_var_add_val(
		&make, make_if_add_true_act(&make, if_x64, make_create_var(&make, STRV("BITS"), MAKE_VAR_INST, &mbits)), MSTR(STRV("64")));
	make_if_t if_x86 = make_add_act(&make, make_create_if(&make, MVAR(march), MSTR(STRV("x86"))));
	make_var_add_val(
		&make, make_if_add_true_act(&make, if_x86, make_create_var(&make, STRV("BITS"), MAKE_VAR_INST, &mbits)), MSTR(STRV("32")));
	make_add_act(&make, make_create_empty(&make));

	make_var_add_val(
		&make, make_add_act(&make, make_create_var(&make, STRV("CONFIG"), MAKE_VAR_INST, &mconfig)), MSTR(STRV("Release")));
	uint mcflags	   = MAKE_END;
	uint mldflags	   = MAKE_END;
	make_if_t if_debug = make_add_act(&make, make_create_if(&make, MVAR(mconfig), MSTR(STRV("Debug"))));
	make_var_add_val(&make,
			 make_if_add_true_act(&make, if_debug, make_create_var(&make, STRV("CFLAGS"), MAKE_VAR_INST, &mcflags)),
			 MSTR(STRV("-Wall -Wextra -Werror -pedantic -O0 -ggdb -coverage")));
	make_var_add_val(&make,
			 make_if_add_true_act(&make, if_debug, make_create_var(&make, STRV("LDFLAGS"), MAKE_VAR_INST, &mldflags)),
			 MSTR(STRV("-coverage")));
	make_if_t if_release = make_add_act(&make, make_create_if(&make, MVAR(mconfig), MSTR(STRV("Release"))));
	make_var_add_val(&make,
			 make_if_add_true_act(&make, if_release, make_create_var(&make, STRV("CFLAGS"), MAKE_VAR_INST, &mcflags)),
			 MSTR(STRV("-Wall -Wextra -Werror -pedantic")));
	make_var_add_val(&make,
			 make_if_add_true_act(&make, if_release, make_create_var(&make, STRV("LDFLAGS"), MAKE_VAR_INST, &mldflags)),
			 MSTR(STRV("")));
	make_add_act(&make, make_create_empty(&make));

	buf.len = 0;
	if (path_is_rel(&outdir)) {
		str_cat(&buf, STRV("$(BUILDDIR)"));
	}
	str_cat(&buf, STRVN(outdir.data, outdir.len));

	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STRV("OUTDIR"), MAKE_VAR_INST, NULL)), MSTR(STRV_STR(buf)));

	static const struct {
		strv_t name;
		strv_t val;
	} outdirs[] = {
		{STRVS("INTDIR"), STRVS("$(OUTDIR)int")},
		{STRVS("LIBSDIR"), STRVS("$(OUTDIR)libs")},
		{STRVS("TESTSDIR"), STRVS("$(OUTDIR)tests")},
		{STRVS("EXESDIR"), STRVS("$(OUTDIR)exes")},
	};

	for (size_t i = 0; i < sizeof(outdirs) / sizeof(outdirs[0]); i++) {
		make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, outdirs[i].name, MAKE_VAR_INST, NULL)), MSTR(outdirs[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	make_var_add_val(&make,
			 make_add_act(&make, make_create_var(&make, STRV("PKGDIR"), MAKE_VAR_REF, NULL)),
			 MSTR(STRV("$(dir $(abspath $(lastword $(MAKEFILE_LIST))))")));

	enum {
		PKGDIR_SRC,
		PKGDIR_INC,
		PKGDIR_DRV,
		PKGDIR_TST,
	};

	struct {
		strv_t name;
		strv_t val;
		make_var_t var;
	} pkgdirs[] = {
		[PKGDIR_SRC] = {STRVS("PKGDIR_SRC"), STRVS("$(PKGDIR)src/")},
		[PKGDIR_INC] = {STRVS("PKGDIR_INC"), STRVS("$(PKGDIR)include/")},
		[PKGDIR_DRV] = {STRVS("PKGDIR_DRV"), STRVS("$(PKGDIR)drivers/")},
		[PKGDIR_TST] = {STRVS("PKGDIR_TST"), STRVS("$(PKGDIR)test/")},
	};

	for (size_t i = 0; i < sizeof(pkgdirs) / sizeof(pkgdirs[0]); i++) {
		pkgdirs[i].var = make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, pkgdirs[i].name, MAKE_VAR_REF, NULL)), MSTR(pkgdirs[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

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
		make_var_t var;
	} pkgfiles[] = {
		[PKGSRC_C] = {STRVS("PKGSRC_C"), STRVS("$(shell find $(PKGDIR_SRC) -type f -name '*.c')")},
		[PKGSRC_H] = {STRVS("PKGSRC_H"), STRVS("$(shell find $(PKGDIR_SRC) -type f -name '*.h')")},
		[PKGDRV_C] = {STRVS("PKGDRV_C"), STRVS("$(shell find $(PKGDIR_DRV) -type f -name '*.c')")},
		[PKGTST_C] = {STRVS("PKGTST_C"), STRVS("$(shell find $(PKGDIR_TST) -type f -name '*.c')")},
		[PKGTST_H] = {STRVS("PKGTST_H"), STRVS("$(shell find $(PKGDIR_TST) -type f -name '*.h')")},
		[PKGINC_H] = {STRVS("PKGINC_H"), STRVS("$(shell find $(PKGDIR_INC) -type f -name '*.h')")},
	};

	for (size_t i = 0; i < sizeof(pkgfiles) / sizeof(pkgfiles[0]); i++) {
		pkgfiles[i].var = make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, pkgfiles[i].name, MAKE_VAR_REF, NULL)), MSTR(pkgfiles[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	enum {
		INTDIR_SRC,
		INTDIR_DRV,
		INTDIR_TST,
	};

	struct {
		strv_t name;
		strv_t val;
		make_var_t var;
	} intdirs[] = {
		[INTDIR_SRC] = {STRVS("INTDIR_SRC"), STRVS("$(INTDIR)/$(PKGNAME)/src/")},
		[INTDIR_DRV] = {STRVS("INTDIR_DRV"), STRVS("$(INTDIR)/$(PKGNAME)/drivers/")},
		[INTDIR_TST] = {STRVS("INTDIR_TST"), STRVS("$(INTDIR)/$(PKGNAME)/test/")},
	};

	for (size_t i = 0; i < sizeof(intdirs) / sizeof(intdirs[0]); i++) {
		intdirs[i].var = make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, intdirs[i].name, MAKE_VAR_REF, NULL)), MSTR(intdirs[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

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
		make_var_t var;
	} objs[] = {
		[PKGSRC_OBJ]  = {STRVS("PKGSRC_OBJ"), STRVS("$(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))")},
		[PKGSRC_GCDA] = {STRVS("PKGSRC_GCDA"), STRVS("$(patsubst %.o,%.gcda,$(PKGSRC_OBJ))")},
		[PKGDRV_OBJ]  = {STRVS("PKGDRV_OBJ"), STRVS("$(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))")},
		[PKGDRV_GCDA] = {STRVS("PKGDRV_GCDA"), STRVS("$(patsubst %.o,%.gcda,$(PKGDRV_OBJ))")},
		[PKGTST_OBJ]  = {STRVS("PKGTST_OBJ"), STRVS("$(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))")},
		[PKGTST_GCDA] = {STRVS("PKGTST_GCDA"), STRVS("$(patsubst %.o,%.gcda,$(PKGTST_OBJ))")},
	};

	for (size_t i = 0; i < sizeof(objs) / sizeof(objs[0]); i++) {
		objs[i].var = make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, objs[i].name, MAKE_VAR_REF, NULL)), MSTR(objs[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	enum {
		PKGEXE,
		PKGLIB,
		PKGTST,
	};

	struct {
		strv_t name;
		strv_t val;
		make_var_t var;
	} targets[] = {
		[PKGEXE] = {STRVS("PKGEXE"), STRVS("$(EXESDIR)/$(PKGNAME)")},
		[PKGLIB] = {STRVS("PKGLIB"), STRVS("$(LIBSDIR)/$(PKGNAME).a")},
		[PKGTST] = {STRVS("PKGTST"), STRVS("$(TESTSDIR)/$(PKGNAME)")},
	};

	for (size_t i = 0; i < sizeof(targets) / sizeof(targets[0]); i++) {
		targets[i].var = make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, targets[i].name, MAKE_VAR_REF, NULL)), MSTR(targets[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	make_def_t def_exe = make_add_act(&make, make_create_def(&make, STRV("pkg/exe")));

	make_var_add_val(
		&make, make_def_add_act(&make, def_exe, make_create_var(&make, STRV("PKGNAME"), MAKE_VAR_INST, NULL)), MSTR(STRV("$(1)")));

	make_var_add_val(&make,
			 make_def_add_act(&make, def_exe, make_create_var(&make, STRV("$(1)_SRC"), MAKE_VAR_INST, NULL)),
			 MVAR(pkgdirs[PKGDIR_SRC].var));

	make_var_add_val(&make,
			 make_def_add_act(&make, def_exe, make_create_var(&make, STRV("$(1)_H"), MAKE_VAR_INST, NULL)),
			 MVAR(pkgfiles[PKGSRC_H].var));

	make_var_add_val(&make,
			 make_def_add_act(&make, def_exe, make_create_var(&make, STRV("$(1)_INTDIR"), MAKE_VAR_INST, NULL)),
			 MVAR(intdirs[INTDIR_SRC].var));

	make_var_t def_exe_obj =
		make_var_add_val(&make,
				 make_def_add_act(&make, def_exe, make_create_var(&make, STRV("$(1)_OBJ"), MAKE_VAR_INST, NULL)),
				 MVAR(objs[PKGSRC_OBJ].var));

	make_var_add_val(&make,
			 make_def_add_act(&make, def_exe, make_create_var(&make, STRV("$(1)_GCDA"), MAKE_VAR_INST, NULL)),
			 MVAR(objs[PKGSRC_GCDA].var));

	make_var_t def_exe_target =
		make_var_add_val(&make,
				 make_def_add_act(&make, def_exe, make_create_var(&make, STRV("$(1)"), MAKE_VAR_INST, NULL)),
				 MVAR(targets[PKGEXE].var));

	make_def_add_act(&make, def_exe, make_create_empty(&make));

	make_rule_t def_exe_phony = make_def_add_act(&make, def_exe, make_create_phony(&make));

	make_rule_add_depend(&make, def_exe_phony, MRULEACT(MSTR(STRV("$(1)")), STRV("/compile")));
	make_rule_t def_exe_compile =
		make_def_add_act(&make, def_exe, make_create_rule(&make, MRULEACT(MSTR(STRV("$(1)")), STRV("/compile")), 1));
	make_rule_add_depend(&make, def_exe_compile, MRULE(MVAR(def_exe_target)));

	make_rule_t def_exe_rule_target = make_def_add_act(&make, def_exe, make_create_rule(&make, MRULE(MVAR(def_exe_target)), 1));
	make_rule_add_depend(&make, def_exe_compile, MRULE(MSTR(STRV("$(1)_DRIVERS"))));
	make_rule_add_depend(&make, def_exe_compile, MRULE(MVAR(def_exe_obj)));
	make_rule_add_depend(&make, def_exe_compile, MRULE(MSTR(STRV("$(1)_LIBS"))));

	make_rule_add_act(&make, def_exe_rule_target, make_create_cmd(&make, MCMD(STRV("@mkdir -pv $(@D)"))));
	make_rule_add_act(&make,
			  def_exe_rule_target,
			  make_create_cmd(&make,
					  MCMD(STRV("$(TCC) -m$(BITS) $(LDFLAGS) -o $@ $($(1)_OBJ) $($(1)_DRIVERS) -L$(LIBSDIR) $(patsubst "
						    "%,-l:%,$(notdir $($(1)_LIBS)))"))));

	make_rule_t def_exe_rule_obj = make_def_add_act(&make, def_exe, make_create_rule(&make, MRULE(MSTR(STRV("$($(1)_INTDIR)%.o"))), 1));
	make_rule_add_act(&make, def_exe_rule_obj, make_create_cmd(&make, MCMD(STRV("@mkdir -pv $(@D)"))));
	make_rule_add_act(
		&make,
		def_exe_rule_obj,
		make_create_cmd(&make,
				MCMD(STRV("$(TCC) -m$(BITS) -c $(patsubst %,-I%,$($(1)_SRC) $($(1)_INCLUDES)) $(CFLAGS) -o $@ $<"))));

	make_add_act(&make, make_create_empty(&make));

	if (proj->is_pkg) {
		gen_pkg(&proj->pkg, drv->dst, buf);

		// TODO: generate package and extract $(INTDIR)/libbuild path from it by expanding variables
		make_add_act(&make, make_create_cmd(&make, MCMD(STRV("include $(SRCDIR)pkg.mk"))));
		make_add_act(&make, make_create_empty(&make));

		make_rule_t phone = make_add_act(&make, make_create_phony(&make));

		make_rule_add_depend(&make, phone, MRULE(MSTR(STRV("test"))));
		make_rule_t test = make_add_act(&make, make_create_rule(&make, MRULE(MSTR(STRV("test"))), 1));
		make_rule_add_depend(&make, test, MRULEACT(MSTR(proj->pkg.name), STRV("/test")));

		make_rule_add_depend(&make, phone, MRULE(MSTR(STRV("coverage"))));
		make_rule_t cov = make_add_act(&make, make_create_rule(&make, MRULE(MSTR(STRV("coverage"))), 1));
		make_rule_add_depend(&make, cov, MRULE(MSTR(STRV("test"))));

		make_rule_add_act(&make,
				  cov,
				  make_create_cmd(&make,
						  MCMD(STRV("lcov -q -c -o $(BUILDDIR)/bin/lcov.info -d $(INTDIR)/libbuild/src -d "
							    "$(INTDIR)/libbuild/drivers"))));
	}

	path_t make_path = proj->dir;
	path_child(&make_path, STRV("Makefile"));

	print_dst_ex_t dst = drv->dst;

	dst.dst.dst = c_dopen(dst, make_path.data, "w");
	dst.dst.off += make_print(&make, dst.dst);
	c_dclose(dst);

	make_vars_t vars = {0};
	make_vars_init(&make, &vars, ALLOC_STD);

	make_vars_eval(&make, &vars);
	make_vars_print(&vars, PRINT_DST_STD());
	make_vars_free(&vars);

	make_free(&make);

	return 0;
}

static gen_driver_t make = {
	.param = "M",
	.desc  = "Make",
	.gen   = gen_make,
};

GEN_DRIVER(make, &make);
