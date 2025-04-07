#ifndef PKGS_H
#define PKGS_H

#include "arr.h"
#include "list.h"
#include "pkg.h"
#include "strbuf.h"

typedef struct pkgs_s {
	strbuf_t names;
	arr_t pkgs;
	list_t deps;
	alloc_t alloc;
} pkgs_t;

pkgs_t *pkgs_init(pkgs_t *pkgs, uint pkgs_cap, alloc_t alloc);
void pkgs_free(pkgs_t *pkgs);

pkg_t *pkgs_add_pkg(pkgs_t *pkgs, strv_t name, uint *id);

strv_t pkgs_get_pkg_name(const pkgs_t *pkgs, uint id);
pkg_t *pkgs_get_pkg(const pkgs_t *pkgs, uint id);

int pkgs_add_dep(pkgs_t *pkgs, uint id, strv_t dep);
int pkgs_get_pkg_deps(const pkgs_t *pkgs, uint pkg, arr_t *deps);

int pkgs_get_build_order(const pkgs_t *pkgs, arr_t *order);

int pkgs_print(const pkgs_t *pkgs, print_dst_t dst);

#endif
