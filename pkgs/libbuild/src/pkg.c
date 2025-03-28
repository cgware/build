#include "pkg.h"

#include "log.h"

pkg_t *pkg_init(pkg_t *pkg)
{
	if (pkg == NULL) {
		return NULL;
	}

	return pkg;
}

void pkg_free(pkg_t *pkg)
{
	if (pkg == NULL) {
		return;
	}
}

int pkg_set_dir(pkg_t *pkg, strv_t dir)
{
	if (pkg == NULL) {
		return 1;
	}

	path_init(&pkg->dir, dir);

	path_init(&pkg->src, dir);
	path_child(&pkg->src, STRV("src"));

	if (!path_is_dir(&pkg->src)) {
		pkg->src.len = 0;
	} else {
		pkg->type = PKG_TYPE_EXE;
	}

	path_init(&pkg->include, dir);
	path_child(&pkg->include, STRV("include"));

	if (!path_is_dir(&pkg->include)) {
		pkg->include.len = 0;
	} else {
		pkg->type = PKG_TYPE_LIB;
	}

	return 0;
}

int pkg_print(const pkg_t *pkg, print_dst_t dst)
{
	int off = dst.off;

	dst.off += c_dprintf(dst,
			     "[project.package]\n"
			     "SRC: %.*s\n"
			     "INCLUDE: %.*s\n"
			     "\n",
			     pkg->src.len,
			     pkg->src.data,
			     pkg->include.len,
			     pkg->include.data);

	return dst.off - off;
}
