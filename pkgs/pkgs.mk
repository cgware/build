include $(SRCDIR)pkgs/cbuild/pkg.mk
include $(SRCDIR)pkgs/build/pkg.mk

.PHONY: pkgs/test
pkgs/test: cbuild/test build/test
