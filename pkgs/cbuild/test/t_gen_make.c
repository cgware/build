#include "t_gen_common.h"

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

TEST(gen_make_proj_build_dir)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_build_dir(&com, STRV("M")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("tmp/build/Makefile"), 0, &tmp);
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
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int\n"
		    "LIBDIR := $(OUTDIR)lib\n"
		    "BINDIR := $(OUTDIR)bin\n"
		    "TSTDIR := $(OUTDIR)test\n"
		    "\n"
		    "PKGDIR = $(PROJDIR)$($(PN).DIR)\n"
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
		    "INTDIR_SRC = $(INTDIR)/$(PN)/src/\n"
		    "INTDIR_DRV = $(INTDIR)/$(PN)/drivers/\n"
		    "INTDIR_TST = $(INTDIR)/$(PN)/test/\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "PKGEXE = $(BINDIR)/$(PN)\n"
		    "PKGLIB = $(LIBDIR)/$(PN).a\n"
		    "PKGTST = $(TSTDIR)/$(PN)\n"
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

	t_gen_free(&com);

	END;
}

TEST(gen_make_proj_empty)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_empty(&com, STRV("M")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
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
		    "PKGDIR = $(PROJDIR)$($(PN).DIR)\n"
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
		    "INTDIR_SRC = $(INTDIR)/$(PN)/src/\n"
		    "INTDIR_DRV = $(INTDIR)/$(PN)/drivers/\n"
		    "INTDIR_TST = $(INTDIR)/$(PN)/test/\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "PKGEXE = $(BINDIR)/$(PN)\n"
		    "PKGLIB = $(LIBDIR)/$(PN).a\n"
		    "PKGTST = $(TSTDIR)/$(PN)\n"
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

	t_gen_free(&com);

	END;
}

TEST(gen_make_proj_name)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_name(&com, STRV("M")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
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
		    "PKGDIR = $(PROJDIR)$($(PN).DIR)\n"
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
		    "INTDIR_SRC = $(INTDIR)/$(PN)/src/\n"
		    "INTDIR_DRV = $(INTDIR)/$(PN)/drivers/\n"
		    "INTDIR_TST = $(INTDIR)/$(PN)/test/\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "PKGEXE = $(BINDIR)/$(PN)\n"
		    "PKGLIB = $(LIBDIR)/$(PN).a\n"
		    "PKGTST = $(TSTDIR)/$(PN)\n"
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

	t_gen_free(&com);

	END;
}

TEST(gen_make_proj_unknown)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_unknown(&com, STRV("M")), 0);

	char buf[2400] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
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
		    "PKGDIR = $(PROJDIR)$($(PN).DIR)\n"
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
		    "INTDIR_SRC = $(INTDIR)/$(PN)/src/\n"
		    "INTDIR_DRV = $(INTDIR)/$(PN)/drivers/\n"
		    "INTDIR_TST = $(INTDIR)/$(PN)/test/\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "PKGEXE = $(BINDIR)/$(PN)\n"
		    "PKGLIB = $(LIBDIR)/$(PN).a\n"
		    "PKGTST = $(TSTDIR)/$(PN)\n"
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

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN).DIR :=\n"
		    "TN := pkg\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call unknown))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_proj_exe)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_exe(&com, STRV("M")), 0);

	char buf[2400] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
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
		    "PKGDIR = $(PROJDIR)$($(PN).DIR)\n"
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
		    "INTDIR_SRC = $(INTDIR)/$(PN)/src/\n"
		    "INTDIR_DRV = $(INTDIR)/$(PN)/drivers/\n"
		    "INTDIR_TST = $(INTDIR)/$(PN)/test/\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "PKGEXE = $(BINDIR)/$(PN)\n"
		    "PKGLIB = $(LIBDIR)/$(PN).a\n"
		    "PKGTST = $(TSTDIR)/$(PN)\n"
		    "\n"
		    ".PHONY: all\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define exe\n"
		    "$(PN).$(TN) := $(PKGEXE)\n"
		    "\n"
		    "all: $(PN).$(TN)/compile\n"
		    "\n"
		    ".PHONY: $(PN).$(TN)/compile\n"
		    "\n"
		    "$(PN).$(TN)/compile: $(PKGEXE)\n"
		    "\n"
		    "$(PKGEXE): $($(PN).$(TN).DRIVERS) $(PKGSRC_OBJ) $($(PN).$(TN).LIBS)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGSRC_OBJ) $($(PN).$(TN).DRIVERS) $($(PN).$(TN).LIBS)\n"
		    "\n"
		    "$(INTDIR_SRC)%.o: $(PKGDIR_SRC)%.c $(PKGSRC_H) $($(PN).$(TN).HEADERS)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	$(TCC) -m$(BITS) -c $(PKGDIR_SRC:%=-I%) $($(PN).$(TN).INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<\n"
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

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN).DIR :=\n"
		    "TN := pkg\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(PN).$(TN).LIBS :=\n"
		    "$(PN).$(TN).DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_proj_lib)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_lib(&com, STRV("M")), 0);

	char buf[2400] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
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
		    "PKGDIR = $(PROJDIR)$($(PN).DIR)\n"
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
		    "INTDIR_SRC = $(INTDIR)/$(PN)/src/\n"
		    "INTDIR_DRV = $(INTDIR)/$(PN)/drivers/\n"
		    "INTDIR_TST = $(INTDIR)/$(PN)/test/\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "PKGEXE = $(BINDIR)/$(PN)\n"
		    "PKGLIB = $(LIBDIR)/$(PN).a\n"
		    "PKGTST = $(TSTDIR)/$(PN)\n"
		    "\n"
		    ".PHONY: all\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define lib\n"
		    "$(PN).$(TN) := $(PKGLIB)\n"
		    "\n"
		    "all: $(PN).$(TN)/compile\n"
		    "\n"
		    ".PHONY: $(PN).$(TN)/compile\n"
		    "\n"
		    "$(PN).$(TN)/compile: $(PKGLIB)\n"
		    "\n"
		    "$(PKGLIB): $(PKGSRC_OBJ)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	ar rcs $$@ $(PKGSRC_OBJ)\n"
		    "\n"
		    "$(INTDIR_SRC)%.o: $(PKGDIR_SRC)%.c $(PKGSRC_H) $($(PN).$(TN).HEADERS)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	$(TCC) -m$(BITS) -c $(PKGDIR_SRC:%=-I%) $($(PN).$(TN).INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<\n"
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

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN).DIR :=\n"
		    "TN := pkg\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_proj_test)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_test(&com, STRV("M")), 0);

	char buf[2400] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
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
		    "PKGDIR = $(PROJDIR)$($(PN).DIR)\n"
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
		    "INTDIR_SRC = $(INTDIR)/$(PN)/src/\n"
		    "INTDIR_DRV = $(INTDIR)/$(PN)/drivers/\n"
		    "INTDIR_TST = $(INTDIR)/$(PN)/test/\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "PKGEXE = $(BINDIR)/$(PN)\n"
		    "PKGLIB = $(LIBDIR)/$(PN).a\n"
		    "PKGTST = $(TSTDIR)/$(PN)\n"
		    "\n"
		    ".PHONY: all\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define test\n"
		    "$(PN).$(TN) := $(PKGTST)\n"
		    "\n"
		    "all: $(PN).$(TN)/compile\n"
		    "\n"
		    ".PHONY: $(PN).$(TN)/compile\n"
		    "\n"
		    "$(PN).$(TN)/compile: $(PKGTST)\n"
		    "\n"
		    "$(PKGTST): $($(PN).$(TN).DRIVERS) $(PKGTST_OBJ) $($(PN).$(TN).LIBS)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGTST_OBJ) $($(PN).$(TN).DRIVERS) $($(PN).$(TN).LIBS)\n"
		    "\n"
		    "$(INTDIR_TST)%.o: $(PKGDIR_TST)%.c $(PKGSRC_H) $($(PN).$(TN).HEADERS)\n"
		    "	@mkdir -pv $$(@D)\n"
		    "	$(TCC) -m$(BITS) -c $(PKGDIR_TST:%=-I%) $($(PN).$(TN).INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<\n"
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

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN).DIR :=\n"
		    "TN := pkg\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(PN).$(TN).LIBS :=\n"
		    "$(PN).$(TN).DRIVERS :=\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_exe)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN).DIR :=\n"
		    "TN := pkg\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(PN).$(TN).LIBS :=\n"
		    "$(PN).$(TN).DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN).DIR :=\n"
		    "TN := pkg\n"
		    "$(PN).$(TN).HEADERS := $(PKGINC_H)\n"
		    "$(PN).$(TN).INCLUDES := $(PKGDIR)include\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_test)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib\n"
		    "$(PN).DIR :=\n"
		    "TN := lib\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n"
		    "TN := test\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(PN).$(TN).LIBS := $(lib.lib)\n"
		    "$(PN).$(TN).DRIVERS :=\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_multi)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_multi(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("./a/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := a\n"
		    "$(PN).DIR := a\n"
		    "TN := a\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(PN).$(TN).LIBS :=\n"
		    "$(PN).$(TN).DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./b/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := b\n"
		    "$(PN).DIR := b\n"
		    "TN := b\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(PN).$(TN).LIBS :=\n"
		    "$(PN).$(TN).DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_depends)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_depends(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("./lib/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib\n"
		    "$(PN).DIR := lib\n"
		    "TN := lib\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := exe\n"
		    "$(PN).DIR := exe\n"
		    "TN := exe\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(PN).$(TN).LIBS := $(lib.lib)\n"
		    "$(PN).$(TN).DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_rdepends)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_rdepends(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("./base/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := base\n"
		    "$(PN).DIR := base\n"
		    "TN := base\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib1/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib1\n"
		    "$(PN).DIR := lib1\n"
		    "TN := lib1\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib2/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib2\n"
		    "$(PN).DIR := lib2\n"
		    "TN := lib2\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := exe\n"
		    "$(PN).DIR := exe\n"
		    "TN := exe\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(PN).$(TN).LIBS := $(lib1.lib1) $(lib2.lib2) $(base.base)\n"
		    "$(PN).$(TN).DRIVERS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

STEST(gen_make)
{
	SSTART;

	RUN(gen_make_null);
	RUN(gen_make_proj_build_dir);
	RUN(gen_make_proj_empty);
	RUN(gen_make_proj_name);
	RUN(gen_make_proj_unknown);
	RUN(gen_make_proj_exe);
	RUN(gen_make_proj_lib);
	RUN(gen_make_proj_test);
	RUN(gen_make_pkg_exe);
	RUN(gen_make_pkg_lib);
	RUN(gen_make_pkg_lib_test);
	RUN(gen_make_pkg_multi);
	RUN(gen_make_pkg_depends);
	RUN(gen_make_pkg_rdepends);

	SEND;
}
