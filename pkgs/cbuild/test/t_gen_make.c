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
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "CP := cp\n"
		    "DIR_PROJ := .." SEP ".." SEP "\n"
		    "DIR_BUILD :=\n"
		    "DIR_TMP := $(DIR_PROJ)tmp/\n"
		    "DIR_TMP_EXT := $(DIR_TMP)ext/\n"
		    "DIR_TMP_REP := $(DIR_TMP)report/\n"
		    "DIR_TMP_COV := $(DIR_TMP_REP)cov/\n"
		    "DIR_TMP_DL := $(DIR_TMP)dl/\n"
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "ABS_DIR_OUT_BIN = $(abspath $(DIR_OUT_BIN))/\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/$(PN)/\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_URI_FILE = $($(PN)_URI_FILE)\n"
		    "PKG_URI_NAME = $($(PN)_URI_NAME)\n"
		    "PKG_URI_ROOT = $($(PN)_URI_ROOT)\n"
		    "DIR_PKG = $(DIR_PROJ)$(PKG_DIR)\n"
		    "DIR_PKG_SRC = $(DIR_PKG)src/\n"
		    "DIR_PKG_INC = $(DIR_PKG)include/\n"
		    "DIR_PKG_DRV = $(DIR_PKG)drivers/\n"
		    "DIR_PKG_TST = $(DIR_PKG)test/\n"
		    "DIR_TMP_EXT_PKG = $(DIR_TMP_EXT)$(PKG_DIR)\n"
		    "DIR_TMP_EXT_PKG_SRC = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-src/\n"
		    "DIR_TMP_EXT_PKG_SRC_ROOT = $(DIR_TMP_EXT_PKG_SRC)$(PKG_URI_ROOT)\n"
		    "DIR_TMP_EXT_PKG_BUILD = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-build-$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "TGT_SRC = $($(PN)_$(TN)_SRC)\n"
		    "TGT_BUILD = $($(PN)_$(TN)_BUILD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_PREP = $($(PN)_$(TN)_PREP)\n"
		    "TGT_CONF = $($(PN)_$(TN)_CONF)\n"
		    "TGT_COMP = $($(PN)_$(TN)_COMP)\n"
		    "TGT_INST = $($(PN)_$(TN)_INST)\n"
		    "TGT_TGT = $($(PN)_$(TN)_TGT)\n"
		    "DIR_OUT_LIB_FILE = $(TGT_OUT)$(PN)$(EXT_LIB)\n"
		    "DIR_OUT_BIN_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_PKG = $(TGT_OUT)\n"
		    "DIR_OUT_TST_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_TGT)\n"
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
		    ".PHONY: precov_$(ARCH)_$(CONFIG) cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "precov_$(ARCH)_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(ARCH)_$(CONFIG))\n"
		    "\n"
		    "cov_$(ARCH)_$(CONFIG):\n"
		    "\t@if [ \"$(CONFIG)\" = \"Debug\" ] && [ -n \"$$(GCDA_$(ARCH)_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT) --exclude \"*/test/*\" --exclude \"*/tmp/*\"; \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _cov,$(a),$(c)))))\n"
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
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "CP := cp\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "DIR_TMP := $(DIR_PROJ)tmp/\n"
		    "DIR_TMP_EXT := $(DIR_TMP)ext/\n"
		    "DIR_TMP_REP := $(DIR_TMP)report/\n"
		    "DIR_TMP_COV := $(DIR_TMP_REP)cov/\n"
		    "DIR_TMP_DL := $(DIR_TMP)dl/\n"
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "ABS_DIR_OUT_BIN = $(abspath $(DIR_OUT_BIN))/\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/$(PN)/\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_URI_FILE = $($(PN)_URI_FILE)\n"
		    "PKG_URI_NAME = $($(PN)_URI_NAME)\n"
		    "PKG_URI_ROOT = $($(PN)_URI_ROOT)\n"
		    "DIR_PKG = $(DIR_PROJ)$(PKG_DIR)\n"
		    "DIR_PKG_SRC = $(DIR_PKG)src/\n"
		    "DIR_PKG_INC = $(DIR_PKG)include/\n"
		    "DIR_PKG_DRV = $(DIR_PKG)drivers/\n"
		    "DIR_PKG_TST = $(DIR_PKG)test/\n"
		    "DIR_TMP_EXT_PKG = $(DIR_TMP_EXT)$(PKG_DIR)\n"
		    "DIR_TMP_EXT_PKG_SRC = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-src/\n"
		    "DIR_TMP_EXT_PKG_SRC_ROOT = $(DIR_TMP_EXT_PKG_SRC)$(PKG_URI_ROOT)\n"
		    "DIR_TMP_EXT_PKG_BUILD = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-build-$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "TGT_SRC = $($(PN)_$(TN)_SRC)\n"
		    "TGT_BUILD = $($(PN)_$(TN)_BUILD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_PREP = $($(PN)_$(TN)_PREP)\n"
		    "TGT_CONF = $($(PN)_$(TN)_CONF)\n"
		    "TGT_COMP = $($(PN)_$(TN)_COMP)\n"
		    "TGT_INST = $($(PN)_$(TN)_INST)\n"
		    "TGT_TGT = $($(PN)_$(TN)_TGT)\n"
		    "DIR_OUT_LIB_FILE = $(TGT_OUT)$(PN)$(EXT_LIB)\n"
		    "DIR_OUT_BIN_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_PKG = $(TGT_OUT)\n"
		    "DIR_OUT_TST_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_TGT)\n"
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
		    ".PHONY: precov_$(ARCH)_$(CONFIG) cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "precov_$(ARCH)_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(ARCH)_$(CONFIG))\n"
		    "\n"
		    "cov_$(ARCH)_$(CONFIG):\n"
		    "\t@if [ \"$(CONFIG)\" = \"Debug\" ] && [ -n \"$$(GCDA_$(ARCH)_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT) --exclude \"*/test/*\" --exclude \"*/tmp/*\"; \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _cov,$(a),$(c)))))\n"
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
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "CP := cp\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "DIR_TMP := $(DIR_PROJ)tmp/\n"
		    "DIR_TMP_EXT := $(DIR_TMP)ext/\n"
		    "DIR_TMP_REP := $(DIR_TMP)report/\n"
		    "DIR_TMP_COV := $(DIR_TMP_REP)cov/\n"
		    "DIR_TMP_DL := $(DIR_TMP)dl/\n"
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "ABS_DIR_OUT_BIN = $(abspath $(DIR_OUT_BIN))/\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/$(PN)/\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_URI_FILE = $($(PN)_URI_FILE)\n"
		    "PKG_URI_NAME = $($(PN)_URI_NAME)\n"
		    "PKG_URI_ROOT = $($(PN)_URI_ROOT)\n"
		    "DIR_PKG = $(DIR_PROJ)$(PKG_DIR)\n"
		    "DIR_PKG_SRC = $(DIR_PKG)src/\n"
		    "DIR_PKG_INC = $(DIR_PKG)include/\n"
		    "DIR_PKG_DRV = $(DIR_PKG)drivers/\n"
		    "DIR_PKG_TST = $(DIR_PKG)test/\n"
		    "DIR_TMP_EXT_PKG = $(DIR_TMP_EXT)$(PKG_DIR)\n"
		    "DIR_TMP_EXT_PKG_SRC = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-src/\n"
		    "DIR_TMP_EXT_PKG_SRC_ROOT = $(DIR_TMP_EXT_PKG_SRC)$(PKG_URI_ROOT)\n"
		    "DIR_TMP_EXT_PKG_BUILD = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-build-$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "TGT_SRC = $($(PN)_$(TN)_SRC)\n"
		    "TGT_BUILD = $($(PN)_$(TN)_BUILD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_PREP = $($(PN)_$(TN)_PREP)\n"
		    "TGT_CONF = $($(PN)_$(TN)_CONF)\n"
		    "TGT_COMP = $($(PN)_$(TN)_COMP)\n"
		    "TGT_INST = $($(PN)_$(TN)_INST)\n"
		    "TGT_TGT = $($(PN)_$(TN)_TGT)\n"
		    "DIR_OUT_LIB_FILE = $(TGT_OUT)$(PN)$(EXT_LIB)\n"
		    "DIR_OUT_BIN_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_PKG = $(TGT_OUT)\n"
		    "DIR_OUT_TST_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_TGT)\n"
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
		    ".PHONY: precov_$(ARCH)_$(CONFIG) cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "precov_$(ARCH)_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(ARCH)_$(CONFIG))\n"
		    "\n"
		    "cov_$(ARCH)_$(CONFIG):\n"
		    "\t@if [ \"$(CONFIG)\" = \"Debug\" ] && [ -n \"$$(GCDA_$(ARCH)_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT) --exclude \"*/test/*\" --exclude \"*/tmp/*\"; \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _cov,$(a),$(c)))))\n"
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
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "CP := cp\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "DIR_TMP := $(DIR_PROJ)tmp/\n"
		    "DIR_TMP_EXT := $(DIR_TMP)ext/\n"
		    "DIR_TMP_REP := $(DIR_TMP)report/\n"
		    "DIR_TMP_COV := $(DIR_TMP_REP)cov/\n"
		    "DIR_TMP_DL := $(DIR_TMP)dl/\n"
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "ABS_DIR_OUT_BIN = $(abspath $(DIR_OUT_BIN))/\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/$(PN)/\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_URI_FILE = $($(PN)_URI_FILE)\n"
		    "PKG_URI_NAME = $($(PN)_URI_NAME)\n"
		    "PKG_URI_ROOT = $($(PN)_URI_ROOT)\n"
		    "DIR_PKG = $(DIR_PROJ)$(PKG_DIR)\n"
		    "DIR_PKG_SRC = $(DIR_PKG)src/\n"
		    "DIR_PKG_INC = $(DIR_PKG)include/\n"
		    "DIR_PKG_DRV = $(DIR_PKG)drivers/\n"
		    "DIR_PKG_TST = $(DIR_PKG)test/\n"
		    "DIR_TMP_EXT_PKG = $(DIR_TMP_EXT)$(PKG_DIR)\n"
		    "DIR_TMP_EXT_PKG_SRC = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-src/\n"
		    "DIR_TMP_EXT_PKG_SRC_ROOT = $(DIR_TMP_EXT_PKG_SRC)$(PKG_URI_ROOT)\n"
		    "DIR_TMP_EXT_PKG_BUILD = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-build-$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "TGT_SRC = $($(PN)_$(TN)_SRC)\n"
		    "TGT_BUILD = $($(PN)_$(TN)_BUILD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_PREP = $($(PN)_$(TN)_PREP)\n"
		    "TGT_CONF = $($(PN)_$(TN)_CONF)\n"
		    "TGT_COMP = $($(PN)_$(TN)_COMP)\n"
		    "TGT_INST = $($(PN)_$(TN)_INST)\n"
		    "TGT_TGT = $($(PN)_$(TN)_TGT)\n"
		    "DIR_OUT_LIB_FILE = $(TGT_OUT)$(PN)$(EXT_LIB)\n"
		    "DIR_OUT_BIN_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_PKG = $(TGT_OUT)\n"
		    "DIR_OUT_TST_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_TGT)\n"
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
		    ".PHONY: precov_$(ARCH)_$(CONFIG) cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "precov_$(ARCH)_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(ARCH)_$(CONFIG))\n"
		    "\n"
		    "cov_$(ARCH)_$(CONFIG):\n"
		    "\t@if [ \"$(CONFIG)\" = \"Debug\" ] && [ -n \"$$(GCDA_$(ARCH)_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT) --exclude \"*/test/*\" --exclude \"*/tmp/*\"; \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _cov,$(a),$(c)))))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_proj_exe)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_exe(&com, STRV("M")), 0);

	char buf[5120] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
	EXPECT_STRN(
		tmp.data,
		"ARCHS := host\n"
		"CONFIGS := Debug\n"
		"OPEN := 1\n"
		"\n"
		"EXT_LIB := .a\n"
		"EXT_EXE :=\n"
		"\n"
		"CP := cp\n"
		"DIR_PROJ :=\n"
		"DIR_BUILD :=\n"
		"DIR_TMP := $(DIR_PROJ)tmp/\n"
		"DIR_TMP_EXT := $(DIR_TMP)ext/\n"
		"DIR_TMP_REP := $(DIR_TMP)report/\n"
		"DIR_TMP_COV := $(DIR_TMP_REP)cov/\n"
		"DIR_TMP_DL := $(DIR_TMP)dl/\n"
		"DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		"DIR_OUT_INT = $(DIR_OUT)int/\n"
		"DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		"DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		"DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		"ABS_DIR_OUT_BIN = $(abspath $(DIR_OUT_BIN))/\n"
		"DIR_OUT_EXT = $(DIR_OUT)ext/$(PN)/\n"
		"DIR_OUT_TST = $(DIR_OUT)test/\n"
		"PKG_DIR = $($(PN)_DIR)\n"
		"PKG_URI = $($(PN)_URI)\n"
		"PKG_URI_FILE = $($(PN)_URI_FILE)\n"
		"PKG_URI_NAME = $($(PN)_URI_NAME)\n"
		"PKG_URI_ROOT = $($(PN)_URI_ROOT)\n"
		"DIR_PKG = $(DIR_PROJ)$(PKG_DIR)\n"
		"DIR_PKG_SRC = $(DIR_PKG)src/\n"
		"DIR_PKG_INC = $(DIR_PKG)include/\n"
		"DIR_PKG_DRV = $(DIR_PKG)drivers/\n"
		"DIR_PKG_TST = $(DIR_PKG)test/\n"
		"DIR_TMP_EXT_PKG = $(DIR_TMP_EXT)$(PKG_DIR)\n"
		"DIR_TMP_EXT_PKG_SRC = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-src/\n"
		"DIR_TMP_EXT_PKG_SRC_ROOT = $(DIR_TMP_EXT_PKG_SRC)$(PKG_URI_ROOT)\n"
		"DIR_TMP_EXT_PKG_BUILD = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-build-$(ARCH)-$(CONFIG)/\n"
		"DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		"DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		"DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		"TGT_SRC = $($(PN)_$(TN)_SRC)\n"
		"TGT_BUILD = $($(PN)_$(TN)_BUILD)\n"
		"TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		"TGT_PREP = $($(PN)_$(TN)_PREP)\n"
		"TGT_CONF = $($(PN)_$(TN)_CONF)\n"
		"TGT_COMP = $($(PN)_$(TN)_COMP)\n"
		"TGT_INST = $($(PN)_$(TN)_INST)\n"
		"TGT_TGT = $($(PN)_$(TN)_TGT)\n"
		"DIR_OUT_LIB_FILE = $(TGT_OUT)$(PN)$(EXT_LIB)\n"
		"DIR_OUT_BIN_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		"DIR_OUT_EXT_PKG = $(TGT_OUT)\n"
		"DIR_OUT_TST_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		"DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_TGT)\n"
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
		"GCDA_$(ARCH)_$(CONFIG) += $(PKGSRC_GCDA)\n"
		"\n"
		"all: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		"\n"
		".PHONY: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		"\n"
		"$(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile: $(DIR_OUT_BIN_FILE)\n"
		"\n"
		"$(DIR_OUT_BIN_FILE): $($(PN)_$(TN)_DRIVERS:%=$(DIR_OUT_DRV)%) $(PKGSRC_OBJ) "
		"$($(PN)_$(TN)_LIBS_PRIV:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		"\t@mkdir -pv $$(@D)\n"
		"\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) $(LDFLAGS) $(LDFLAGS_$(CONFIG)) -o $$@ $(PKGSRC_OBJ) "
		"$($(PN)_$(TN)_DRIVERS:%=$(DIR_OUT_DRV)%) "
		"$($(PN)_$(TN)_LIBS_PRIV:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		"\n"
		"$(DIR_OUT_INT_SRC)%.o: $(DIR_PKG_SRC)%.c $(PKGSRC_H) $($(PN)_$(TN)_HEADERS) "
		"$($(PN)_$(TN)_LIBS_PRIV:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		"\t@mkdir -pv $$(@D)\n"
		"\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_SRC:%=-I%) $($(PN)_$(TN)_INCLUDE_PRIV:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		"-o $$@ $$<\n"
		"\n"
		"$(DIR_OUT_DRV_PKG)%.o: $(DIR_PKG_DRV)%.c $($(PN)_$(TN)_HEADERS) $($(PN)_$(TN)_LIBS_PRIV:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		"\t@mkdir -pv $$(@D)\n"
		"\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_DRV:%=-I%) $($(PN)_$(TN)_INCLUDE_PRIV:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		"-o $$@ $$<\n"
		"\n"
		"endef\n"
		"\n"
		"define exe\n"
		"$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _exe,$(a),$(c)))))\n"
		"endef\n"
		"\n",
		3951);

	EXPECT_STRN(tmp.data + 3951,
		    "define _cov\n"
		    ".PHONY: precov_$(ARCH)_$(CONFIG) cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "precov_$(ARCH)_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(ARCH)_$(CONFIG))\n"
		    "\n"
		    "cov_$(ARCH)_$(CONFIG):\n"
		    "\t@if [ \"$(CONFIG)\" = \"Debug\" ] && [ -n \"$$(GCDA_$(ARCH)_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT) --exclude \"*/test/*\" --exclude \"*/tmp/*\"; \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _cov,$(a),$(c)))))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len - 3951);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
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

	char buf[5120] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
	EXPECT_STRN(
		tmp.data,
		"ARCHS := host\n"
		"CONFIGS := Debug\n"
		"OPEN := 1\n"
		"\n"
		"EXT_LIB := .a\n"
		"EXT_EXE :=\n"
		"\n"
		"CP := cp\n"
		"DIR_PROJ :=\n"
		"DIR_BUILD :=\n"
		"DIR_TMP := $(DIR_PROJ)tmp/\n"
		"DIR_TMP_EXT := $(DIR_TMP)ext/\n"
		"DIR_TMP_REP := $(DIR_TMP)report/\n"
		"DIR_TMP_COV := $(DIR_TMP_REP)cov/\n"
		"DIR_TMP_DL := $(DIR_TMP)dl/\n"
		"DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		"DIR_OUT_INT = $(DIR_OUT)int/\n"
		"DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		"DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		"DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		"ABS_DIR_OUT_BIN = $(abspath $(DIR_OUT_BIN))/\n"
		"DIR_OUT_EXT = $(DIR_OUT)ext/$(PN)/\n"
		"DIR_OUT_TST = $(DIR_OUT)test/\n"
		"PKG_DIR = $($(PN)_DIR)\n"
		"PKG_URI = $($(PN)_URI)\n"
		"PKG_URI_FILE = $($(PN)_URI_FILE)\n"
		"PKG_URI_NAME = $($(PN)_URI_NAME)\n"
		"PKG_URI_ROOT = $($(PN)_URI_ROOT)\n"
		"DIR_PKG = $(DIR_PROJ)$(PKG_DIR)\n"
		"DIR_PKG_SRC = $(DIR_PKG)src/\n"
		"DIR_PKG_INC = $(DIR_PKG)include/\n"
		"DIR_PKG_DRV = $(DIR_PKG)drivers/\n"
		"DIR_PKG_TST = $(DIR_PKG)test/\n"
		"DIR_TMP_EXT_PKG = $(DIR_TMP_EXT)$(PKG_DIR)\n"
		"DIR_TMP_EXT_PKG_SRC = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-src/\n"
		"DIR_TMP_EXT_PKG_SRC_ROOT = $(DIR_TMP_EXT_PKG_SRC)$(PKG_URI_ROOT)\n"
		"DIR_TMP_EXT_PKG_BUILD = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-build-$(ARCH)-$(CONFIG)/\n"
		"DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		"DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		"DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		"TGT_SRC = $($(PN)_$(TN)_SRC)\n"
		"TGT_BUILD = $($(PN)_$(TN)_BUILD)\n"
		"TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		"TGT_PREP = $($(PN)_$(TN)_PREP)\n"
		"TGT_CONF = $($(PN)_$(TN)_CONF)\n"
		"TGT_COMP = $($(PN)_$(TN)_COMP)\n"
		"TGT_INST = $($(PN)_$(TN)_INST)\n"
		"TGT_TGT = $($(PN)_$(TN)_TGT)\n"
		"DIR_OUT_LIB_FILE = $(TGT_OUT)$(PN)$(EXT_LIB)\n"
		"DIR_OUT_BIN_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		"DIR_OUT_EXT_PKG = $(TGT_OUT)\n"
		"DIR_OUT_TST_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		"DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_TGT)\n"
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
		"GCDA_$(ARCH)_$(CONFIG) += $(PKGSRC_GCDA)\n"
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
		"$(DIR_OUT_INT_SRC)%.o: $(DIR_PKG_SRC)%.c $(PKGSRC_H) $($(PN)_$(TN)_HEADERS) "
		"$($(PN)_$(TN)_LIBS:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		"\t@mkdir -pv $$(@D)\n"
		"\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_SRC:%=-I%) $($(PN)_$(TN)_INCLUDE_PRIV:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		"-o $$@ $$<\n"
		"\n"
		"$(DIR_OUT_DRV_PKG)%.o: $(DIR_PKG_DRV)%.c $($(PN)_$(TN)_HEADERS) $($(PN)_$(TN)_LIBS:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		"\t@mkdir -pv $$(@D)\n"
		"\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_DRV:%=-I%) $($(PN)_$(TN)_INCLUDE_PRIV:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		"-o $$@ $$<\n"
		"\n"
		"endef\n"
		"\n"
		"define lib\n"
		"$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _lib,$(a),$(c)))))\n"
		"endef\n"
		"\n",
		3697);

	EXPECT_STRN(tmp.data + 3697,
		    "define _cov\n"
		    ".PHONY: precov_$(ARCH)_$(CONFIG) cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "precov_$(ARCH)_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(ARCH)_$(CONFIG))\n"
		    "\n"
		    "cov_$(ARCH)_$(CONFIG):\n"
		    "\t@if [ \"$(CONFIG)\" = \"Debug\" ] && [ -n \"$$(GCDA_$(ARCH)_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT) --exclude \"*/test/*\" --exclude \"*/tmp/*\"; \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _cov,$(a),$(c)))))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len - 3697);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
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

	char buf[5120] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "ARCHS := host\n"
		    "CONFIGS := Debug\n"
		    "OPEN := 1\n"
		    "\n"
		    "EXT_LIB := .a\n"
		    "EXT_EXE :=\n"
		    "\n"
		    "CP := cp\n"
		    "DIR_PROJ :=\n"
		    "DIR_BUILD :=\n"
		    "DIR_TMP := $(DIR_PROJ)tmp/\n"
		    "DIR_TMP_EXT := $(DIR_TMP)ext/\n"
		    "DIR_TMP_REP := $(DIR_TMP)report/\n"
		    "DIR_TMP_COV := $(DIR_TMP_REP)cov/\n"
		    "DIR_TMP_DL := $(DIR_TMP)dl/\n"
		    "DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT = $(DIR_OUT)int/\n"
		    "DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		    "DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		    "DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		    "ABS_DIR_OUT_BIN = $(abspath $(DIR_OUT_BIN))/\n"
		    "DIR_OUT_EXT = $(DIR_OUT)ext/$(PN)/\n"
		    "DIR_OUT_TST = $(DIR_OUT)test/\n"
		    "PKG_DIR = $($(PN)_DIR)\n"
		    "PKG_URI = $($(PN)_URI)\n"
		    "PKG_URI_FILE = $($(PN)_URI_FILE)\n"
		    "PKG_URI_NAME = $($(PN)_URI_NAME)\n"
		    "PKG_URI_ROOT = $($(PN)_URI_ROOT)\n"
		    "DIR_PKG = $(DIR_PROJ)$(PKG_DIR)\n"
		    "DIR_PKG_SRC = $(DIR_PKG)src/\n"
		    "DIR_PKG_INC = $(DIR_PKG)include/\n"
		    "DIR_PKG_DRV = $(DIR_PKG)drivers/\n"
		    "DIR_PKG_TST = $(DIR_PKG)test/\n"
		    "DIR_TMP_EXT_PKG = $(DIR_TMP_EXT)$(PKG_DIR)\n"
		    "DIR_TMP_EXT_PKG_SRC = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-src/\n"
		    "DIR_TMP_EXT_PKG_SRC_ROOT = $(DIR_TMP_EXT_PKG_SRC)$(PKG_URI_ROOT)\n"
		    "DIR_TMP_EXT_PKG_BUILD = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-build-$(ARCH)-$(CONFIG)/\n"
		    "DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		    "DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		    "DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		    "TGT_SRC = $($(PN)_$(TN)_SRC)\n"
		    "TGT_BUILD = $($(PN)_$(TN)_BUILD)\n"
		    "TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		    "TGT_PREP = $($(PN)_$(TN)_PREP)\n"
		    "TGT_CONF = $($(PN)_$(TN)_CONF)\n"
		    "TGT_COMP = $($(PN)_$(TN)_COMP)\n"
		    "TGT_INST = $($(PN)_$(TN)_INST)\n"
		    "TGT_TGT = $($(PN)_$(TN)_TGT)\n"
		    "DIR_OUT_LIB_FILE = $(TGT_OUT)$(PN)$(EXT_LIB)\n"
		    "DIR_OUT_BIN_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_PKG = $(TGT_OUT)\n"
		    "DIR_OUT_TST_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		    "DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_TGT)\n"
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
		    "$(DIR_TMP_DL)$(PKG_URI_FILE):\n"
		    "\t@mkdir -pv $$(@D)\n"
		    "\twget $(PKG_URI) -O $$@\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define ext_zip\n"
		    "$(DIR_TMP_EXT_PKG_SRC): $(DIR_TMP_DL)$(PKG_URI_FILE)\n"
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
		    "$(DIR_OUT_EXT_FILE): $(DIR_TMP_EXT_PKG_SRC)\n"
		    "\t@mkdir -pv $$(@D) $(DIR_TMP_EXT_PKG_BUILD)\n"
		    "\tcd $(TGT_BUILD) && $(if $(strip $(TGT_PREP)),$(TGT_PREP),:)\n"
		    "\tcd $(TGT_BUILD) && $(if $(strip $(TGT_CONF)),$(TGT_CONF),:)\n"
		    "\tcd $(TGT_BUILD) && $(if $(strip $(TGT_COMP)),$(TGT_COMP),:)\n"
		    "\tcd $(TGT_BUILD) && $(if $(strip $(TGT_INST)),$(TGT_INST),:)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "define ext\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _ext,$(a),$(c)))))\n"
		    "endef\n"
		    "\n",
		    3583);

	EXPECT_STRN(tmp.data + 3583,
		    "define _cov\n"
		    ".PHONY: precov_$(ARCH)_$(CONFIG) cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "precov_$(ARCH)_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(ARCH)_$(CONFIG))\n"
		    "\n"
		    "cov_$(ARCH)_$(CONFIG):\n"
		    "\t@if [ \"$(CONFIG)\" = \"Debug\" ] && [ -n \"$$(GCDA_$(ARCH)_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT) --exclude \"*/test/*\" --exclude \"*/tmp/*\"; \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _cov,$(a),$(c)))))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len - 3583);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "$(PN)_URI := url\n"
		    "$(PN)_URI_FILE := \n"
		    "$(PN)_URI_NAME := \n"
		    "$(eval $(call fetch_wget))\n"
		    "$(eval $(call ext_zip))\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
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

	char buf[5120] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("Makefile"), 0, &tmp);
	EXPECT_STRN(
		tmp.data,
		"ARCHS := host\n"
		"CONFIGS := Debug\n"
		"OPEN := 1\n"
		"\n"
		"EXT_LIB := .a\n"
		"EXT_EXE :=\n"
		"\n"
		"CP := cp\n"
		"DIR_PROJ :=\n"
		"DIR_BUILD :=\n"
		"DIR_TMP := $(DIR_PROJ)tmp/\n"
		"DIR_TMP_EXT := $(DIR_TMP)ext/\n"
		"DIR_TMP_REP := $(DIR_TMP)report/\n"
		"DIR_TMP_COV := $(DIR_TMP_REP)cov/\n"
		"DIR_TMP_DL := $(DIR_TMP)dl/\n"
		"DIR_OUT = $(DIR_PROJ)bin/$(ARCH)-$(CONFIG)/\n"
		"DIR_OUT_INT = $(DIR_OUT)int/\n"
		"DIR_OUT_LIB = $(DIR_OUT)lib/\n"
		"DIR_OUT_DRV = $(DIR_OUT)drivers/\n"
		"DIR_OUT_BIN = $(DIR_OUT)bin/\n"
		"ABS_DIR_OUT_BIN = $(abspath $(DIR_OUT_BIN))/\n"
		"DIR_OUT_EXT = $(DIR_OUT)ext/$(PN)/\n"
		"DIR_OUT_TST = $(DIR_OUT)test/\n"
		"PKG_DIR = $($(PN)_DIR)\n"
		"PKG_URI = $($(PN)_URI)\n"
		"PKG_URI_FILE = $($(PN)_URI_FILE)\n"
		"PKG_URI_NAME = $($(PN)_URI_NAME)\n"
		"PKG_URI_ROOT = $($(PN)_URI_ROOT)\n"
		"DIR_PKG = $(DIR_PROJ)$(PKG_DIR)\n"
		"DIR_PKG_SRC = $(DIR_PKG)src/\n"
		"DIR_PKG_INC = $(DIR_PKG)include/\n"
		"DIR_PKG_DRV = $(DIR_PKG)drivers/\n"
		"DIR_PKG_TST = $(DIR_PKG)test/\n"
		"DIR_TMP_EXT_PKG = $(DIR_TMP_EXT)$(PKG_DIR)\n"
		"DIR_TMP_EXT_PKG_SRC = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-src/\n"
		"DIR_TMP_EXT_PKG_SRC_ROOT = $(DIR_TMP_EXT_PKG_SRC)$(PKG_URI_ROOT)\n"
		"DIR_TMP_EXT_PKG_BUILD = $(DIR_TMP_EXT_PKG)$(PKG_URI_NAME)-build-$(ARCH)-$(CONFIG)/\n"
		"DIR_OUT_INT_SRC = $(DIR_OUT_INT)$(PN)/src/\n"
		"DIR_OUT_INT_TST = $(DIR_OUT_INT)$(PN)/test/\n"
		"DIR_OUT_DRV_PKG = $(DIR_OUT_DRV)$(PN)/\n"
		"TGT_SRC = $($(PN)_$(TN)_SRC)\n"
		"TGT_BUILD = $($(PN)_$(TN)_BUILD)\n"
		"TGT_OUT = $($(PN)_$(TN)_OUT)\n"
		"TGT_PREP = $($(PN)_$(TN)_PREP)\n"
		"TGT_CONF = $($(PN)_$(TN)_CONF)\n"
		"TGT_COMP = $($(PN)_$(TN)_COMP)\n"
		"TGT_INST = $($(PN)_$(TN)_INST)\n"
		"TGT_TGT = $($(PN)_$(TN)_TGT)\n"
		"DIR_OUT_LIB_FILE = $(TGT_OUT)$(PN)$(EXT_LIB)\n"
		"DIR_OUT_BIN_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		"DIR_OUT_EXT_PKG = $(TGT_OUT)\n"
		"DIR_OUT_TST_FILE = $(TGT_OUT)$(PN)$(EXT_EXE)\n"
		"DIR_OUT_EXT_FILE = $(DIR_OUT_EXT_PKG)$(TGT_TGT)\n"
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
		"GCDA_$(ARCH)_$(CONFIG) += $(PKGTST_GCDA)\n"
		"\n"
		"all: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile\n"
		"\n"
		"test: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/test\n"
		"\n"
		"cov_$(ARCH)_$(CONFIG): $(PN)_$(TN)_$(ARCH)_$(CONFIG)/cov\n"
		"\n"
		".PHONY: $(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile $(PN)_$(TN)_$(ARCH)_$(CONFIG)/test $(PN)_$(TN)_$(ARCH)_$(CONFIG)/cov\n"
		"\n"
		"$(PN)_$(TN)_$(ARCH)_$(CONFIG)/compile: $(DIR_OUT_TST_FILE)\n"
		"\n"
		"$(PN)_$(TN)_$(ARCH)_$(CONFIG)/test: $(DIR_OUT_TST_FILE)\n"
		"\n"
		"$(PN)_$(TN)_$(ARCH)_$(CONFIG)/cov: $(DIR_OUT_TST_FILE)\n"
		"\n"
		"$(DIR_OUT_TST_FILE): $($(PN)_$(TN)_DRIVERS:%=$(DIR_OUT_DRV)%) $(PKGTST_OBJ) "
		"$($(PN)_$(TN)_LIBS_PRIV:%=$$(%_$(ARCH)_$(CONFIG))) | precov_$(ARCH)_$(CONFIG)\n"
		"\t@mkdir -pv $$(@D)\n"
		"\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) $(LDFLAGS) $(LDFLAGS_$(CONFIG)) -o $$@ $(PKGTST_OBJ) "
		"$($(PN)_$(TN)_DRIVERS:%=$(DIR_OUT_DRV)%) "
		"$($(PN)_$(TN)_LIBS_PRIV:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		"\t$(DIR_OUT_TST_FILE)\n"
		"\n"
		"$(DIR_OUT_INT_TST)%.o: $(DIR_PKG_TST)%.c $(PKGSRC_H) $($(PN)_$(TN)_HEADERS) "
		"$($(PN)_$(TN)_LIBS_PRIV:%=$$(%_$(ARCH)_$(CONFIG)))\n"
		"\t@mkdir -pv $$(@D)\n"
		"\t$(TCC_$(ARCH)) $(FLAGS_$(ARCH)) -c $(DIR_PKG_TST:%=-I%) $($(PN)_$(TN)_INCLUDE_PRIV:%=-I%) $(CFLAGS) $(CFLAGS_$(CONFIG)) "
		"-o $$@ $$<\n"
		"\n"
		"endef\n"
		"\n"
		"define test\n"
		"$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _test,$(a),$(c)))))\n"
		"endef\n"
		"\n",
		4017);

	EXPECT_STRN(tmp.data + 4017,
		    "define _cov\n"
		    ".PHONY: precov_$(ARCH)_$(CONFIG) cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "precov_$(ARCH)_$(CONFIG):\n"
		    "\t@rm -fv $$(GCDA_$(ARCH)_$(CONFIG))\n"
		    "\n"
		    "cov_$(ARCH)_$(CONFIG):\n"
		    "\t@if [ \"$(CONFIG)\" = \"Debug\" ] && [ -n \"$$(GCDA_$(ARCH)_$(CONFIG))\" ]; then \\\n"
		    "\t\tmkdir -pv $(DIR_TMP_COV); \\\n"
		    "\t\tlcov -q -c -o $(DIR_TMP_COV)lcov.info -d $(DIR_OUT) --exclude \"*/test/*\" --exclude \"*/tmp/*\"; \\\n"
		    "\t\tgenhtml -q -o $(DIR_TMP_COV) $(DIR_TMP_COV)lcov.info; \\\n"
		    "\t\t[ \"$(OPEN)\" = \"1\" ] && open $(DIR_TMP_COV)index.html || true; \\\n"
		    "\tfi\n"
		    "\n"
		    "cov: cov_$(ARCH)_$(CONFIG)\n"
		    "\n"
		    "endef\n"
		    "\n"
		    "$(foreach a,$(ARCHS),$(foreach c,$(CONFIGS),$(eval $(call _cov,$(a),$(c)))))\n"
		    "\n"
		    "include $(DIR_BUILD)pkg.mk\n"
		    "\n",
		    tmp.len - 4017);

	fs_read(&com.fs, STRV("pkg.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := pkg\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_TST)\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg(&com, STRV("M")), 0);

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

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_exe_out)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe_out(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(abspath exes)/\n"
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

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
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

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE_PRIV := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
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

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_out)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_out(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(abspath libs)/\n"
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

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDE_PRIV := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
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

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
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

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDE_PRIV := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_test)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_test(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_TST)\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_test_out)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_test_out(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(abspath tests)/\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_test_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_test_drv(&com, STRV("M")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_TST)\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
		    "$(eval $(call test))\n",
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

	fs_read(&com.fs, STRV("lib.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("test.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := test\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_TST)\n"
		    "$(PN)_$(TN)_LIBS_PRIV := _lib\n"
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

	fs_read(&com.fs, STRV("lib.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDE_PRIV := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("test.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := test\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE_PRIV := $(_lib_INCLUDE)\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_TST)\n"
		    "$(PN)_$(TN)_LIBS_PRIV := _lib\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_test_inc_src)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_inc_src(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("lib.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDE_PRIV := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("test.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := test\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE_PRIV := $(DIR_PKG)src $(_lib_INCLUDE)\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_TST)\n"
		    "$(PN)_$(TN)_LIBS_PRIV := _lib\n"
		    "$(eval $(call test))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_lib_exe_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_exe_drv(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("lib.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("exe.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := exe\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
		    "$(PN)_$(TN)_LIBS_PRIV := _lib\n"
		    "$(PN)_$(TN)_DRIVERS := $(_lib_DRIVERS)\n"
		    "$(eval $(call exe))\n",
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

	fs_read(&com.fs, STRV("lib.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("test.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := test\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_TST)\n"
		    "$(PN)_$(TN)_LIBS_PRIV := _lib\n"
		    "$(PN)_$(TN)_DRIVERS := $(_lib_DRIVERS)\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("lib.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_INCLUDE_PRIV := $(DIR_PKG)include\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(PN)_$(TN)_DRIVERS := $(PKGDRV_C:$(DIR_PKG_DRV)%.c=$(PN)/%.o)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("test.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := test\n"
		    "$(PN)_$(TN)_HEADERS := $(PKGINC_H)\n"
		    "$(PN)_$(TN)_INCLUDE_PRIV := $(_lib_INCLUDE)\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_TST)\n"
		    "$(PN)_$(TN)_LIBS_PRIV := _lib\n"
		    "$(PN)_$(TN)_DRIVERS := $(_lib_DRIVERS)\n"
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

	fs_read(&com.fs, STRV("./a/.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := a\n"
		    "$(PN)_DIR := a" SEP "\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./b/.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := b\n"
		    "$(PN)_DIR := b" SEP "\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
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

	fs_read(&com.fs, STRV("./lib/.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib\n"
		    "$(PN)_DIR := lib" SEP "\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := exe\n"
		    "$(PN)_DIR := exe" SEP "\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
		    "$(PN)_$(TN)_LIBS_PRIV := lib_\n"
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

	fs_read(&com.fs, STRV("./base/.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := base\n"
		    "$(PN)_DIR := base" SEP "\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib1/.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib1\n"
		    "$(PN)_DIR := lib1" SEP "\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(PN)_$(TN)_LIBS := base_\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib2/.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := lib2\n"
		    "$(PN)_DIR := lib2" SEP "\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(PN)_$(TN)_LIBS := base_\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := exe\n"
		    "$(PN)_DIR := exe" SEP "\n"
		    "TN := \n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
		    "$(PN)_$(TN)_LIBS_PRIV := lib1_ lib2_ base_\n"
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

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_uri)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_uri(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "$(PN)_URI := url\n"
		    "$(PN)_URI_FILE := file\n"
		    "$(PN)_URI_NAME := name\n"
		    "$(PN)_URI_ROOT := main\n"
		    "TN := \n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_cmd)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_cmd(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
		    "$(PN)_$(TN)_PREP = prep\n"
		    "$(PN)_$(TN)_CONF = conf\n"
		    "$(PN)_$(TN)_COMP = comp\n"
		    "$(PN)_$(TN)_INST = inst\n"
		    "$(PN)_$(TN)_TGT = out\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_out)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_out(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath exts)/\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_inc(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_INCLUDE := $(DIR_TMP_EXT_PKG_SRC_ROOT)include\n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_lib)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_lib(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_exe)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_exe(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := \n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
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

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "$(PN)_URI := url\n"
		    "$(PN)_URI_FILE := file\n"
		    "$(PN)_URI_NAME := name-1.0\n"
		    "$(PN)_URI_ROOT := main\n"
		    "$(eval $(call fetch_wget))\n"
		    "$(eval $(call ext_zip))\n"
		    "TN := \n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_tar)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_tar(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV(".mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "$(PN)_URI := url\n"
		    "$(PN)_URI_FILE := file\n"
		    "$(PN)_URI_NAME := name-1.0\n"
		    "$(PN)_URI_ROOT := main\n"
		    "$(eval $(call fetch_wget))\n"
		    "$(eval $(call ext_tar))\n"
		    "TN := \n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_dep_lib)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_dep_lib(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("lib.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := lib\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_LIB)\n"
		    "$(eval $(call lib))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("ext.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := ext\n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
		    "$(eval $(call ext))\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_make_pkg_ext_dep_exe)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_dep_exe(&com, STRV("M")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("exe.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := exe\n"
		    "$(PN)_$(TN)_OUT = $(DIR_OUT_BIN)\n"
		    "$(eval $(call exe))\n",
		    tmp.len);

	fs_read(&com.fs, STRV("ext.mk"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "PN := \n"
		    "$(PN)_DIR :=\n"
		    "TN := ext\n"
		    "$(PN)_$(TN)_SRC = $(abspath $(DIR_TMP_EXT_PKG_SRC_ROOT))/\n"
		    "$(PN)_$(TN)_BUILD = $(abspath $(DIR_TMP_EXT_PKG_BUILD))/\n"
		    "$(PN)_$(TN)_OUT = $(abspath $(DIR_OUT_EXT))/\n"
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
	RUN(gen_make_pkg);
	RUN(gen_make_pkg_exe);
	RUN(gen_make_pkg_exe_out);
	RUN(gen_make_pkg_exe_drv);
	RUN(gen_make_pkg_exe_drv_inc);
	RUN(gen_make_pkg_lib);
	RUN(gen_make_pkg_lib_out);
	RUN(gen_make_pkg_lib_inc);
	RUN(gen_make_pkg_lib_drv);
	RUN(gen_make_pkg_lib_drv_inc);
	RUN(gen_make_pkg_test);
	RUN(gen_make_pkg_test_out);
	RUN(gen_make_pkg_test_drv);
	RUN(gen_make_pkg_lib_test);
	RUN(gen_make_pkg_lib_test_inc);
	RUN(gen_make_pkg_lib_test_inc_src);
	RUN(gen_make_pkg_lib_exe_drv);
	RUN(gen_make_pkg_lib_test_drv);
	RUN(gen_make_pkg_lib_test_drv_inc);
	RUN(gen_make_pkg_multi);
	RUN(gen_make_pkg_depends);
	RUN(gen_make_pkg_rdepends);
	RUN(gen_make_pkg_ext_unknown);
	RUN(gen_make_pkg_ext_uri);
	RUN(gen_make_pkg_ext_cmd);
	RUN(gen_make_pkg_ext_out);
	RUN(gen_make_pkg_ext_inc);
	RUN(gen_make_pkg_ext_lib);
	RUN(gen_make_pkg_ext_exe);
	RUN(gen_make_pkg_ext_zip);
	RUN(gen_make_pkg_ext_tar);
	RUN(gen_make_pkg_ext_dep_lib);
	RUN(gen_make_pkg_ext_dep_exe);

	SEND;
}
