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
CFLAGS := -Wall -Wextra -pedantic -O0 -ggdb -coverage
LDFLAGS := -coverage
endif
ifeq ($(CONFIG), Release)
CFLAGS := -Wall -Wextra -pedantic
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

include $(SRCDIR)package/pkg.mk

.PHONY: test
test: pkgs/test

.PHONY: coverage
coverage: test
	lcov -q -c -o $(BUILDDIR)/bin/lcov.info -d $(INTDIR)/libbuild/src -d $(INTDIR)/libbuild/drivers
ifeq ($(SHOW), true)
	genhtml -q -o $(BUILDDIR)/report/coverage $(BUILDDIR)/bin/lcov.info 
	open $(BUILDDIR)/report/coverage/index.html
endif