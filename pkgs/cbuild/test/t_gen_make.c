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
		    "ARCHS := host\n"
		    "CONFIGS := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "DIR_PROJ := .." SEP ".." SEP "\n"
		    "DIR_BUILD :=\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_DLFILE = $($(PN)_DLFILE)\n"
		    "PKG_DLROOT = $($(PN)_DLROOT)\n"
		    "TGT_CMD = $($(PN)_$(TN)_CMD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_DST = $($(PN)_$(TN)_DST)\n"
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
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_LIB_FILE = $(DIR_OUT_LIB)$(PN).a\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "DIR_OUT_BIN_FILE = $(DIR_OUT_BIN)$(PN)\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/\n"
		    "DIR_OUT_EXT_PKG = $(DIR_OUT_EXT)$(PN)/\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_DST)\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "DIR_OUT_TST_FILE = $(DIR_OUT_TST)$(PN)\n"
		    "\n"
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "TCC_host := $(CC)\n"
		    "FLAGS_host :=\n"
		    "TCC_x64 := $(CC)\n"
		    "FLAGS_x64 := -m64\n"
		    "TCC_x86 := $(CC)\n"
		    "FLAGS_x86 := -m32\n"
		    "\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "CFLAGS_Debug := -O0 -ggdb -coverage\n"
		    "CFLAGS_Release :=\n"
		    "LDFLAGS :=\n"
		    "LDFLAGS_Debug := -coverage\n"
		    "LDFLAGS_Release :=\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(DIR_PKG_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(DIR_PKG_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(DIR_PKG_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(DIR_PKG_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(DIR_PKG_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(DIR_PKG_INC) -type f -name '*.h')\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(DIR_PKG_SRC)%.c,$(DIR_OUT_INT_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(DIR_PKG_DRV)%.c,$(DIR_OUT_DRV_PKG)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(DIR_PKG_TST)%.c,$(DIR_OUT_INT_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "ARCH = $1\n"
		    "CONFIG = $2\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define _cov\n"
		    "precov_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(CONFIG))\n"
		    "\n"
		    "cov_$(CONFIG):\n"
		    "\t@if [ -n \"$$(GCDA_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT_INT); \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(eval $(call _cov,host,Debug))\n"
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
		    "ARCHS := host\n"
		    "CONFIGS := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_DLFILE = $($(PN)_DLFILE)\n"
		    "PKG_DLROOT = $($(PN)_DLROOT)\n"
		    "TGT_CMD = $($(PN)_$(TN)_CMD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_DST = $($(PN)_$(TN)_DST)\n"
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
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_LIB_FILE = $(DIR_OUT_LIB)$(PN).a\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "DIR_OUT_BIN_FILE = $(DIR_OUT_BIN)$(PN)\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/\n"
		    "DIR_OUT_EXT_PKG = $(DIR_OUT_EXT)$(PN)/\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_DST)\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "DIR_OUT_TST_FILE = $(DIR_OUT_TST)$(PN)\n"
		    "\n"
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "TCC_host := $(CC)\n"
		    "FLAGS_host :=\n"
		    "TCC_x64 := $(CC)\n"
		    "FLAGS_x64 := -m64\n"
		    "TCC_x86 := $(CC)\n"
		    "FLAGS_x86 := -m32\n"
		    "\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "CFLAGS_Debug := -O0 -ggdb -coverage\n"
		    "CFLAGS_Release :=\n"
		    "LDFLAGS :=\n"
		    "LDFLAGS_Debug := -coverage\n"
		    "LDFLAGS_Release :=\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(DIR_PKG_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(DIR_PKG_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(DIR_PKG_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(DIR_PKG_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(DIR_PKG_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(DIR_PKG_INC) -type f -name '*.h')\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(DIR_PKG_SRC)%.c,$(DIR_OUT_INT_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(DIR_PKG_DRV)%.c,$(DIR_OUT_DRV_PKG)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(DIR_PKG_TST)%.c,$(DIR_OUT_INT_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "ARCH = $1\n"
		    "CONFIG = $2\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define _cov\n"
		    "precov_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(CONFIG))\n"
		    "\n"
		    "cov_$(CONFIG):\n"
		    "\t@if [ -n \"$$(GCDA_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT_INT); \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(eval $(call _cov,host,Debug))\n"
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
		    "ARCHS := host\n"
		    "CONFIGS := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_DLFILE = $($(PN)_DLFILE)\n"
		    "PKG_DLROOT = $($(PN)_DLROOT)\n"
		    "TGT_CMD = $($(PN)_$(TN)_CMD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_DST = $($(PN)_$(TN)_DST)\n"
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
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_LIB_FILE = $(DIR_OUT_LIB)$(PN).a\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "DIR_OUT_BIN_FILE = $(DIR_OUT_BIN)$(PN)\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/\n"
		    "DIR_OUT_EXT_PKG = $(DIR_OUT_EXT)$(PN)/\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_DST)\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "DIR_OUT_TST_FILE = $(DIR_OUT_TST)$(PN)\n"
		    "\n"
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "TCC_host := $(CC)\n"
		    "FLAGS_host :=\n"
		    "TCC_x64 := $(CC)\n"
		    "FLAGS_x64 := -m64\n"
		    "TCC_x86 := $(CC)\n"
		    "FLAGS_x86 := -m32\n"
		    "\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "CFLAGS_Debug := -O0 -ggdb -coverage\n"
		    "CFLAGS_Release :=\n"
		    "LDFLAGS :=\n"
		    "LDFLAGS_Debug := -coverage\n"
		    "LDFLAGS_Release :=\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(DIR_PKG_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(DIR_PKG_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(DIR_PKG_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(DIR_PKG_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(DIR_PKG_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(DIR_PKG_INC) -type f -name '*.h')\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(DIR_PKG_SRC)%.c,$(DIR_OUT_INT_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(DIR_PKG_DRV)%.c,$(DIR_OUT_DRV_PKG)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(DIR_PKG_TST)%.c,$(DIR_OUT_INT_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "ARCH = $1\n"
		    "CONFIG = $2\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define _cov\n"
		    "precov_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(CONFIG))\n"
		    "\n"
		    "cov_$(CONFIG):\n"
		    "\t@if [ -n \"$$(GCDA_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT_INT); \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(eval $(call _cov,host,Debug))\n"
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

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "ARCHS := host\n"
		    "CONFIGS := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_DLFILE = $($(PN)_DLFILE)\n"
		    "PKG_DLROOT = $($(PN)_DLROOT)\n"
		    "TGT_CMD = $($(PN)_$(TN)_CMD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_DST = $($(PN)_$(TN)_DST)\n"
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
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_LIB_FILE = $(DIR_OUT_LIB)$(PN).a\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "DIR_OUT_BIN_FILE = $(DIR_OUT_BIN)$(PN)\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/\n"
		    "DIR_OUT_EXT_PKG = $(DIR_OUT_EXT)$(PN)/\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_DST)\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "DIR_OUT_TST_FILE = $(DIR_OUT_TST)$(PN)\n"
		    "\n"
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "TCC_host := $(CC)\n"
		    "FLAGS_host :=\n"
		    "TCC_x64 := $(CC)\n"
		    "FLAGS_x64 := -m64\n"
		    "TCC_x86 := $(CC)\n"
		    "FLAGS_x86 := -m32\n"
		    "\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "CFLAGS_Debug := -O0 -ggdb -coverage\n"
		    "CFLAGS_Release :=\n"
		    "LDFLAGS :=\n"
		    "LDFLAGS_Debug := -coverage\n"
		    "LDFLAGS_Release :=\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(DIR_PKG_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(DIR_PKG_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(DIR_PKG_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(DIR_PKG_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(DIR_PKG_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(DIR_PKG_INC) -type f -name '*.h')\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(DIR_PKG_SRC)%.c,$(DIR_OUT_INT_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(DIR_PKG_DRV)%.c,$(DIR_OUT_DRV_PKG)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(DIR_PKG_TST)%.c,$(DIR_OUT_INT_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "ARCH = $1\n"
		    "CONFIG = $2\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define unknown\n"
		    "endef\n"
		    "\n"
		    "define _cov\n"
		    "precov_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(CONFIG))\n"
		    "\n"
		    "cov_$(CONFIG):\n"
		    "\t@if [ -n \"$$(GCDA_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT_INT); \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(eval $(call _cov,host,Debug))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
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
		    "ARCHS := host\n"
		    "CONFIGS := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_DLFILE = $($(PN)_DLFILE)\n"
		    "PKG_DLROOT = $($(PN)_DLROOT)\n"
		    "TGT_CMD = $($(PN)_$(TN)_CMD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_DST = $($(PN)_$(TN)_DST)\n"
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
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_LIB_FILE = $(DIR_OUT_LIB)$(PN).a\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "DIR_OUT_BIN_FILE = $(DIR_OUT_BIN)$(PN)\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/\n"
		    "DIR_OUT_EXT_PKG = $(DIR_OUT_EXT)$(PN)/\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_DST)\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "DIR_OUT_TST_FILE = $(DIR_OUT_TST)$(PN)\n"
		    "\n"
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "TCC_host := $(CC)\n"
		    "FLAGS_host :=\n"
		    "TCC_x64 := $(CC)\n"
		    "FLAGS_x64 := -m64\n"
		    "TCC_x86 := $(CC)\n"
		    "FLAGS_x86 := -m32\n"
		    "\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "CFLAGS_Debug := -O0 -ggdb -coverage\n"
		    "CFLAGS_Release :=\n"
		    "LDFLAGS :=\n"
		    "LDFLAGS_Debug := -coverage\n"
		    "LDFLAGS_Release :=\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(DIR_PKG_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(DIR_PKG_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(DIR_PKG_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(DIR_PKG_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(DIR_PKG_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(DIR_PKG_INC) -type f -name '*.h')\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(DIR_PKG_SRC)%.c,$(DIR_OUT_INT_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(DIR_PKG_DRV)%.c,$(DIR_OUT_DRV_PKG)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(DIR_PKG_TST)%.c,$(DIR_OUT_INT_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "ARCH = $1\n"
		    "CONFIG = $2\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define _exe\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG) := $(DIR_OUT_BIN_FILE)\n"
		    "\n"
		    "GCDA_$(CONFIG) += $(PKGSRC_GCDA)\n"
		    "\n"
		    "all: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		    "\n"
		    ".PHONY: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		    "\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile: $(DIR_OUT_BIN_FILE)\n"
		    "\n"
		    "$(DIR_OUT_BIN_FILE): $($(PN)_$(TN)_DRIVERS:%=$(DIR_OUT_DRV)%) $(PKGSRC_OBJ) "
		    "$($(PN)_$(TN)_LIBS:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) $(LDFLAGS) $(LDFLAGS_$(CONFIG)) -o $$@ $(PKGSRC_OBJ) "
		    "$($(PN)_$(TN)_DRIVERS:%=$(DIR_OUT_DRV)%) "
		    "$($(PN)_$(TN)_LIBS:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		    "\n"
		    "$(DIR_OUT_INT_SRC)%.o: $(DIR_PKG_SRC)%.c $(PKGSRC_H) $($(PN)_$(TN)_HEADERS)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_SRC:%=-I%) $($(PN)_$(TN)_INCLUDES:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		    "-o $$@ $$<\n"
		    "\n"
		    "$(DIR_OUT_DRV_PKG)%.o: $(DIR_PKG_DRV)%.c $($(PN)_$(TN)_HEADERS)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_DRV:%=-I%) $($(PN)_$(TN)_INCLUDES:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		    "-o $$@ $$<\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define exe\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _exe,$(a),$(c)))))\n"
		    "endef\n"
		    "\n"
		    "define _cov\n"
		    "precov_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(CONFIG))\n"
		    "\n"
		    "cov_$(CONFIG):\n"
		    "\t@if [ -n \"$$(GCDA_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT_INT); \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(eval $(call _cov,host,Debug))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_LIBS :=\n"
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
		    "ARCHS := host\n"
		    "CONFIGS := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_DLFILE = $($(PN)_DLFILE)\n"
		    "PKG_DLROOT = $($(PN)_DLROOT)\n"
		    "TGT_CMD = $($(PN)_$(TN)_CMD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_DST = $($(PN)_$(TN)_DST)\n"
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
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_LIB_FILE = $(DIR_OUT_LIB)$(PN).a\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "DIR_OUT_BIN_FILE = $(DIR_OUT_BIN)$(PN)\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/\n"
		    "DIR_OUT_EXT_PKG = $(DIR_OUT_EXT)$(PN)/\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_DST)\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "DIR_OUT_TST_FILE = $(DIR_OUT_TST)$(PN)\n"
		    "\n"
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "TCC_host := $(CC)\n"
		    "FLAGS_host :=\n"
		    "TCC_x64 := $(CC)\n"
		    "FLAGS_x64 := -m64\n"
		    "TCC_x86 := $(CC)\n"
		    "FLAGS_x86 := -m32\n"
		    "\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "CFLAGS_Debug := -O0 -ggdb -coverage\n"
		    "CFLAGS_Release :=\n"
		    "LDFLAGS :=\n"
		    "LDFLAGS_Debug := -coverage\n"
		    "LDFLAGS_Release :=\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(DIR_PKG_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(DIR_PKG_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(DIR_PKG_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(DIR_PKG_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(DIR_PKG_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(DIR_PKG_INC) -type f -name '*.h')\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(DIR_PKG_SRC)%.c,$(DIR_OUT_INT_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(DIR_PKG_DRV)%.c,$(DIR_OUT_DRV_PKG)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(DIR_PKG_TST)%.c,$(DIR_OUT_INT_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "ARCH = $1\n"
		    "CONFIG = $2\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define _lib\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG) := $(DIR_OUT_LIB_FILE)\n"
		    "\n"
		    "GCDA_$(CONFIG) += $(PKGSRC_GCDA)\n"
		    "\n"
		    "all: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		    "\n"
		    ".PHONY: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		    "\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile: $(DIR_OUT_LIB_FILE)\n"
		    "\n"
		    "$(DIR_OUT_LIB_FILE): $(PKGSRC_OBJ)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\tar rcs $$@ $(PKGSRC_OBJ)\n"
		    "\n"
		    "$(DIR_OUT_INT_SRC)%.o: $(DIR_PKG_SRC)%.c $(PKGSRC_H) $($(PN)_$(TN)_HEADERS)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_SRC:%=-I%) $($(PN)_$(TN)_INCLUDES:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		    "-o $$@ $$<\n"
		    "\n"
		    "$(DIR_OUT_DRV_PKG)%.o: $(DIR_PKG_DRV)%.c $($(PN)_$(TN)_HEADERS)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_DRV:%=-I%) $($(PN)_$(TN)_INCLUDES:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		    "-o $$@ $$<\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define lib\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _lib,$(a),$(c)))))\n"
		    "endef\n"
		    "\n"
		    "define _cov\n"
		    "precov_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(CONFIG))\n"
		    "\n"
		    "cov_$(CONFIG):\n"
		    "\t@if [ -n \"$$(GCDA_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT_INT); \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(eval $(call _cov,host,Debug))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
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
		    "ARCHS := host\n"
		    "CONFIGS := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_DLFILE = $($(PN)_DLFILE)\n"
		    "PKG_DLROOT = $($(PN)_DLROOT)\n"
		    "TGT_CMD = $($(PN)_$(TN)_CMD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_DST = $($(PN)_$(TN)_DST)\n"
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
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_LIB_FILE = $(DIR_OUT_LIB)$(PN).a\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "DIR_OUT_BIN_FILE = $(DIR_OUT_BIN)$(PN)\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/\n"
		    "DIR_OUT_EXT_PKG = $(DIR_OUT_EXT)$(PN)/\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_DST)\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "DIR_OUT_TST_FILE = $(DIR_OUT_TST)$(PN)\n"
		    "\n"
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "TCC_host := $(CC)\n"
		    "FLAGS_host :=\n"
		    "TCC_x64 := $(CC)\n"
		    "FLAGS_x64 := -m64\n"
		    "TCC_x86 := $(CC)\n"
		    "FLAGS_x86 := -m32\n"
		    "\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "CFLAGS_Debug := -O0 -ggdb -coverage\n"
		    "CFLAGS_Release :=\n"
		    "LDFLAGS :=\n"
		    "LDFLAGS_Debug := -coverage\n"
		    "LDFLAGS_Release :=\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(DIR_PKG_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(DIR_PKG_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(DIR_PKG_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(DIR_PKG_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(DIR_PKG_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(DIR_PKG_INC) -type f -name '*.h')\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(DIR_PKG_SRC)%.c,$(DIR_OUT_INT_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(DIR_PKG_DRV)%.c,$(DIR_OUT_DRV_PKG)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(DIR_PKG_TST)%.c,$(DIR_OUT_INT_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "ARCH = $1\n"
		    "CONFIG = $2\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define fetch_wget\n"
		    "$(DIR_TMP_DL_PKG)$(PKG_DLFILE):\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\twget $(PKG_URI) -O $$@\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define ext_zip\n"
		    "$(DIR_TMP_EXT_PKG): $(DIR_TMP_DL_PKG)$(PKG_DLFILE)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\tunzip $$< -d $$@\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define _ext\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG) := $(DIR_OUT_EXT_FILE)\n"
		    "\n"
		    "all: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		    "\n"
		    ".PHONY: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		    "\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile: $(DIR_OUT_EXT_FILE)\n"
		    "\n"
		    "$(DIR_OUT_EXT_FILE): $(DIR_TMP_EXT_PKG)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\tcd $(DIR_TMP_EXT_PKG_ROOT) && $(TGT_CMD)\n"
		    "\tcp $(DIR_TMP_EXT_PKG_ROOT_OUT) $(DIR_OUT_EXT_FILE)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define ext\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _ext,$(a),$(c)))))\n"
		    "endef\n"
		    "\n"
		    "define _cov\n"
		    "precov_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(CONFIG))\n"
		    "\n"
		    "cov_$(CONFIG):\n"
		    "\t@if [ -n \"$$(GCDA_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT_INT); \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(eval $(call _cov,host,Debug))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "$(PN)_URI := url\n"
		    "$(PN)_DLFILE := \n"
		    "$(eval $(call fetch_wget))\n"
		    "$(eval $(call ext_zip))\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_CMD =\n"
		    "$(PN)_$(TN)_OUT =\n"
		    "$(PN)_$(TN)_DST =\n"
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
		    "ARCHS := host\n"
		    "CONFIGS := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_DLFILE = $($(PN)_DLFILE)\n"
		    "PKG_DLROOT = $($(PN)_DLROOT)\n"
		    "TGT_CMD = $($(PN)_$(TN)_CMD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_DST = $($(PN)_$(TN)_DST)\n"
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
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_LIB_FILE = $(DIR_OUT_LIB)$(PN).a\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "DIR_OUT_BIN_FILE = $(DIR_OUT_BIN)$(PN)\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/\n"
		    "DIR_OUT_EXT_PKG = $(DIR_OUT_EXT)$(PN)/\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_DST)\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "DIR_OUT_TST_FILE = $(DIR_OUT_TST)$(PN)\n"
		    "\n"
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "TCC_host := $(CC)\n"
		    "FLAGS_host :=\n"
		    "TCC_x64 := $(CC)\n"
		    "FLAGS_x64 := -m64\n"
		    "TCC_x86 := $(CC)\n"
		    "FLAGS_x86 := -m32\n"
		    "\n"
		    "CFLAGS := -Wall -Wextra -Werror -pedantic\n"
		    "CFLAGS_Debug := -O0 -ggdb -coverage\n"
		    "CFLAGS_Release :=\n"
		    "LDFLAGS :=\n"
		    "LDFLAGS_Debug := -coverage\n"
		    "LDFLAGS_Release :=\n"
		    "\n"
		    "PKGSRC_C = $(shell find $(DIR_PKG_SRC) -type f -name '*.c')\n"
		    "PKGSRC_H = $(shell find $(DIR_PKG_SRC) -type f -name '*.h')\n"
		    "PKGDRV_C = $(shell find $(DIR_PKG_DRV) -type f -name '*.c')\n"
		    "PKGTST_C = $(shell find $(DIR_PKG_TST) -type f -name '*.c')\n"
		    "PKGTST_H = $(shell find $(DIR_PKG_TST) -type f -name '*.h')\n"
		    "PKGINC_H = $(shell find $(DIR_PKG_INC) -type f -name '*.h')\n"
		    "\n"
		    "PKGSRC_OBJ = $(patsubst $(DIR_PKG_SRC)%.c,$(DIR_OUT_INT_SRC)%.o,$(PKGSRC_C))\n"
		    "PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))\n"
		    "PKGDRV_OBJ = $(patsubst $(DIR_PKG_DRV)%.c,$(DIR_OUT_DRV_PKG)%.o,$(PKGDRV_C))\n"
		    "PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))\n"
		    "PKGTST_OBJ = $(patsubst $(DIR_PKG_TST)%.c,$(DIR_OUT_INT_TST)%.o,$(PKGTST_C))\n"
		    "PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))\n"
		    "\n"
		    "ARCH = $1\n"
		    "CONFIG = $2\n"
		    "\n"
		    ".PHONY: all test cov\n"
		    "\n"
		    "all:\n"
		    "\n"
		    "define _test\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG) := $(DIR_OUT_TST_FILE)\n"
		    "\n"
		    "GCDA_$(CONFIG) += $(PKGTST_GCDA)\n"
		    "\n"
		    "all: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		    "\n"
		    "test: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/test\n"
		    "\n"
		    "cov_$(CONFIG): $(PN)_$(TN)_$(ARCH)_$(CONFIG)/cov\n"
		    "\n"
		    ".PHONY: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile $(PN)_$(TN)_$(ARCH)_$(CONFIG)/test $(PN)_$(TN)_$(ARCH)_$(CONFIG)/cov\n"
		    "\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile: $(DIR_OUT_TST_FILE)\n"
		    "\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG)/test: $(DIR_OUT_TST_FILE)\n"
		    "\t$(DIR_OUT_TST_FILE)\n"
		    "\n"
		    "$(PN)_$(TN)_$(ARCH)_$(CONFIG)/cov: precov_$(CONFIG) $(DIR_OUT_TST_FILE)\n"
		    "\t$(DIR_OUT_TST_FILE)\n"
		    "\n"
		    "$(DIR_OUT_TST_FILE): $($(PN)_$(TN)_DRIVERS:%=$(DIR_OUT_DRV)%) $(PKGTST_OBJ) "
		    "$($(PN)_$(TN)_LIBS:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) $(LDFLAGS) $(LDFLAGS_$(CONFIG)) -o $$@ $(PKGTST_OBJ) "
		    "$($(PN)_$(TN)_DRIVERS:%=$(DIR_OUT_DRV)%) "
		    "$($(PN)_$(TN)_LIBS:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		    "\n"
		    "$(DIR_OUT_INT_TST)%.o: $(DIR_PKG_TST)%.c $(PKGSRC_H) $($(PN)_$(TN)_HEADERS)\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_TST:%=-I%) $($(PN)_$(TN)_INCLUDES:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		    "-o $$@ $$<\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define test\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _test,$(a),$(c)))))\n"
		    "endef\n"
		    "\n"
		    "define _cov\n"
		    "precov_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(CONFIG))\n"
		    "\n"
		    "cov_$(CONFIG):\n"
		    "\t@if [ -n \"$$(GCDA_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT_INT); \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(eval $(call _cov,host,Debug))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_LIBS :=\n"
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
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_LIBS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_exe_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe_drv(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_LIBS :=\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_exe_drv_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe_drv_inc(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDES := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_LIBS :=\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
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
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_inc(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDES := $(DIR_PKG)include\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_drv(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_drv_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_drv_inc(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDES := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
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

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib\n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(eval $(call lib))\n"
		    "TN := test\n"
		    "$(PN)_$(TN)_LIBS := lib_lib\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_test_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_inc(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib\n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDES := $(DIR_PKG)include\n"
		    "$(eval $(call lib))\n"
		    "TN := test\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDES := $(DIR_PKG)include $(lib_lib_INCLUDE)\n"
		    "$(PN)_$(TN)_LIBS := lib_lib\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_test_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_drv(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib\n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
		    "$(eval $(call lib))\n"
		    "TN := test\n"
		    "$(PN)_$(TN)_LIBS := lib_lib\n"
		    "$(PN)_$(TN)_DRIVERS := $(lib_lib_DRIVERS)\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_test_drv_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_drv_inc(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib\n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDES := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
		    "$(eval $(call lib))\n"
		    "TN := test\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"		     // FIXME: not needed
		    "$(PN)_$(TN)_INCLUDES := $(DIR_PKG)include $(lib_lib_INCLUDE)\n" // FIXME: $(DIR_PKG)include is not needed
		    "$(PN)_$(TN)_LIBS := lib_lib\n"
		    "$(PN)_$(TN)_DRIVERS := $(lib_lib_DRIVERS)\n"
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
		    "$(PN)_DIR := a" SEP "\n"
		    "TN := a\n"
		    "$(PN)_$(TN)_LIBS :=\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./b/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := b\n"
		    "$(PN)_DIR := b" SEP "\n"
		    "TN := b\n"
		    "$(PN)_$(TN)_LIBS :=\n"
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

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("./lib/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib\n"
		    "$(PN)_DIR := lib" SEP "\n"
		    "TN := lib\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := exe\n"
		    "$(PN)_DIR := exe" SEP "\n"
		    "TN := exe\n"
		    "$(PN)_$(TN)_LIBS := lib_lib\n"
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

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("./base/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := base\n"
		    "$(PN)_DIR := base" SEP "\n"
		    "TN := base\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib1/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib1\n"
		    "$(PN)_DIR := lib1" SEP "\n"
		    "TN := lib1\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib2/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib2\n"
		    "$(PN)_DIR := lib2" SEP "\n"
		    "TN := lib2\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := exe\n"
		    "$(PN)_DIR := exe" SEP "\n"
		    "TN := exe\n"
		    "$(PN)_$(TN)_LIBS := lib1_lib1 lib2_lib2 base_base\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_unknown)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_unknown(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN)_DIR :=\n"
		    "$(PN)_URI := url\n"
		    "$(PN)_DLFILE := archive\n"
		    "$(PN)_DLROOT := main\n"
		    "$(eval $(call fetch_wget))\n"
		    "$(eval $(call ext_unknown))\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_CMD = cmd\n"
		    "$(PN)_$(TN)_OUT = out\n"
		    "$(PN)_$(TN)_DST = dst\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_zip)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_zip(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := pkg\n"
		    "$(PN)_DIR :=\n"
		    "$(PN)_URI := url\n"
		    "$(PN)_DLFILE := archive.zip\n"
		    "$(PN)_DLROOT := main\n"
		    "$(eval $(call fetch_wget))\n"
		    "$(eval $(call ext_zip))\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_CMD = cmd\n"
		    "$(PN)_$(TN)_OUT = out\n"
		    "$(PN)_$(TN)_DST = dst\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_deps)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_deps(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(eval $(call lib))\n"
		    "TN := ext\n"
		    "$(PN)_$(TN)_CMD =\n"
		    "$(PN)_$(TN)_OUT =\n"
		    "$(PN)_$(TN)_DST =\n"
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
	RUN(gen_make_pkg_exe_drv);
	RUN(gen_make_pkg_exe_drv_inc);
	RUN(gen_make_pkg_lib);
	RUN(gen_make_pkg_lib_inc);
	RUN(gen_make_pkg_lib_drv);
	RUN(gen_make_pkg_lib_drv_inc);
	RUN(gen_make_pkg_lib_test);
	RUN(gen_make_pkg_lib_test_inc);
	RUN(gen_make_pkg_lib_test_drv);
	RUN(gen_make_pkg_lib_test_drv_inc);
	RUN(gen_make_pkg_multi);
	RUN(gen_make_pkg_depends);
	RUN(gen_make_pkg_rdepends);
	RUN(gen_make_pkg_ext_unknown);
	RUN(gen_make_pkg_ext_zip);
	RUN(gen_make_pkg_ext_deps);

	SEND;
}
