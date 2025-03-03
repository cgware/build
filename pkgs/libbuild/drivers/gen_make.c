#include "gen.h"

#include "file/make.h"
#include "log.h"
#include "mem.h"
#include "var.h"

#include <stdio.h>

static int gen_make(const proj_t *proj)
{
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
	make_ext_set_val(&make, mcurdir, MSTR(STR(".")));
	make_ext_set_val(&make, march, MSTR(STR("x32")));

	make_var_add_val(
		&make, make_add_act(&make, make_create_var(&make, STRV("BUILDDIR"), MAKE_VAR_INST, NULL)), MSTR(STR("$(CURDIR)/")));
	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STRV("SRCDIR"), MAKE_VAR_INST, NULL)), MSTR(STR("$(CURDIR)/")));
	make_add_act(&make, make_create_empty(&make));
	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STRV("TCC"), MAKE_VAR_INST, NULL)), MSTR(STR("$(CC)")));
	make_add_act(&make, make_create_empty(&make));

	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STRV("ARCH"), MAKE_VAR_INST, &march)), MSTR(STR("x64")));
	uint mbits	 = MAKE_END;
	make_if_t if_x64 = make_add_act(&make, make_create_if(&make, MVAR(march), MSTR(STR("x64"))));
	make_var_add_val(
		&make, make_if_add_true_act(&make, if_x64, make_create_var(&make, STRV("BITS"), MAKE_VAR_INST, &mbits)), MSTR(STR("64")));
	make_if_t if_x86 = make_add_act(&make, make_create_if(&make, MVAR(march), MSTR(STR("x86"))));
	make_var_add_val(
		&make, make_if_add_true_act(&make, if_x86, make_create_var(&make, STRV("BITS"), MAKE_VAR_INST, &mbits)), MSTR(STR("32")));
	make_add_act(&make, make_create_empty(&make));

	make_var_add_val(&make, make_add_act(&make, make_create_var(&make, STRV("CONFIG"), MAKE_VAR_INST, &mconfig)), MSTR(STR("Release")));
	uint mcflags	   = MAKE_END;
	uint mldflags	   = MAKE_END;
	make_if_t if_debug = make_add_act(&make, make_create_if(&make, MVAR(mconfig), MSTR(STR("Debug"))));
	make_var_add_val(&make,
			 make_if_add_true_act(&make, if_debug, make_create_var(&make, STRV("CFLAGS"), MAKE_VAR_INST, &mcflags)),
			 MSTR(STR("-Wall -Wextra -Werror -pedantic -O0 -ggdb -coverage")));
	make_var_add_val(&make,
			 make_if_add_true_act(&make, if_debug, make_create_var(&make, STRV("LDFLAGS"), MAKE_VAR_INST, &mldflags)),
			 MSTR(STR("-coverage")));
	make_if_t if_release = make_add_act(&make, make_create_if(&make, MVAR(mconfig), MSTR(STR("Release"))));
	make_var_add_val(&make,
			 make_if_add_true_act(&make, if_release, make_create_var(&make, STRV("CFLAGS"), MAKE_VAR_INST, &mcflags)),
			 MSTR(STR("-Wall -Wextra -Werror -pedantic")));
	make_var_add_val(&make,
			 make_if_add_true_act(&make, if_release, make_create_var(&make, STRV("LDFLAGS"), MAKE_VAR_INST, &mldflags)),
			 MSTR(STR("")));
	make_add_act(&make, make_create_empty(&make));

	make_var_add_val(&make,
			 make_add_act(&make, make_create_var(&make, STRV("OUTDIR"), MAKE_VAR_INST, NULL)),
			 MSTR(strf(path_is_rel(&outdir) ? "$(BUILDDIR)%.*s" : "%.*s", outdir.len, outdir.data)));

	static const struct {
		strv_t name;
		str_t val;
	} outdirs[] = {
		{STRVS("INTDIR"), STRS("$(OUTDIR)int")},
		{STRVS("LIBSDIR"), STRS("$(OUTDIR)libs")},
		{STRVS("TESTSDIR"), STRS("$(OUTDIR)tests")},
		{STRVS("EXESDIR"), STRS("$(OUTDIR)exes")},
	};

	for (size_t i = 0; i < sizeof(outdirs) / sizeof(outdirs[0]); i++) {
		make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, outdirs[i].name, MAKE_VAR_INST, NULL)), MSTR(outdirs[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	make_var_add_val(&make,
			 make_add_act(&make, make_create_var(&make, STRV("PKGDIR"), MAKE_VAR_REF, NULL)),
			 MSTR(STR("$(dir $(abspath $(lastword $(MAKEFILE_LIST))))")));

	static const struct {
		strv_t name;
		str_t val;
	} pkgdirs[] = {
		{STRVS("PKGSRCDIR"), STRS("$(PKGDIR)src/")},
		{STRVS("PKGINCDIR"), STRS("$(PKGDIR)include/")},
		{STRVS("PKGDRVDIR"), STRS("$(PKGDIR)drivers/")},
		{STRVS("PKGTESTDIR"), STRS("$(PKGDIR)test/")},
	};

	for (size_t i = 0; i < sizeof(pkgdirs) / sizeof(pkgdirs[0]); i++) {
		make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, pkgdirs[i].name, MAKE_VAR_REF, NULL)), MSTR(pkgdirs[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	static const struct {
		strv_t name;
		str_t val;
	} pkgfiles[] = {
		{STRVS("PKGSRC_C"), STRS("$(shell find $(PKGSRCDIR) -type f -name '*.c')")},
		{STRVS("PKGSRC_H"), STRS("$(shell find $(PKGSRCDIR) -type f -name '*.h')")},
		{STRVS("PKGDRV_C"), STRS("$(shell find $(PKGDRVDIR) -type f -name '*.c')")},
		{STRVS("PKGTEST_C"), STRS("$(shell find $(PKGTESTDIR) -type f -name '*.c')")},
		{STRVS("PKGTEST_H"), STRS("$(shell find $(PKGTESTDIR) -type f -name '*.h')")},
		{STRVS("PKGINC_H"), STRS("$(shell find $(PKGINCDIR) -type f -name '*.h')")},
	};

	for (size_t i = 0; i < sizeof(pkgfiles) / sizeof(pkgfiles[0]); i++) {
		make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, pkgfiles[i].name, MAKE_VAR_REF, NULL)), MSTR(pkgfiles[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	static const struct {
		strv_t name;
		str_t val;
	} intdirs[] = {
		{STRVS("INTSRCDIR"), STRS("$(INTDIR)/$(PKGNAME)/src/")},
		{STRVS("INTDRVDIR"), STRS("$(INTDIR)/$(PKGNAME)/drivers/")},
		{STRVS("INTTESTDIR"), STRS("$(INTDIR)/$(PKGNAME)/test/")},
	};

	for (size_t i = 0; i < sizeof(intdirs) / sizeof(intdirs[0]); i++) {
		make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, intdirs[i].name, MAKE_VAR_REF, NULL)), MSTR(intdirs[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	static const struct {
		strv_t name;
		str_t val;
	} objs[] = {
		{STRVS("PKGSRC_OBJ"), STRS("$(patsubst $(PKGSRCDIR)%.c,$(INTSRCDIR)%.o,$(PKGSRC_C))")},
		{STRVS("PKGSRC_GCDA"), STRS("$(patsubst %.o,%.gcda,$(PKGSRC_OBJ))")},
		{STRVS("PKGDRV_OBJ"), STRS("$(patsubst $(PKGDRVDIR)%.c,$(INTDRVDIR)%.o,$(PKGDRV_C))")},
		{STRVS("PKGDRV_GCDA"), STRS("$(patsubst %.o,%.gcda,$(PKGDRV_OBJ))")},
		{STRVS("PKGTEST_OBJ"), STRS("$(patsubst $(PKGTESTDIR)%.c,$(INTTESTDIR)%.o,$(PKGTEST_C))")},
		{STRVS("PKGTEST_GCDA"), STRS("$(patsubst %.o,%.gcda,$(PKGTEST_OBJ))")},
	};

	for (size_t i = 0; i < sizeof(objs) / sizeof(objs[0]); i++) {
		make_var_add_val(&make, make_add_act(&make, make_create_var(&make, objs[i].name, MAKE_VAR_REF, NULL)), MSTR(objs[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	static const struct {
		strv_t name;
		str_t val;
	} targets[] = {
		{STRVS("PKGEXE"), STRS("$(EXESDIR)/$(PKGNAME)")},
		{STRVS("PKGLIB"), STRS("$(LIBSDIR)/$(PKGNAME).a")},
		{STRVS("PKGTEST"), STRS("$(TESTSDIR)/$(PKGNAME)")},
	};

	for (size_t i = 0; i < sizeof(targets) / sizeof(targets[0]); i++) {
		make_var_add_val(
			&make, make_add_act(&make, make_create_var(&make, targets[i].name, MAKE_VAR_REF, NULL)), MSTR(targets[i].val));
	}
	make_add_act(&make, make_create_empty(&make));

	if (proj->is_pkg) {
		make_add_act(&make, make_create_cmd(&make, MCMD(STR("include $(SRCDIR)pkg.mk"))));
	}

	printf("---------------------------\n");
	make_print(&make, PRINT_DST_STD());
	printf("---------------------------\n");

	make_vars_t vars = {0};
	make_vars_init(&make, &vars, ALLOC_STD);

	make_vars_eval(&make, &vars);
	make_vars_print(&make, &vars, PRINT_DST_STD());
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
