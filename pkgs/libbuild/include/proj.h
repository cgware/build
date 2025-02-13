#ifndef PROJ_H
#define PROJ_H

#include "arr.h"
#include "pkg.h"

typedef struct proj_s {
	path_t builddir;
	path_t outdir;
	arr_t pkgs;
} proj_t;

proj_t *proj_init(proj_t *proj, uint pkgs_cap, alloc_t alloc);
void proj_free(proj_t *proj);

pkg_t *proj_add_pkg(proj_t *proj);

int proj_print(const proj_t *proj, print_dst_t dst);

#endif
