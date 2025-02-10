#ifndef PKG_H
#define PKG_H

#include "path.h"
#include "strv.h"

typedef struct pkg_s {
	path_t src;
	path_t include;
} pkg_t;

pkg_t *pkg_init(pkg_t *pkg);
void pkg_free(pkg_t *pkg);

int pkg_set_source(pkg_t *pkg, strv_t source);

#endif
