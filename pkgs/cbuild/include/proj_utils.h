#ifndef PROJ_UTILS
#define PROJ_UTILS

#include "proj.h"

pkg_t *proj_add_pkg_target(proj_t *proj, strv_t name, uint *pkg_id, uint *target_id);

int proj_add_dep_uri(proj_t *proj, uint target, strv_t uri);

int proj_set_ext_uri(proj_t *proj, pkg_t *pkg, strv_t uri);

#endif
