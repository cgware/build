#ifndef PROJ_H
#define PROJ_H

#include "fs.h"
#include "path.h"
#include "pkgs.h"

typedef enum proj_str_e {
	EXT_NAME,
	EXT_URL,
	__EXT_STR_CNT,
} proj_str_t;

typedef enum ext_proto_e {
	EXT_UNKNOWN,
	EXT_GIT,
} ext_proto_t;

typedef struct ext_s {
	ext_proto_t proto;
	size_t strs[__EXT_STR_CNT];
} ext_t;

typedef struct proj_s {
	path_t dir;
	path_t outdir;
	pkgs_t pkgs;
	strvbuf_t strs;
} proj_t;

proj_t *proj_init(proj_t *proj, uint pkgs_cap, alloc_t alloc);
void proj_free(proj_t *proj);

pkg_t *proj_set_pkg(proj_t *proj, uint *id);
pkg_t *proj_add_pkg(proj_t *proj, uint *id);

size_t proj_print(const proj_t *proj, dst_t dst);

#endif
