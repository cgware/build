#ifndef PKGS_H
#define PKGS_H

#include "pkg.h"
#include "strbuf.h"

typedef struct pkgs_s {
	arr_t pkgs;
	targets_t targets;
	strvbuf_t strs;
} pkgs_t;

pkgs_t *pkgs_init(pkgs_t *pkgs, uint pkgs_cap, alloc_t alloc);
void pkgs_free(pkgs_t *pkgs);

pkg_t *pkgs_add(pkgs_t *pkgs, uint *id);

int pkgs_set_str(pkgs_t *pkgs, size_t off, strv_t val);

int pkgs_set_uri(pkgs_t *pkgs, pkg_t *pkg, strv_t uri);

strv_t pkgs_get_name(const pkgs_t *pkgs, uint id);
pkg_t *pkgs_get(const pkgs_t *pkgs, uint id);

pkg_t *pkgs_find(const pkgs_t *pkgs, strv_t name, list_node_t *pkg);

int pkgs_get_build_order(const pkgs_t *pkgs, arr_t *order);

size_t pkgs_print(const pkgs_t *pkgs, dst_t dst);

#endif
