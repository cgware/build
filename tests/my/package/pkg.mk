PKGNAME := cbase

define cb
$(1)_HEADERS := $(PKGINC_H)
$(1)_INCLUDES := $(PKGINCDIR)

$(1)_SRC := $(PKGSRCDIR)
$(1)_H := $(PKGSRC_H)
$(1)_INTDIR := $(INTSRCDIR)
$(1)_OBJ := $(PKGSRC_OBJ)
$(1)_GCDA := $(PKGSRC_GCDA)
$(1) := $(PKGLIB)
endef

$(eval $(call cb,CBASE))

.PHONY: cbase
cbase: $(CBASE)

$(CBASE): $(CBASE_OBJ)
	@mkdir -p $(@D)
	ar rcs $@ $(CBASE_OBJ)

$(CBASE_INTDIR)%.o: $(CBASE_SRC)%.c $(CBASE_H) $(CBASE_HEADERS)
	@mkdir -p $(@D)
	$(TCC) -m$(BITS) -c $(patsubst %,-I%,$(CBASE_SRC) $(CBASE_INCLUDES)) $(CFLAGS) -o $@ $<

CBASE_TEST_LIBS := $(CBASE)

CBASE_TEST_SRC := $(PKGTESTDIR)
CBASE_TEST_H := $(PKGTEST_H)
CBASE_TEST_INTDIR := $(INTTESTDIR)
CBASE_TEST_OBJ := $(PKGTEST_OBJ)
CBASE_TEST_GCDA := $(PKGTEST_GCDA)
CBASE_TEST := $(PKGTEST)

.PHONY: cbase_test
cbase_test: $(CBASE_TEST)
	@rm -rf $(CBASE_GCDA) $(CBASE_TEST_GCDA)
	$(CBASE_TEST)

$(CBASE_TEST): $(CBASE_TEST_OBJ) $(CBASE_TEST_LIBS)
	@mkdir -p $(@D)
	$(TCC) -m$(BITS) $(LDFLAGS) -o $@ $(CBASE_TEST_OBJ) -L$(LIBSDIR) $(patsubst %,-l:%,$(notdir $(CBASE_TEST_LIBS)))

$(CBASE_TEST_INTDIR)%.o: $(CBASE_TEST_SRC)%.c $(CBASE_TEST_H) $(CBASE_HEADERS)
	@mkdir -p $(@D)
	$(TCC) -m$(BITS) -c $(patsubst %,-I%,$(CBASE_TEST_SRC) $(CBASE_INCLUDES)) $(CFLAGS) -o $@ $<
