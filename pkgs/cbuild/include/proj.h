#ifndef PROJ_H
#define PROJ_H

#include "path.h"
#include "strbuf.h"

typedef enum pkg_str_e {
	PKG_NAME,
	PKG_URL,
	PKG_DIR,
	PKG_SRC,
	PKG_INC,
	__PKG_STR_CNT,
} pkg_str_t;

typedef enum pkg_url_proto_e {
	PKG_URL_UNKNOWN,
	PKG_URL_GIT,
} pkg_url_proto_t;

typedef struct pkg_s {
	uint strs;
	pkg_url_proto_t proto;
	byte inited : 1;
} pkg_t;

typedef enum target_type_s {
	TARGET_TYPE_UNKNOWN,
	TARGET_TYPE_EXE,
	TARGET_TYPE_LIB,
	__TARGET_TYPE_MAX,
} target_type_t;

typedef enum target_str_e {
	TARGET_NAME,
	TARGET_FILE,
	__TARGET_STR_CNT,
} target_str_t;

typedef struct target_s {
	uint pkg;
	target_type_t type;
	uint strs;
	byte inited : 1;
} target_t;

typedef struct dep_s {
	uint from;
	uint to;
} dep_t;

typedef struct proj_s {
	strv_t name;
	path_t dir;
	path_t outdir;
	arr_t pkgs;
	arr_t targets;
	strbuf_t strs;
	arr_t deps;
	alloc_t alloc;
} proj_t;

proj_t *proj_init(proj_t *proj, uint pkgs_cap, uint targets_cnt, alloc_t alloc);
void proj_free(proj_t *proj);

pkg_t *proj_add_pkg(proj_t *proj, strv_t name, uint *id);
pkg_t *proj_get_pkg(const proj_t *proj, uint id);
pkg_t *proj_find_pkg(const proj_t *proj, strv_t name, uint *id);

target_t *proj_add_target(proj_t *proj, uint pkg, strv_t name, uint *id);
target_t *proj_get_target(const proj_t *proj, uint id);
target_t *proj_find_target(const proj_t *proj, uint pkg, strv_t name, uint *id);

int proj_set_str(proj_t *proj, uint id, strv_t val);
strv_t proj_get_str(const proj_t *proj, uint id);

int proj_add_dep(proj_t *proj, uint target, uint dep);
int proj_get_deps(const proj_t *proj, uint target, arr_t *deps);
size_t proj_print_deps(const proj_t *proj, dst_t dst);

int proj_get_pkg_build_order(const proj_t *proj, arr_t *order, alloc_t alloc);
int proj_get_target_build_order(const proj_t *proj, arr_t *order, alloc_t alloc);

size_t proj_print(const proj_t *proj, dst_t dst);

#endif
