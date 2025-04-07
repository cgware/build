#ifndef PKG_H
#define PKG_H

#include "arr.h"
#include "file/cfg.h"
#include "list.h"
#include "path.h"
#include "strbuf.h"

typedef enum pkg_type_s {
	PKG_TYPE_UNKNOWN,
	PKG_TYPE_EXE,
	PKG_TYPE_LIB,
} pkg_type_t;

typedef struct pkg_s {
	path_t dir;
	path_t src;
	path_t inc;
	pkg_type_t type;
	lnode_t deps;
} pkg_t;

pkg_t *pkg_init(pkg_t *pkg);
void pkg_free(pkg_t *pkg);

int pkg_print(const pkg_t *pkg, print_dst_t dst);

#endif
