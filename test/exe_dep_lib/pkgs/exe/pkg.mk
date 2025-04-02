PKG := exe
$(PKG)_HEADERS :=
$(PKG)_INCLUDES :=
$(PKG)_LIBS := $(lib)
$(PKG)_DRIVERS :=
$(eval $(call pkg/exe))
