BUILDDIR := 
SRCDIR := 

TCC := $(CC)

ARCH := x64
ifeq ($(ARCH),x64)
BITS := 64
endif
ifeq ($(ARCH),x86)
BITS := 32
endif

CONFIG := Release
ifeq ($(CONFIG),Debug)
CFLAGS := -Wall -Wextra -Werror -pedantic -O0 -ggdb -coverage
LDFLAGS := -coverage
endif
ifeq ($(CONFIG),Release)
CFLAGS := -Wall -Wextra -Werror -pedantic
LDFLAGS := 
endif

OUTDIR := $(BUILDDIR)bin/$(ARCH)-$(CONFIG)/
INTDIR := $(OUTDIR)int
LIBSDIR := $(OUTDIR)libs
TESTSDIR := $(OUTDIR)tests
EXESDIR := $(OUTDIR)exes

PKGDIR = $(dir $(lastword $(MAKEFILE_LIST)))
PKGDIR_SRC = $(PKGDIR)src/
PKGDIR_INC = $(PKGDIR)include/
PKGDIR_DRV = $(PKGDIR)drivers/
PKGDIR_TST = $(PKGDIR)test/

PKGSRC_C = $(shell find $(PKGDIR_SRC) -type f -name '*.c')
PKGSRC_H = $(shell find $(PKGDIR_SRC) -type f -name '*.h')
PKGDRV_C = $(shell find $(PKGDIR_DRV) -type f -name '*.c')
PKGTST_C = $(shell find $(PKGDIR_TST) -type f -name '*.c')
PKGTST_H = $(shell find $(PKGDIR_TST) -type f -name '*.h')
PKGINC_H = $(shell find $(PKGDIR_INC) -type f -name '*.h')

INTDIR_SRC = $(INTDIR)/$(PKGNAME)/src/
INTDIR_DRV = $(INTDIR)/$(PKGNAME)/drivers/
INTDIR_TST = $(INTDIR)/$(PKGNAME)/test/

PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))
PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))
PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))
PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))
PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))
PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))

PKGEXE = $(EXESDIR)/$(PKGNAME)
PKGLIB = $(LIBSDIR)/$(PKGNAME).a
PKGTST = $(TESTSDIR)/$(PKGNAME)

define pkg/exe
PKGNAME := $(1)
$(1)_SRC := $$(PKGDIR_SRC)
$(1)_H := $$(PKGSRC_H)
$(1)_INTDIR := $$(INTDIR_SRC)
$(1)_OBJ := $$(PKGSRC_OBJ)
$(1)_GCDA := $$(PKGSRC_GCDA)
$(1) := $$(PKGEXE)

.PHONY: $(1)/compile

$(1)/compile: $$($(1))

$$($(1)): $$($(1)_DRIVERS) $$($(1)_OBJ) $$($(1)_LIBS)
	@mkdir -pv $$(@D)
	$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $$($(1)_OBJ) $$($(1)_DRIVERS) -L$(LIBSDIR) $$($$(notdir $$($(1)_LIBS)):%=-l:%)

$$($(1)_INTDIR)%.o: $$($(1)_SRC)%.c $$($(1)_H) $$($(1)_HEADERS)
	@mkdir -pv $$(@D)
	$(TCC) -m$(BITS) -c $$($(1)_SRC:%=-I%) $$($(1)_INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<

endef

include $(SRCDIR)pkg.mk

.PHONY: test coverage

test: exe/test

coverage: test
	lcov -q -c -o $(BUILDDIR)/bin/lcov.info -d $(INTDIR)/libbuild/src -d $(INTDIR)/libbuild/drivers

