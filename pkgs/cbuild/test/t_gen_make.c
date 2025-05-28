#include "gen.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(gen_make_empty)
{
	START;

	strv_t dir = STRV("./test/empty/");

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t tfs = {0};
	fs_init(&tfs, 0, 0, ALLOC_STD);

	log_set_quiet(0, 1);
	proj_set_dir(&proj, &tfs, dir);
	log_set_quiet(0, 0);

	gen_driver_t *drvi = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			drvi = i->data;
			if (strv_eq(drvi->param, STRV("M"))) {
				break;
			}
		}
	}

	gen_driver_t drv = *drvi;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("."));
	fs_mkdir(&fs, STRV("./test"));
	fs_mkdir(&fs, STRV("./test/empty"));

	drv.fs = &fs;

	log_set_quiet(0, 1);
	EXPECT_EQ(drv.gen(&drv, &proj), 0);
	log_set_quiet(0, 0);

	str_t buf = strz(2600);
	fs_read(&fs, STRV("./test/empty/Makefile"), 0, &buf);
	EXPECT_STRN(buf.data,
		    "BUILDDIR := \n"
		    "SRCDIR := \n"
		    "\n"
		    "TCC := $(CC)\n"
		    "\n"
		    "ARCH := x64\n"
		    "ifeq ($(ARCH),x64)\n"
		    "BITS := 64\n"
		    "endif\n"
		    "ifeq ($(ARCH),x86)\n"
		    "BITS := 32\n"
		    "endif\n"
		    "\n"
		    "CONFIG := Debug\n"
		    "ifeq ($(CONFIG),Debug)\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic -O0 -ggdb -coverage\n"
		    "LDFLAGS := -coverage\n"
		    "endif\n"
		    "ifeq ($(CONFIG),Release)\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "LDFLAGS := \n"
		    "endif\n"
		    "\n"
		    "OUTDIR := $(BUILDDIR)bin" SEP "$(ARCH)-$(CONFIG)/\n"
		    "INTDIR := $(OUTDIR)int\n"
		    "LIBDIR := $(OUTDIR)lib\n"
		    "BINDIR := $(OUTDIR)bin\n"
		    "TSTDIR := $(OUTDIR)test\n"
		    "\n"
		    "PKGDIR = $(dir $(lastword $(MAKEFILE_LIST)))\n"
		    "PKGDIR_SRC = $(PKGDIR)src/\n"
		    "PKGDIR_INC = $(PKGDIR)include/\n"
		    "PKGDIR_DRV = $(PKGDIR)drivers/\n"
		    "PKGDIR_TST = $(PKGDIR)test/\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(PKGDIR_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(PKGDIR_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(PKGDIR_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(PKGDIR_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(PKGDIR_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(PKGDIR_INC) -type f -name '*.h')\n"
		    "\n"
		    "INTDIR_SRC = $(INTDIR)/$(PKG)/src/\n"
		    "INTDIR_DRV = $(INTDIR)/$(PKG)/drivers/\n"
		    "INTDIR_TST = $(INTDIR)/$(PKG)/test/\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "PKGEXE = $(BINDIR)/$(PKG)\n"
		    "PKGLIB = $(LIBDIR)/$(PKG).a\n"
		    "PKGTST = $(TSTDIR)/$(PKG)\n"
		    "\n"
		    ".PHONY: all\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define exe\n"
		    "$(PKG) := $(PKGEXE)\n"
		    "\n"
		    "all: $(PKG)/compile\n"
		    "\n"
		    ".PHONY: $(PKG)/compile\n"
		    "\n"
		    "$(PKG)/compile: $(PKGEXE)\n"
		    "\n"
		    "$(PKGEXE): $($(PKG)_DRIVERS) $(PKGSRC_OBJ) $($(PKG)_LIBS)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGSRC_OBJ) $($(PKG)_DRIVERS) $($(PKG)_LIBS)\n"
		    "\n"
		    "$(INTDIR_SRC)%.o: $(PKGDIR_SRC)%.c $(PKGSRC_H) $($(PKG)_HEADERS)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	$(TCC) -m$(BITS) -c $(PKGDIR_SRC:%=-I%) $($(PKG)_INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define lib\n"
		    "$(PKG) := $(PKGLIB)\n"
		    "\n"
		    "all: $(PKG)/compile\n"
		    "\n"
		    ".PHONY: $(PKG)/compile\n"
		    "\n"
		    "$(PKG)/compile: $(PKGLIB)\n"
		    "\n"
		    "$(PKGLIB): $(PKGSRC_OBJ)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	ar rcs $$@ $(PKGSRC_OBJ)\n"
		    "\n"
		    "$(INTDIR_SRC)%.o: $(PKGDIR_SRC)%.c $(PKGSRC_H) $($(PKG)_HEADERS)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	$(TCC) -m$(BITS) -c $(PKGDIR_SRC:%=-I%) $($(PKG)_INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<\n"
		    "\n"
		    "endef\n"
		    "\n"
		    ".PHONY: test coverage\n"
		    "\n"
		    "test:\n"
		    "\n"
		    "coverage: test\n"
		    "	lcov -q -c -o $(BUILDDIR)/bin/lcov.info -d $(INTDIR)\n"
		    "\n",
		    buf.len);

	proj_free(&proj);
	fs_free(&fs);
	fs_free(&tfs);
	str_free(&buf);

	END;
}

TEST(gen_make_exe)
{
	START;

	strv_t dir = STRV("./test/exe/");

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t tfs = {0};
	fs_init(&tfs, 0, 0, ALLOC_STD);

	proj_set_dir(&proj, &tfs, dir);

	gen_driver_t *drvi = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			drvi = i->data;
			if (strv_eq(drvi->param, STRV("M"))) {
				break;
			}
		}
	}

	gen_driver_t drv = *drvi;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("."));
	fs_mkdir(&fs, STRV("./test"));
	fs_mkdir(&fs, STRV("./test/exe"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj), 0);

	str_t buf = strz(128);
	fs_read(&fs, STRV("./test/exe/pkg.mk"), 0, &buf);
	EXPECT_STRN(buf.data,
		    "PKG := exe\n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS :=\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    buf.len);

	proj_free(&proj);
	fs_free(&fs);
	fs_free(&tfs);
	str_free(&buf);

	END;
}

TEST(gen_make_lib)
{
	START;

	strv_t dir = STRV("./test/lib/");

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t tfs = {0};
	fs_init(&tfs, 0, 0, ALLOC_STD);

	proj_set_dir(&proj, &tfs, dir);

	gen_driver_t *drvi = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			drvi = i->data;
			if (strv_eq(drvi->param, STRV("M"))) {
				break;
			}
		}
	}

	gen_driver_t drv = *drvi;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("."));
	fs_mkdir(&fs, STRV("./test"));
	fs_mkdir(&fs, STRV("./test/lib"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj), 0);

	str_t buf = strz(128);
	fs_read(&fs, STRV("./test/lib/pkg.mk"), 0, &buf);
	EXPECT_STRN(buf.data,
		    "PKG := lib\n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS :=\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call lib))\n",
		    buf.len);

	proj_free(&proj);
	fs_free(&fs);
	fs_free(&tfs);
	str_free(&buf);

	END;
}

TEST(gen_make_exe_dep_lib)
{
	START;

	strv_t dir = STRV("./test/exe_dep_lib/");

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t tfs = {0};
	fs_init(&tfs, 0, 0, ALLOC_STD);

	proj_set_dir(&proj, &tfs, dir);

	gen_driver_t *drvi = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			drvi = i->data;
			if (strv_eq(drvi->param, STRV("M"))) {
				break;
			}
		}
	}

	gen_driver_t drv = *drvi;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("."));
	fs_mkdir(&fs, STRV("./test"));
	fs_mkdir(&fs, STRV("./test/exe_dep_lib"));
	fs_mkdir(&fs, STRV("./test/exe_dep_lib/pkgs"));
	fs_mkdir(&fs, STRV("./test/exe_dep_lib/pkgs" SEP "lib"));
	fs_mkdir(&fs, STRV("./test/exe_dep_lib/pkgs" SEP "exe"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj), 0);

	str_t buf = strz(128);
	fs_read(&fs, STRV("./test/exe_dep_lib/pkgs" SEP "lib" SEP "pkg.mk"), 0, &buf);
	EXPECT_STRN(buf.data,
		    "PKG := lib\n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS :=\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call lib))\n",
		    buf.len);

	fs_read(&fs, STRV("./test/exe_dep_lib/pkgs" SEP "exe" SEP "pkg.mk"), 0, &buf);
	EXPECT_STRN(buf.data,
		    "PKG := exe\n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS := $(lib)\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    buf.len);

	proj_free(&proj);
	fs_free(&fs);
	fs_free(&tfs);
	str_free(&buf);

	END;
}

STEST(gen_make)
{
	SSTART;

	RUN(gen_make_empty);
	RUN(gen_make_exe);
	RUN(gen_make_lib);
	RUN(gen_make_exe_dep_lib);

	SEND;
}
