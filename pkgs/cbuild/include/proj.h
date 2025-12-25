#ifndef PROJ_H
#define PROJ_H

#include "list.h"
#include "strbuf.h"

typedef enum target_type_s {
	TARGET_TYPE_UNKNOWN,
	TARGET_TYPE_EXE,
	TARGET_TYPE_LIB,
	TARGET_TYPE_EXT,
	TARGET_TYPE_TST,
	__TARGET_TYPE_MAX,
} target_type_t;

typedef enum target_str_e {
	TARGET_NAME,
	TARGET_PREP,
	TARGET_CONF,
	TARGET_COMP,
	TARGET_INST,
	TARGET_DST,
	__TARGET_STR_CNT,
} target_str_t;

typedef struct target_s {
	uint strs;
	target_type_t type;
	uint pkg;
	list_node_t deps;
	uint has_deps;
} target_t;

typedef enum pkg_uri_proto_e {
	PKG_URI_PROTO_UNKNOWN,
	PKG_URI_PROTO_GIT,
	PKG_URI_PROTO_HTTPS,
	__PKG_URI_PROTO_MAX,
} pkg_uri_proto_t;

typedef enum pkg_uri_ext_e {
	PKG_URI_EXT_UNKNOWN,
	PKG_URI_EXT_ZIP,
	PKG_URI_EXT_TAR_GZ,
	__PKG_URI_EXT_MAX,
} pkg_uri_ext_t;

typedef struct pkg_uri_s {
	pkg_uri_proto_t proto;
	pkg_uri_ext_t ext;
} pkg_uri_t;

typedef enum pkg_str_e {
	PKG_STR_NAME,
	PKG_STR_PATH,
	PKG_STR_SRC,
	PKG_STR_INC,
	PKG_STR_DRV,
	PKG_STR_TST,
	PKG_STR_URI,
	PKG_STR_URI_FILE,
	PKG_STR_URI_NAME,
	PKG_STR_URI_VER,
	PKG_STR_URI_DIR,
	__PKG_STR_CNT,
} pkg_str_t;

typedef struct pkg_s {
	uint strs;
	pkg_uri_t uri;
	list_node_t targets;
	uint has_targets;
	uint state;
} pkg_t;

typedef struct proj_s {
	strbuf_t strs;
	arr_t pkgs;
	list_t targets;
	list_t deps;
	uint name;
	uint outdir;
} proj_t;

proj_t *proj_init(proj_t *proj, uint pkgs_cap, uint targets_cap, alloc_t alloc);
void proj_free(proj_t *proj);

pkg_t *proj_add_pkg(proj_t *proj, uint *id);
pkg_t *proj_get_pkg(const proj_t *proj, uint id);
pkg_t *proj_find_pkg(const proj_t *proj, strv_t name, uint *id);

target_t *proj_add_target(proj_t *proj, uint pkg, uint *id);
target_t *proj_get_target(const proj_t *proj, uint id);
target_t *proj_find_target(const proj_t *proj, uint pkg, strv_t name, uint *id);

int proj_set_str(proj_t *proj, uint id, strv_t val);
strv_t proj_get_str(const proj_t *proj, uint id);

int proj_add_dep(proj_t *proj, uint target, uint dep);
int proj_get_deps(const proj_t *proj, list_node_t target, arr_t *deps);

int proj_get_pkg_build_order(const proj_t *proj, arr_t *order, alloc_t alloc);

size_t proj_print(const proj_t *proj, dst_t dst);

#endif
