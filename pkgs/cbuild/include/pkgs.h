#ifndef PKGS_H
#define PKGS_H

#include "pkg.h"
#include "strbuf.h"

typedef struct pkgs_s {
	strbuf_t names;
	arr_t pkgs;
	alloc_t alloc;
} pkgs_t;

pkgs_t *pkgs_init(pkgs_t *pkgs, uint pkgs_cap, alloc_t alloc);
void pkgs_free(pkgs_t *pkgs);

pkg_t *pkgs_add_pkg(pkgs_t *pkgs, strv_t name, uint *id);

strv_t pkgs_get_pkg_name(const pkgs_t *pkgs, uint id);
pkg_t *pkgs_get_pkg(const pkgs_t *pkgs, uint id);

int pkgs_get_build_order(const pkgs_t *pkgs, const targets_t *targets, arr_t *order);

int pkgs_print(const pkgs_t *pkgs, const targets_t *targets, print_dst_t dst);

#endif
