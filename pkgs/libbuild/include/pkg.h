#ifndef PKG_H
#define PKG_H

#include "path.h"
#include "strv.h"

typedef enum pkg_type_s {
	PKG_TYPE_UNKNOWN,
	PKG_TYPE_EXE,
	PKG_TYPE_LIB,
} pkg_type_t;

typedef struct pkg_s {
	path_t dir;
	path_t src;
	path_t include;
	strv_t name;
	pkg_type_t type;
} pkg_t;

pkg_t *pkg_init(pkg_t *pkg);
void pkg_free(pkg_t *pkg);

int pkg_set_dir(pkg_t *pkg, strv_t dir);

int pkg_print(const pkg_t *pkg, print_dst_t dst);

#endif
