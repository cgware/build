#include "t_gen_common.h"

#include "log.h"
#include "mem.h"
#include "path.h"
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

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("tmp/build/Makefile"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "ARCH := x64\n"
		    "CONFIG := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "DIR_PROJ := ../../\n"
		    "DIR_BUILD :=\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_DLFILE = $($(PN)_DLFILE)\n"
		    "PKG_DLROOT = $($(PN)_DLROOT)\n"
		    "TGT_CMD = $($(PN)_$(TN)_CMD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "DIR_TMP := $(DIR_PROJ)tmp/\n"
		    "DIR_TMP_EXT := $(DIR_TMP)ext/\n"
		    "DIR_TMP_EXT_PKG = $(DIR_TMP_EXT)$(PKG_DIR)\n"
		    "DIR_TMP_EXT_PKG_ROOT = $(DIR_TMP_EXT_PKG)$(PKG_DLROOT)\n"
		    "DIR_TMP_EXT_PKG_ROOT_OUT = $(DIR_TMP_EXT_PKG_ROOT)$(TGT_OUT)\n"
		    "DIR_TMP_REP := $(DIR_TMP)report/\n"
		    "DIR_TMP_COV := $(DIR_TMP_REP)cov/\n"
		    "DIR_TMP_DL := $(DIR_TMP)dl/\n"
		    "DIR_TMP_DL_PKG = $(DIR_TMP_DL)$(PKG_DIR)\n"
		    "DIR_PKG = $(DIR_PROJ)$(PKG_DIR)\n"
		    "DIR_PKG_SRC = $(DIR_PKG)src/\n"
		    "DIR_PKG_INC = $(DIR_PKG)include/\n"
		    "DIR_PKG_DRV = $(DIR_PKG)drivers/\n"
		    "DIR_PKG_TST = $(DIR_PKG)test/\n"
		    "DIR_OUT := $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT := $(DIR_OUT)int/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_DRV = $(DIR_OUT_INT)$(PN)/drivers/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_LIB := $(DIR_OUT)lib/\n"
		    "DIR_OUT_LIB_FILE = $(DIR_OUT_LIB)$(PN).a\n"
		    "DIR_OUT_BIN := $(DIR_OUT)bin/\n"
		    "DIR_OUT_BIN_FILE = $(DIR_OUT_BIN)$(PN)\n"
		    "DIR_OUT_EXT := $(DIR_OUT)ext/\n"
		    "DIR_OUT_EXT_PKG = $(DIR_OUT_EXT)$(PN)/\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TN)\n"
		    "DIR_OUT_TST := $(DIR_OUT)test/\n"
		    "DIR_OUT_TST_FILE = $(DIR_OUT_TST)$(PN)"
		    "\n"
		    "ifeq ($(ARCH),x64)\n"
		    "BITS := 64\n"
		    "endif\n"
		    "ifeq ($(ARCH),x86)\n"
		    "BITS := 32\n"
		    "endif\n"
		    "\n"
		    "ifeq ($(CONFIG),Debug)\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic -O0 -ggdb -coverage\n"
		    "LDFLAGS := -coverage\n"
		    "endif\n"
		    "ifeq ($(CONFIG),Release)\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "LDFLAGS :=\n"
		    "endif\n"
		    "\n"
		    "TCC := $(CC)\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(PKGDIR_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(PKGDIR_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(PKGDIR_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(PKGDIR_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(PKGDIR_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(PKGDIR_INC) -type f -name '*.h')\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "GCDA :=\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "precov:\n"
		    "\t@rm -fv $(GCDA)\n"
		    "\n"
		    "cov:\n"
		    "\t@if [ -n \"$(GCDA)\" ]; then \\\n"
		    "\t\tmkdir -pv $(COVDIR); \\\n"
		    "\t\tlcov -q -c -o $(COVDIR)lcov.info -d $(INTDIR); \\\n"
		    "\t\tgenhtml -q -o $(COVDIR) $(COVDIR)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(COVDIR)index.html || true; \\\n"
		    "\tfi\n"
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

	char buf[4096] = {0};
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
		    "OPEN := 1\n"
		    "\n"
		    "DLDIR := $(PROJDIR)tmp/dl/\n"
		    "EXTDIR := $(PROJDIR)tmp/ext/\n"
		    "REPDIR := $(PROJDIR)tmp/report/\n"
		    "COVDIR := $(REPDIR)cov/\n"
		    "\n"
		    "PKGDLDIR = $(DLDIR)$($(PN).DIR)\n"
		    "PKGEXTDIR = $(EXTDIR)$($(PN).DIR)\n"
		    "PKGDLFILE = $($(PN).DLFILE)\n"
		    "PKGDLROOT = $($(PN).DLROOT)\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int/\n"
		    "LIBDIR := $(OUTDIR)lib/\n"
		    "BINDIR := $(OUTDIR)bin/\n"
		    "EXTOUTDIR := $(OUTDIR)ext/\n"
		    "TSTDIR := $(OUTDIR)test/\n"
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
		    "PKGEXE = $(BINDIR)$(PN)\n"
		    "PKGLIB = $(LIBDIR)$(PN).a\n"
		    "PKGEXT = $(EXTOUTDIR)$(PN)/$(TN)\n"
		    "PKGTST = $(TSTDIR)$(PN)\n"
		    "\n"
		    "GCDA :=\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "precov:\n"
		    "\t@rm -fv $(GCDA)\n"
		    "\n"
		    "cov:\n"
		    "\t@if [ -n \"$(GCDA)\" ]; then \\\n"
		    "\t\tmkdir -pv $(COVDIR); \\\n"
		    "\t\tlcov -q -c -o $(COVDIR)lcov.info -d $(INTDIR); \\\n"
		    "\t\tgenhtml -q -o $(COVDIR) $(COVDIR)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(COVDIR)index.html || true; \\\n"
		    "\tfi\n"
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

	char buf[4096] = {0};
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
		    "OPEN := 1\n"
		    "\n"
		    "DLDIR := $(PROJDIR)tmp/dl/\n"
		    "EXTDIR := $(PROJDIR)tmp/ext/\n"
		    "REPDIR := $(PROJDIR)tmp/report/\n"
		    "COVDIR := $(REPDIR)cov/\n"
		    "\n"
		    "PKGDLDIR = $(DLDIR)$($(PN).DIR)\n"
		    "PKGEXTDIR = $(EXTDIR)$($(PN).DIR)\n"
		    "PKGDLFILE = $($(PN).DLFILE)\n"
		    "PKGDLROOT = $($(PN).DLROOT)\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int/\n"
		    "LIBDIR := $(OUTDIR)lib/\n"
		    "BINDIR := $(OUTDIR)bin/\n"
		    "EXTOUTDIR := $(OUTDIR)ext/\n"
		    "TSTDIR := $(OUTDIR)test/\n"
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
		    "PKGEXE = $(BINDIR)$(PN)\n"
		    "PKGLIB = $(LIBDIR)$(PN).a\n"
		    "PKGEXT = $(EXTOUTDIR)$(PN)/$(TN)\n"
		    "PKGTST = $(TSTDIR)$(PN)\n"
		    "\n"
		    "GCDA :=\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "precov:\n"
		    "\t@rm -fv $(GCDA)\n"
		    "\n"
		    "cov:\n"
		    "\t@if [ -n \"$(GCDA)\" ]; then \\\n"
		    "\t\tmkdir -pv $(COVDIR); \\\n"
		    "\t\tlcov -q -c -o $(COVDIR)lcov.info -d $(INTDIR); \\\n"
		    "\t\tgenhtml -q -o $(COVDIR) $(COVDIR)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(COVDIR)index.html || true; \\\n"
		    "\tfi\n"
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
		    "OPEN := 1\n"
		    "\n"
		    "DLDIR := $(PROJDIR)tmp/dl/\n"
		    "EXTDIR := $(PROJDIR)tmp/ext/\n"
		    "REPDIR := $(PROJDIR)tmp/report/\n"
		    "COVDIR := $(REPDIR)cov/\n"
		    "\n"
		    "PKGDLDIR = $(DLDIR)$($(PN).DIR)\n"
		    "PKGEXTDIR = $(EXTDIR)$($(PN).DIR)\n"
		    "PKGDLFILE = $($(PN).DLFILE)\n"
		    "PKGDLROOT = $($(PN).DLROOT)\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int/\n"
		    "LIBDIR := $(OUTDIR)lib/\n"
		    "BINDIR := $(OUTDIR)bin/\n"
		    "EXTOUTDIR := $(OUTDIR)ext/\n"
		    "TSTDIR := $(OUTDIR)test/\n"
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
		    "PKGEXE = $(BINDIR)$(PN)\n"
		    "PKGLIB = $(LIBDIR)$(PN).a\n"
		    "PKGEXT = $(EXTOUTDIR)$(PN)/$(TN)\n"
		    "PKGTST = $(TSTDIR)$(PN)\n"
		    "\n"
		    "GCDA :=\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define unknown\n"
		    "endef\n"
		    "\n"
		    "precov:\n"
		    "\t@rm -fv $(GCDA)\n"
		    "\n"
		    "cov:\n"
		    "\t@if [ -n \"$(GCDA)\" ]; then \\\n"
		    "\t\tmkdir -pv $(COVDIR); \\\n"
		    "\t\tlcov -q -c -o $(COVDIR)lcov.info -d $(INTDIR); \\\n"
		    "\t\tgenhtml -q -o $(COVDIR) $(COVDIR)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(COVDIR)index.html || true; \\\n"
		    "\tfi\n"
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

	char buf[4096] = {0};
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
		    "OPEN := 1\n"
		    "\n"
		    "DLDIR := $(PROJDIR)tmp/dl/\n"
		    "EXTDIR := $(PROJDIR)tmp/ext/\n"
		    "REPDIR := $(PROJDIR)tmp/report/\n"
		    "COVDIR := $(REPDIR)cov/\n"
		    "\n"
		    "PKGDLDIR = $(DLDIR)$($(PN).DIR)\n"
		    "PKGEXTDIR = $(EXTDIR)$($(PN).DIR)\n"
		    "PKGDLFILE = $($(PN).DLFILE)\n"
		    "PKGDLROOT = $($(PN).DLROOT)\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int/\n"
		    "LIBDIR := $(OUTDIR)lib/\n"
		    "BINDIR := $(OUTDIR)bin/\n"
		    "EXTOUTDIR := $(OUTDIR)ext/\n"
		    "TSTDIR := $(OUTDIR)test/\n"
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
		    "PKGEXE = $(BINDIR)$(PN)\n"
		    "PKGLIB = $(LIBDIR)$(PN).a\n"
		    "PKGEXT = $(EXTOUTDIR)$(PN)/$(TN)\n"
		    "PKGTST = $(TSTDIR)$(PN)\n"
		    "\n"
		    "GCDA :=\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define exe\n"
		    "$(PN).$(TN) := $(PKGEXE)\n"
		    "\n"
		    "GCDA += $(PKGSRC_GCDA)\n"
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
		    "precov:\n"
		    "\t@rm -fv $(GCDA)\n"
		    "\n"
		    "cov:\n"
		    "\t@if [ -n \"$(GCDA)\" ]; then \\\n"
		    "\t\tmkdir -pv $(COVDIR); \\\n"
		    "\t\tlcov -q -c -o $(COVDIR)lcov.info -d $(INTDIR); \\\n"
		    "\t\tgenhtml -q -o $(COVDIR) $(COVDIR)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(COVDIR)index.html || true; \\\n"
		    "\tfi\n"
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

	char buf[4096] = {0};
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
		    "OPEN := 1\n"
		    "\n"
		    "DLDIR := $(PROJDIR)tmp/dl/\n"
		    "EXTDIR := $(PROJDIR)tmp/ext/\n"
		    "REPDIR := $(PROJDIR)tmp/report/\n"
		    "COVDIR := $(REPDIR)cov/\n"
		    "\n"
		    "PKGDLDIR = $(DLDIR)$($(PN).DIR)\n"
		    "PKGEXTDIR = $(EXTDIR)$($(PN).DIR)\n"
		    "PKGDLFILE = $($(PN).DLFILE)\n"
		    "PKGDLROOT = $($(PN).DLROOT)\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int/\n"
		    "LIBDIR := $(OUTDIR)lib/\n"
		    "BINDIR := $(OUTDIR)bin/\n"
		    "EXTOUTDIR := $(OUTDIR)ext/\n"
		    "TSTDIR := $(OUTDIR)test/\n"
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
		    "PKGEXE = $(BINDIR)$(PN)\n"
		    "PKGLIB = $(LIBDIR)$(PN).a\n"
		    "PKGEXT = $(EXTOUTDIR)$(PN)/$(TN)\n"
		    "PKGTST = $(TSTDIR)$(PN)\n"
		    "\n"
		    "GCDA :=\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define lib\n"
		    "$(PN).$(TN) := $(PKGLIB)\n"
		    "\n"
		    "GCDA += $(PKGSRC_GCDA)\n"
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
		    "precov:\n"
		    "\t@rm -fv $(GCDA)\n"
		    "\n"
		    "cov:\n"
		    "\t@if [ -n \"$(GCDA)\" ]; then \\\n"
		    "\t\tmkdir -pv $(COVDIR); \\\n"
		    "\t\tlcov -q -c -o $(COVDIR)lcov.info -d $(INTDIR); \\\n"
		    "\t\tgenhtml -q -o $(COVDIR) $(COVDIR)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(COVDIR)index.html || true; \\\n"
		    "\tfi\n"
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

TEST(gen_make_proj_ext)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_ext(&com, STRV("M")), 0);

	char buf[4096] = {0};
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
		    "OPEN := 1\n"
		    "\n"
		    "DLDIR := $(PROJDIR)tmp/dl/\n"
		    "EXTDIR := $(PROJDIR)tmp/ext/\n"
		    "REPDIR := $(PROJDIR)tmp/report/\n"
		    "COVDIR := $(REPDIR)cov/\n"
		    "\n"
		    "PKGDLDIR = $(DLDIR)$($(PN).DIR)\n"
		    "PKGEXTDIR = $(EXTDIR)$($(PN).DIR)\n"
		    "PKGDLFILE = $($(PN).DLFILE)\n"
		    "PKGDLROOT = $($(PN).DLROOT)\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int/\n"
		    "LIBDIR := $(OUTDIR)lib/\n"
		    "BINDIR := $(OUTDIR)bin/\n"
		    "EXTOUTDIR := $(OUTDIR)ext/\n"
		    "TSTDIR := $(OUTDIR)test/\n"
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
		    "PKGEXE = $(BINDIR)$(PN)\n"
		    "PKGLIB = $(LIBDIR)$(PN).a\n"
		    "PKGEXT = $(EXTOUTDIR)$(PN)/$(TN)\n"
		    "PKGTST = $(TSTDIR)$(PN)\n"
		    "\n"
		    "GCDA :=\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define fetch_wget\n"
		    "$(PKGDLDIR)$(PKGDLFILE):\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\twget $($(PN).URI) -O $$@\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define ext_zip\n"
		    "$(PKGEXTDIR): $(PKGDLDIR)$(PKGDLFILE)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\tunzip $$< -d $$@\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define ext\n"
		    "$(PN).$(TN) := $(PKGEXT)\n"
		    "\n"
		    "all: $(PN).$(TN)/compile\n"
		    "\n"
		    ".PHONY: $(PN).$(TN)/compile\n"
		    "\n"
		    "$(PN).$(TN)/compile: $(PKGEXT)\n"
		    "\n"
		    "$(PKGEXT): $(PKGEXTDIR)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\tcd $(PKGEXTDIR)$(PKGDLROOT) && $($(PN).$(TN).CMD)\n"
		    "\tcp $(PKGEXTDIR)$(PKGDLROOT)$($(PN).$(TN).OUT) $(EXTOUTDIR)$(PN)\n"
		    "\ttouch $$@\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "precov:\n"
		    "\t@rm -fv $(GCDA)\n"
		    "\n"
		    "cov:\n"
		    "\t@if [ -n \"$(GCDA)\" ]; then \\\n"
		    "\t\tmkdir -pv $(COVDIR); \\\n"
		    "\t\tlcov -q -c -o $(COVDIR)lcov.info -d $(INTDIR); \\\n"
		    "\t\tgenhtml -q -o $(COVDIR) $(COVDIR)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(COVDIR)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "include $(BUILDDIR)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN).DIR :=\n"
		    "$(PN).URI := url\n"
		    "$(PN).DLFILE := \n"
		    "$(eval $(call fetch_wget))\n"
		    "$(eval $(call ext_zip))\n"
		    "TN := pkg\n"
		    "$(PN).$(TN).CMD :=\n"
		    "$(PN).$(TN).OUT :=\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_proj_test)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_test(&com, STRV("M")), 0);

	char buf[4096] = {0};
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
		    "OPEN := 1\n"
		    "\n"
		    "DLDIR := $(PROJDIR)tmp/dl/\n"
		    "EXTDIR := $(PROJDIR)tmp/ext/\n"
		    "REPDIR := $(PROJDIR)tmp/report/\n"
		    "COVDIR := $(REPDIR)cov/\n"
		    "\n"
		    "PKGDLDIR = $(DLDIR)$($(PN).DIR)\n"
		    "PKGEXTDIR = $(EXTDIR)$($(PN).DIR)\n"
		    "PKGDLFILE = $($(PN).DLFILE)\n"
		    "PKGDLROOT = $($(PN).DLROOT)\n"
		    "\n"
		    "OUTDIR := \n"
		    "INTDIR := $(OUTDIR)int/\n"
		    "LIBDIR := $(OUTDIR)lib/\n"
		    "BINDIR := $(OUTDIR)bin/\n"
		    "EXTOUTDIR := $(OUTDIR)ext/\n"
		    "TSTDIR := $(OUTDIR)test/\n"
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
		    "PKGEXE = $(BINDIR)$(PN)\n"
		    "PKGLIB = $(LIBDIR)$(PN).a\n"
		    "PKGEXT = $(EXTOUTDIR)$(PN)/$(TN)\n"
		    "PKGTST = $(TSTDIR)$(PN)\n"
		    "\n"
		    "GCDA :=\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define test\n"
		    "$(PN).$(TN) := $(PKGTST)\n"
		    "\n"
		    "GCDA += $(PKGTST_GCDA)\n"
		    "\n"
		    "all: $(PN).$(TN)/compile\n"
		    "\n"
		    "test: $(PN).$(TN)/test\n"
		    "\n"
		    "cov: $(PN).$(TN)/cov\n"
		    "\n"
		    ".PHONY: $(PN).$(TN)/compile $(PN).$(TN)/test $(PN).$(TN)/cov\n"
		    "\n"
		    "$(PN).$(TN)/compile: $(PKGTST)\n"
		    "\n"
		    "$(PN).$(TN)/test: $(PKGTST)\n"
		    "\t$(PKGTST)\n"
		    "\n"
		    "$(PN).$(TN)/cov: precov $(PKGTST)\n"
		    "\t$(PKGTST)\n"
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
		    "precov:\n"
		    "\t@rm -fv $(GCDA)\n"
		    "\n"
		    "cov:\n"
		    "\t@if [ -n \"$(GCDA)\" ]; then \\\n"
		    "\t\tmkdir -pv $(COVDIR); \\\n"
		    "\t\tlcov -q -c -o $(COVDIR)lcov.info -d $(INTDIR); \\\n"
		    "\t\tgenhtml -q -o $(COVDIR) $(COVDIR)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(COVDIR)index.html || true; \\\n"
		    "\tfi\n"
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
		    "$(PN).DIR := a" SEP "\n"
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
		    "$(PN).DIR := b" SEP "\n"
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
		    "$(PN).DIR := lib" SEP "\n"
		    "TN := lib\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := exe\n"
		    "$(PN).DIR := exe" SEP "\n"
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
		    "$(PN).DIR := base" SEP "\n"
		    "TN := base\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib1/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib1\n"
		    "$(PN).DIR := lib1" SEP "\n"
		    "TN := lib1\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib2/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib2\n"
		    "$(PN).DIR := lib2" SEP "\n"
		    "TN := lib2\n"
		    "$(PN).$(TN).HEADERS :=\n"
		    "$(PN).$(TN).INCLUDES :=\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := exe\n"
		    "$(PN).DIR := exe" SEP "\n"
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

TEST(gen_make_pkg_zip)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_zip(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN).DIR :=\n"
		    "$(PN).URI := url\n"
		    "$(PN).DLFILE := \n"
		    "$(PN).DLROOT := main\n"
		    "$(eval $(call fetch_wget))\n"
		    "$(eval $(call ext_zip))\n"
		    "TN := pkg\n"
		    "$(PN).$(TN).CMD := cmd\n"
		    "$(PN).$(TN).OUT := out\n"
		    "$(eval $(call ext))\n",
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
	RUN(gen_make_proj_ext);
	RUN(gen_make_proj_test);
	RUN(gen_make_pkg_exe);
	RUN(gen_make_pkg_lib);
	RUN(gen_make_pkg_lib_test);
	RUN(gen_make_pkg_multi);
	RUN(gen_make_pkg_depends);
	RUN(gen_make_pkg_rdepends);
	RUN(gen_make_pkg_zip);

	SEND;
}
