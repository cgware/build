#include "gen.h"

#include "log.h"
#include "mem.h"
#include "test.h"

#include <string.h>

static int file;

void *test_empty_open(print_dst_ex_t dst, const char *path, const char *mode)
{
	CSTART;

	(void)mode;

	switch (file) {
	case 0:
		EXPECT_STR(path, "./test/empty/Makefile");
		break;
	default:
		EXPECT_FAIL("Unexpected file: %s\n", path);
		break;
	}

	CEND;

	return dst.dst.dst;
}

int test_empty_close(print_dst_ex_t dst)
{
	CSTART;

	switch (file) {
	case 0:
		EXPECT_STRN(dst.dst.dst,
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
			    "CONFIG := Release\n"
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
			    "define pkg/exe\n"
			    ".PHONY: $(PKG)/compile\n"
			    "\n"
			    "$(PKG)/compile: $(PKGEXE)\n"
			    "\n"
			    "$(PKGEXE): $($(PKG)_DRIVERS) $(PKGSRC_OBJ) $($(PKG)_LIBS)\n"
			    "	@mkdir -pv $$(@D)\n"
			    "	$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGSRC_OBJ) $($(PKG)_DRIVERS) -L$(LIBDIR) $$($$(notdir "
			    "$($(PKG)_LIBS)):%=-l:%)\n"
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
			    2095);
		break;
	default:
		EXPECT_FAIL("%s", "Unexpected file");
		break;
	}

	file++;

	CEND;

	return 0;
}

TEST(gen_make_empty)
{
	START;

	strv_t dir = STRV("./test/empty/");

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);
	log_set_quiet(0, 1);
	proj_set_dir(&proj, dir);
	log_set_quiet(0, 0);

	gen_driver_t *drvi = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			drvi = i->data;
			if (strv_eq(STRV(drvi->param), STRV("M"))) {
				break;
			}
		}
	}

	char buf[4096] = {0};

	gen_driver_t drv = *drvi;
	drv.dst = ((print_dst_ex_t){.dst = PRINT_DST_BUF(buf, sizeof(buf), 0), .open = test_empty_open, .close = test_empty_close});

	file = 0;

	drv.gen(&drv, &proj);

	proj_free(&proj);

	END;
}

void *test_exe_open(print_dst_ex_t dst, const char *path, const char *mode)
{
	CSTART;

	(void)mode;

	switch (file) {
	case 0:
		EXPECT_STR(path, "./test/exe/pkg.mk");
		break;
	case 1:
		EXPECT_STR(path, "./test/exe/Makefile");
		break;
	default:
		EXPECT_FAIL("Unexpected file: %s\n", path);
		break;
	}

	CEND;

	return dst.dst.dst;
}

int test_exe_close(print_dst_ex_t dst)
{
	CSTART;

	switch (file) {
	case 0:
		EXPECT_STRN(dst.dst.dst,
			    "PKG := exe\n"
			    "$(PKG)_HEADERS :=\n"
			    "$(PKG)_INCLUDES :=\n"
			    "$(PKG)_LIBS :=\n"
			    "$(PKG)_DRIVERS :=\n"
			    "$(eval $(call pkg/exe))\n",
			    105);
		break;
	case 1:
		break;
	default:
		EXPECT_FAIL("%s", "Unexpected file");
		break;
	}

	file++;

	CEND;

	return 0;
}

TEST(gen_make_exe)
{
	START;

	strv_t dir = STRV("./test/exe/");

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);
	proj_set_dir(&proj, dir);

	gen_driver_t *drvi = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			drvi = i->data;
			if (strv_eq(STRV(drvi->param), STRV("M"))) {
				break;
			}
		}
	}

	char buf[4096] = {0};

	gen_driver_t drv = *drvi;
	drv.dst		 = ((print_dst_ex_t){.dst = PRINT_DST_BUF(buf, sizeof(buf), 0), .open = test_exe_open, .close = test_exe_close});

	file = 0;

	drv.gen(&drv, &proj);

	proj_free(&proj);

	END;
}

STEST(gen_make)
{
	SSTART;

	RUN(gen_make_empty);
	RUN(gen_make_exe);

	SEND;
}
