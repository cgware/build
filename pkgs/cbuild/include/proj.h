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
	uint pkg;
	list_node_t deps;
	uint has_deps;
	size_t out;
	size_t cmd;
} target_t;

typedef enum pkg_uri_proto_e {
	PKG_URI_PROTO_UNKNOWN,
	PKG_URI_PROTO_GIT,
	PKG_URI_PROTO_HTTPS,
	__PKG_URI_PROTO_MAX,
} pkg_uri_proto_t;

typedef enum pkg_uri_ext_e {
	PKG_URI_EXT_NONE,
	PKG_URI_EXT_ZIP,
	__PKG_URI_EXT_MAX,
} pkg_uri_ext_t;

typedef struct pkg_uri_s {
	pkg_uri_proto_t proto;
	pkg_uri_ext_t ext;
} pkg_uri_t;

typedef struct pkg_s {
	size_t name;
	size_t path;
	size_t src;
	size_t inc;
	size_t test;
	size_t uri_str;
	size_t uri_name;
	size_t uri_dir;
	pkg_uri_t uri;
	list_node_t targets;
	uint has_targets;

} pkg_t;

typedef struct proj_s {
	size_t name;
	strvbuf_t strs;
	size_t outdir;
	arr_t pkgs;
	list_t targets;
	list_t deps;
} proj_t;

proj_t *proj_init(proj_t *proj, uint pkgs_cap, uint targets_cap, alloc_t alloc);
void proj_free(proj_t *proj);

pkg_t *proj_get_pkg(const proj_t *proj, uint id);

target_t *proj_get_target(const proj_t *proj, uint id);

int proj_get_deps(const proj_t *proj, list_node_t target, arr_t *deps);

int proj_get_pkg_build_order(const proj_t *proj, arr_t *order, alloc_t alloc);

int proj_config(proj_t *proj, const config_t *config);

size_t proj_print(const proj_t *proj, dst_t dst);

#endif
