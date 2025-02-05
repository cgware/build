include $(SRCDIR)pkgs/libbuild/pkg.mk
include $(SRCDIR)pkgs/build/pkg.mk

.PHONY: pkgs/test
pkgs/test: libbuild/test build/test
