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

int pkg_set_source(pkg_t *pkg, strv_t source)
{
	if (pkg == NULL) {
		return 1;
	}
	(void)source;
	/*
	path_init(&pkg->src, source);
	path_add_child(&pkg->src, STRV("src"));

	if (!path_is_dir(&pkg->src)) {
		log_info("build", "pkg", NULL, "src folder not found at path: %.*s", source.len, source.data);
		pkg->src.len = 0;
	}

	path_init(&pkg->include, source);
	path_add_child(&pkg->include, STRV("include"));

	if (!path_is_dir(&pkg->include)) {
		log_info("build", "pkg", NULL, "include folder not found at path: %.*s", source.len, source.data);
		pkg->include.len = 0;
	}
	*/
	return 0;
}
