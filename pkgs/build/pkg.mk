PKGNAME := build
BUILD_HEADERS := $(LIBBUILD_HEADERS)
BUILD_INCLUDES := $(LIBBUILD_INCLUDES)
BUILD_LIBS := $(LIBBUILD) $(CPARSE) $(CUTILS) $(CTEST) $(CBASE)

BUILD_SRC := $(PKGSRCDIR)
BUILD_H := $(PKGSRC_H)
BUILD_INTDIR := $(INTSRCDIR)
BUILD_OBJ := $(PKGSRC_OBJ)
BUILD_GCDA := $(PKGSRC_GCDA)
BUILD := $(PKGEXE)

.PHONY: build
build: $(BUILD)

$(BUILD): $(BUILD_OBJ) $(BUILD_LIBS)
	@mkdir -p $(@D)
	$(TCC) -m$(BITS) $(LDFLAGS) -o $@ $(BUILD_OBJ) -L$(LIBSDIR) $(patsubst %,-l:%,$(notdir $(BUILD_LIBS)))

$(BUILD_INTDIR)%.o: $(BUILD_SRC)%.c $(BUILD_H) $(BUILD_HEADERS)
	@mkdir -p $(@D)
	$(TCC) -m$(BITS) -c $(patsubst %,-I%,$(BUILD_SRC) $(BUILD_INCLUDES)) $(CFLAGS) -o $@ $<

BUILD_TEST_LIBS := $(CTEST) $(CBASE)

BUILD_TEST_SRC := $(PKGTESTDIR)
BUILD_TEST_H := $(PKGTEST_H)
BUILD_TEST_INTDIR := $(INTTESTDIR)
BUILD_TEST_OBJ := $(PKGTEST_OBJ)
BUILD_TEST_GCDA := $(PKGTEST_GCDA)
BUILD_TEST := $(PKGTEST)

.PHONY: build/test
build/test: $(BUILD) $(BUILD_TEST)
	@rm -rf $(BUILD_GCDA) $(BUILD_TEST_GCDA)
	$(BUILD_TEST)

$(BUILD_TEST): $(BUILD_TEST_OBJ) $(BUILD_TEST_LIBS)
	@mkdir -p $(@D)
	$(TCC) -m$(BITS) $(LDFLAGS) -o $@ $(BUILD_TEST_OBJ) -L$(LIBSDIR) $(patsubst %,-l:%,$(notdir $(BUILD_TEST_LIBS)))

$(BUILD_TEST_INTDIR)%.o: $(BUILD_TEST_SRC)%.c $(BUILD_TEST_H) $(BUILD_HEADERS) $(CTEST_HEADERS)
	@mkdir -p $(@D)
	$(TCC) -m$(BITS) -c $(patsubst %,-I%,$(BUILD_TEST_SRC) $(BUILD_INCLUDES) $(CTEST_INCLUDES)) $(CFLAGS) -o $@ $<
