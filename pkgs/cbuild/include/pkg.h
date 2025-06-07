#ifndef PKG_H
#define PKG_H

#include "arr.h"
#include "file/cfg.h"
#include "list.h"
#include "path.h"
#include "strbuf.h"
#include "targets.h"

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
	uint id;
	size_t strs[__PKG_STR_CNT];
	list_node_t targets;
	pkg_url_proto_t proto;
	byte has_targets : 1;
	byte loaded : 1;
} pkg_t;

pkg_t *pkg_init(pkg_t *pkg, uint id);
void pkg_free(pkg_t *pkg);

target_t *pkg_add_target(pkg_t *pkg, targets_t *targets, strv_t name, list_node_t *id);

size_t pkg_print(const pkg_t *pkg, const targets_t *targets, dst_t dst);

#endif
