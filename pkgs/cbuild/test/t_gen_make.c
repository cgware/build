#include "gen.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(gen_make_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	char buf[2600] = {0};
	str_t tmp      = STRB(buf, 0);
	log_set_quiet(0, 1);
	proj_set_dir(&proj, &fs, STRV_NULL, &tmp);
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

	drv.fs = &fs;

	log_set_quiet(0, 1);
	EXPECT_EQ(drv.gen(&drv, &proj), 0);
	log_set_quiet(0, 0);

	fs_read(&fs, STRV("Makefile"), 0, &tmp);
	EXPECT_STRN(tmp.data,
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
		    "OUTDIR := $(BUILDDIR)bin" SEP "$(ARCH)-$(CONFIG)" SEP "\n"
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
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_make_exe)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 3, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	proj_set_dir(&proj, &fs, STRV_NULL, &tmp);

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

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj), 0);

	fs_read(&fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PKG := \n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS :=\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_make_lib)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 4, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("src"));
	fs_mkdir(&fs, STRV("include"));

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	proj_set_dir(&proj, &fs, STRV_NULL, &tmp);

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

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj), 0);

	fs_read(&fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PKG := \n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS :=\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_make_exe_dep_lib)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	fs_t fs = {0};
	fs_init(&fs, 10, 1, ALLOC_STD);
	// TODO: test without src and lib
	fs_mkdir(&fs, STRV("pkgs"));
	fs_mkdir(&fs, STRV("pkgs/lib"));
	fs_mkdir(&fs, STRV("pkgs/lib/src"));
	fs_mkdir(&fs, STRV("pkgs/lib/include"));
	fs_mkdir(&fs, STRV("pkgs/exe"));
	fs_mkdir(&fs, STRV("pkgs/exe/src"));

	void *f;
	fs_open(&fs, STRV("pkgs/exe/pkg.cfg"), "w", &f);
	fs_write(&fs, f, STRV("deps = [lib]\n"));
	fs_close(&fs, f);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);
	proj_set_dir(&proj, &fs, STRV_NULL, &tmp);

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

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj), 0);

	fs_read(&fs, STRV("pkgs/lib/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PKG := lib\n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS :=\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&fs, STRV("pkgs/exe/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PKG := exe\n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS := $(lib)\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

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
