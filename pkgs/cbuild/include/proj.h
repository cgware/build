#ifndef PROJ_H
#define PROJ_H

#include "config.h"

typedef enum target_type_s {
	TARGET_TYPE_UNKNOWN,
	TARGET_TYPE_EXE,
	TARGET_TYPE_LIB,
	TARGET_TYPE_EXT,
	TARGET_TYPE_TST,
	__TARGET_TYPE_MAX,
} target_type_t;

typedef struct target_s {
	size_t name;
	target_type_t type;
} target_t;

typedef struct pkg_s {
	size_t name;
	size_t path;
	size_t src;
	size_t inc;
	list_node_t targets;
	uint has_targets;
} pkg_t;

typedef struct proj_s {
	strvbuf_t strs;
	arr_t pkgs;
	list_t targets;
} proj_t;

proj_t *proj_init(proj_t *proj, uint pkgs_cap, uint targets_cap, alloc_t alloc);
void proj_free(proj_t *proj);

int proj_config(proj_t *proj, const config_t *config);

size_t proj_print(const proj_t *proj, dst_t dst);

#endif
