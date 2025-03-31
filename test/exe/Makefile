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
LIBDIR := $(OUTDIR)lib
BINDIR := $(OUTDIR)bin
TSTDIR := $(OUTDIR)test

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

INTDIR_SRC = $(INTDIR)/$(PKG)/src/
INTDIR_DRV = $(INTDIR)/$(PKG)/drivers/
INTDIR_TST = $(INTDIR)/$(PKG)/test/

PKGSRC_OBJ = $(patsubst $(PKGDIR_SRC)%.c,$(INTDIR_SRC)%.o,$(PKGSRC_C))
PKGSRC_GCDA = $(patsubst %.o,%.gcda,$(PKGSRC_OBJ))
PKGDRV_OBJ = $(patsubst $(PKGDIR_DRV)%.c,$(INTDIR_DRV)%.o,$(PKGDRV_C))
PKGDRV_GCDA = $(patsubst %.o,%.gcda,$(PKGDRV_OBJ))
PKGTST_OBJ = $(patsubst $(PKGDIR_TST)%.c,$(INTDIR_TST)%.o,$(PKGTST_C))
PKGTST_GCDA = $(patsubst %.o,%.gcda,$(PKGTST_OBJ))

PKGEXE = $(BINDIR)/$(PKG)
PKGLIB = $(LIBDIR)/$(PKG).a
PKGTST = $(TSTDIR)/$(PKG)

define pkg/exe
.PHONY: $(PKG)/compile

$(PKG)/compile: $(PKGEXE)

$(PKGEXE): $($(PKG)_DRIVERS) $(PKGSRC_OBJ) $($(PKG)_LIBS)
	@mkdir -pv $$(@D)
	$(TCC) -m$(BITS) $(LDFLAGS) -o $$@ $(PKGSRC_OBJ) $($(PKG)_DRIVERS) -L$(LIBDIR) $$($$(notdir $($(PKG)_LIBS)):%=-l:%)

$(INTDIR_SRC)%.o: $(PKGDIR_SRC)%.c $(PKGSRC_H) $($(PKG)_HEADERS)
	@mkdir -pv $$(@D)
	$(TCC) -m$(BITS) -c $(PKGDIR_SRC:%=-I%) $($(PKG)_INCLUDES:%=-I%) $(CFLAGS) -o $$@ $$<

endef

.PHONY: test coverage

test: exe/test

coverage: test
	lcov -q -c -o $(BUILDDIR)/bin/lcov.info -d $(INTDIR)

include $(SRCDIR)pkg.mk

