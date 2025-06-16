#include "gen.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(gen_make_null)
{
	START;

	gen_driver_t drv = *gen_find_param(STRV("M"));

	EXPECT_EQ(drv.gen(&drv, NULL, STRV_NULL, STRV_NULL), 1);

	END;
}

TEST(gen_make_empty)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 4, 1, ALLOC_STD);

	fs_mkpath(&fs, STRV_NULL, STRV("./tmp/build"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	path_init(&proj.outdir, STRV("bin"));

	gen_driver_t drv = *gen_find_param(STRV("M"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV("./"), STRV("./tmp/build/")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&fs, STRV("./tmp/build/Makefile"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PROJDIR := .." SEP ".." SEP "\n"
		    "BUILDDIR :=\n"
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
		    "LDFLAGS :=\n"
		    "endif\n"
		    "\n"
		    "OUTDIR := $(PROJDIR)bin\n"
		    "INTDIR := $(OUTDIR)int\n"
		    "LIBDIR := $(OUTDIR)lib\n"
		    "BINDIR := $(OUTDIR)bin\n"
		    "TSTDIR := $(OUTDIR)test\n"
		    "\n"
		    "PKGDIR = $(PROJDIR)$($(PKG)_DIR)\n"
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
		    ".PHONY: test coverage\n"
		    "\n"
		    "test:\n"
		    "\n"
		    "coverage: test\n"
		    "	lcov -q -c -o $(PROJDIR)bin/lcov.info -d $(INTDIR)\n"
		    "\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_make_unknown)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 3, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("."));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	proj_add_pkg(&proj, STRV_NULL, &pkg_id);
	proj_add_target(&proj, pkg_id, STRV("pkg"), NULL);

	gen_driver_t drv = *gen_find_param(STRV("M"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV("."), STRV(".")), 0);

	char buf[2400] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&fs, STRV("./Makefile"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PROJDIR :=\n"
		    "BUILDDIR :=\n"
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
		    "LDFLAGS :=\n"
		    "endif\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int\n"
		    "LIBDIR := $(OUTDIR)lib\n"
		    "BINDIR := $(OUTDIR)bin\n"
		    "TSTDIR := $(OUTDIR)test\n"
		    "\n"
		    "PKGDIR = $(PROJDIR)$($(PKG)_DIR)\n"
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
		    "define unknown\n"
		    "endef\n"
		    "\n"
		    ".PHONY: test coverage\n"
		    "\n"
		    "test: /test\n"
		    "\n"
		    "coverage: test\n"
		    "	lcov -q -c -o $(PROJDIR)bin/lcov.info -d $(INTDIR)\n"
		    "\n"
		    "include $(BUILDDIR)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&fs, STRV("./pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PKG := \n"
		    "$(PKG)_DIR :=\n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS :=\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call unknown))\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_make_exe)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 3, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("."));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	proj_add_pkg(&proj, STRV_NULL, &pkg_id);
	target_t *target = proj_add_target(&proj, pkg_id, STRV("pkg"), NULL);
	target->type	 = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(STRV("M"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV("."), STRV(".")), 0);

	char buf[2400] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&fs, STRV("./Makefile"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PROJDIR :=\n"
		    "BUILDDIR :=\n"
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
		    "LDFLAGS :=\n"
		    "endif\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int\n"
		    "LIBDIR := $(OUTDIR)lib\n"
		    "BINDIR := $(OUTDIR)bin\n"
		    "TSTDIR := $(OUTDIR)test\n"
		    "\n"
		    "PKGDIR = $(PROJDIR)$($(PKG)_DIR)\n"
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
		    ".PHONY: test coverage\n"
		    "\n"
		    "test: /test\n"
		    "\n"
		    "coverage: test\n"
		    "	lcov -q -c -o $(PROJDIR)bin/lcov.info -d $(INTDIR)\n"
		    "\n"
		    "include $(BUILDDIR)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&fs, STRV("./pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PKG := \n"
		    "$(PKG)_DIR :=\n"
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

	fs_t fs = {0};
	fs_init(&fs, 3, 1, ALLOC_STD);

	fs_mkdir(&fs, STRV("."));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg_id;
	pkg_t *pkg	 = proj_add_pkg(&proj, STRV_NULL, &pkg_id);
	target_t *target = proj_add_target(&proj, pkg_id, STRV("pkg"), NULL);
	target->type	 = TARGET_TYPE_LIB;
	proj_set_str(&proj, pkg->strs + PKG_INC, STRV("include"));

	gen_driver_t drv = *gen_find_param(STRV("M"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV("."), STRV(".")), 0);

	char buf[2400] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&fs, STRV("./Makefile"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PROJDIR :=\n"
		    "BUILDDIR :=\n"
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
		    "LDFLAGS :=\n"
		    "endif\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int\n"
		    "LIBDIR := $(OUTDIR)lib\n"
		    "BINDIR := $(OUTDIR)bin\n"
		    "TSTDIR := $(OUTDIR)test\n"
		    "\n"
		    "PKGDIR = $(PROJDIR)$($(PKG)_DIR)\n"
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
		    "test: /test\n"
		    "\n"
		    "coverage: test\n"
		    "	lcov -q -c -o $(PROJDIR)bin/lcov.info -d $(INTDIR)\n"
		    "\n"
		    "include $(BUILDDIR)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&fs, STRV("./pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PKG := \n"
		    "$(PKG)_DIR :=\n"
		    "$(PKG)_HEADERS := $(PKGINC_H)\n"
		    "$(PKG)_INCLUDES := $(PKGDIR)include\n"
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

	fs_t fs = {0};
	fs_init(&fs, 6, 1, ALLOC_STD);

	fs_mkpath(&fs, STRV_NULL, STRV("./lib"));
	fs_mkpath(&fs, STRV_NULL, STRV("./exe"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg, lib_id, exe_id;

	pkg_t *lib = proj_add_pkg(&proj, STRV("lib"), &pkg);
	proj_set_str(&proj, lib->strs + PKG_DIR, STRV("lib"));
	target_t *ltarget = proj_add_target(&proj, pkg, STRV("lib"), &lib_id);
	ltarget->type	  = TARGET_TYPE_LIB;

	pkg_t *exe = proj_add_pkg(&proj, STRV("exe"), &pkg);
	proj_set_str(&proj, exe->strs + PKG_DIR, STRV("exe"));
	target_t *etarget = proj_add_target(&proj, pkg, STRV("exe"), &exe_id);
	etarget->type	  = TARGET_TYPE_EXE;

	proj_add_dep(&proj, exe_id, lib_id);

	gen_driver_t drv = *gen_find_param(STRV("M"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV("."), STRV(".")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&fs, STRV("./lib/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PKG := lib\n"
		    "$(PKG)_DIR := lib\n"
		    "$(PKG)_HEADERS :=\n"
		    "$(PKG)_INCLUDES :=\n"
		    "$(PKG)_LIBS :=\n"
		    "$(PKG)_DRIVERS :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&fs, STRV("./exe/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PKG := exe\n"
		    "$(PKG)_DIR := exe\n"
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

	RUN(gen_make_null);
	RUN(gen_make_empty);
	RUN(gen_make_unknown);
	RUN(gen_make_exe);
	RUN(gen_make_lib);
	RUN(gen_make_exe_dep_lib);

	SEND;
}
