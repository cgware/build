BUILDDIR := $(CURDIR)/
SRCDIR := $(CURDIR)/

TCC := $(CC)

ARCH := x64
ifeq ($(ARCH), x64)
BITS := 64
endif
ifeq ($(ARCH), x86)
BITS := 32
endif

CONFIG := Release
ifeq ($(CONFIG), Debug)
CFLAGS := -Wall -Wextra -Werror -pedantic -O0 -ggdb -coverage
LDFLAGS := -coverage
endif
ifeq ($(CONFIG), Release)
CFLAGS := -Wall -Wextra -Werror -pedantic
LDFLAGS :=
endif

OUTDIR := $(BUILDDIR)bin/$(ARCH)-$(CONFIG)
INTDIR := $(OUTDIR)/int
LIBSDIR := $(OUTDIR)/libs
TESTSDIR := $(OUTDIR)/tests
EXESDIR := $(OUTDIR)/exes

PKGDIR = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
PKGSRCDIR = $(PKGDIR)src/
PKGINCDIR = $(PKGDIR)include/
PKGDRVDIR = $(PKGDIR)drivers/
PKGTESTDIR = $(PKGDIR)test/

PKGSRC_C = $(shell find $(PKGSRCDIR) -type f -name '*.c')
PKGSRC_H = $(shell find $(PKGSRCDIR) -type f -name '*.h')
PKGDRV_C = $(shell find $(PKGDRVDIR) -type f -name '*.c')
PKGTEST_C = $(shell find $(PKGTESTDIR) -type f -name '*.c')
PKGTEST_H = $(shell find $(PKGTESTDIR) -type f -name '*.h')
PKGINC_H = $(shell find $(PKGINCDIR) -type f -name '*.h')

INTSRCDIR = $(INTDIR)/$(PKGNAME)/src/
INTDRVDIR = $(INTDIR)/$(PKGNAME)/drivers/
INTTESTDIR = $(INTDIR)/$(PKGNAME)/test/

PKGSRC_OBJ = $(patsubst $(PKGSRCDIR)%.c,$(INTSRCDIR)%.o,$(PKGSRC_C))
PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))
PKGDRV_OBJ = $(patsubst $(PKGDRVDIR)%.c,$(INTDRVDIR)%.o,$(PKGDRV_C))
PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))
PKGTEST_OBJ = $(patsubst $(PKGTESTDIR)%.c,$(INTTESTDIR)%.o,$(PKGTEST_C))
PKGTEST_GCDA = $(patsubst %.o,%.gcda,$(PKGTEST_OBJ))

PKGEXE = $(EXESDIR)/$(PKGNAME)
PKGLIB = $(LIBSDIR)/$(PKGNAME).a
PKGTEST = $(TESTSDIR)/$(PKGNAME)

$(SRCDIR)deps/cbase/cbase.mk:
ifeq ($(DEV), true)
	git clone git@github.com:cgware/cbase.git $(SRCDIR)deps/cbase
else
	git clone https://github.com/cgware/cbase.git $(SRCDIR)deps/cbase
endif

include $(SRCDIR)deps/cbase/cbase.mk

$(SRCDIR)deps/ctest/ctest.mk:
ifeq ($(DEV), true)
	git clone git@github.com:cgware/ctest.git $(SRCDIR)deps/ctest
else
	git clone https://github.com/cgware/ctest.git $(SRCDIR)deps/ctest
endif

include $(SRCDIR)deps/ctest/ctest.mk

$(SRCDIR)deps/cutils/cutils.mk:
ifeq ($(DEV), true)
	git clone git@github.com:cgware/cutils.git $(SRCDIR)deps/cutils
else
	git clone https://github.com/cgware/cutils.git $(SRCDIR)deps/cutils
endif

include $(SRCDIR)deps/cutils/cutils.mk

$(SRCDIR)deps/cparse/cparse.mk:
ifeq ($(DEV), true)
	git clone git@github.com:cgware/cparse.git $(SRCDIR)deps/cparse
else
	git clone https://github.com/cgware/cparse.git $(SRCDIR)deps/cparse
endif

include $(SRCDIR)deps/cparse/cparse.mk

include $(SRCDIR)pkgs/pkgs.mk

.PHONY: test
test: pkgs/test

.PHONY: coverage
coverage: test
	@lcov -q -c -o $(BUILDDIR)/bin/lcov.info -d $(INTDIR)/libbuild/src
ifeq ($(SHOW), true)
	@genhtml -q -o $(BUILDDIR)/report/coverage $(BUILDDIR)/bin/lcov.info 
	@open $(BUILDDIR)/report/coverage/index.html
endif