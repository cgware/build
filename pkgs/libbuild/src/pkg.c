#include "pkg.h"

#include "file.h"
#include "file/cfg_parse.h"
#include "log.h"

pkg_t *pkg_init(pkg_t *pkg)
{
	if (pkg == NULL) {
		return NULL;
	}

	pkg->type = PKG_TYPE_UNKNOWN;
	pkg->dir  = (path_t){0};
	pkg->src  = (path_t){0};
	pkg->inc  = (path_t){0};
	pkg->deps = LIST_END;

	return pkg;
}

void pkg_free(pkg_t *pkg)
{
	if (pkg == NULL) {
		return;
	}
}

static const char *pkg_type_str[] = {
	[PKG_TYPE_UNKNOWN] = "UNKNOWN",
	[PKG_TYPE_EXE]	   = "EXE",
	[PKG_TYPE_LIB]	   = "LIB",
};

int pkg_print(const pkg_t *pkg, print_dst_t dst)
{
	int off = dst.off;

	dst.off += c_dprintf(dst,
			     "[package]\n"
			     "TYPE: %s\n"
			     "DIR: %.*s\n"
			     "SRC: %.*s\n"
			     "INC: %.*s\n",
			     pkg_type_str[pkg->type],
			     pkg->dir.len,
			     pkg->dir.data,
			     pkg->src.len,
			     pkg->src.data,
			     pkg->inc.len,
			     pkg->inc.data);

	return dst.off - off;
}
