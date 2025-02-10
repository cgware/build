#ifndef PROJ_H
#define PROJ_H

#include "arr.h"
#include "pkg.h"

typedef enum gen_e {
	GEN_MAKE,
} gen_t;

typedef struct proj_s {
	arr_t pkgs;
} proj_t;

proj_t *proj_init(proj_t *proj, uint pkgs_cap, alloc_t alloc);
void proj_free(proj_t *proj);

pkg_t *proj_add_pkg(proj_t *proj);

int proj_gen(const proj_t *proj, gen_t gen);

#endif
