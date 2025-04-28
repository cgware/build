#ifndef PKG_H
#define PKG_H

#include "arr.h"
#include "file/cfg.h"
#include "list.h"
#include "path.h"
#include "strbuf.h"
#include "targets.h"

typedef struct pkg_s {
	uint id;
	path_t dir;
	path_t src;
	path_t inc;
	lnode_t targets;
} pkg_t;

pkg_t *pkg_init(pkg_t *pkg, uint id);
void pkg_free(pkg_t *pkg);

target_t *pkg_add_target(pkg_t *pkg, targets_t *targets, strv_t name, lnode_t *id);

int pkg_print(const pkg_t *pkg, const targets_t *targets, print_dst_t dst);

#endif
