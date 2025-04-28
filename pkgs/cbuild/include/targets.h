#ifndef targets_H
#define targets_H

#include "list.h"
#include "target.h"
#include "strbuf.h"

typedef struct targets_s {
	strbuf_t names;
	strbuf_t files;
	list_t targets;
	list_t deps;
	alloc_t alloc;
} targets_t;

targets_t *targets_init(targets_t *targets, uint targets_cap, alloc_t alloc);
void targets_free(targets_t *targets);

target_t *targets_add(targets_t *targets, lnode_t *list, strv_t name, lnode_t *id);

target_t *targets_get(const targets_t *targets, lnode_t id);

target_t *targets_add_dep(targets_t *targets, lnode_t id, strv_t dep);
int targets_get_deps(const targets_t *targets, lnode_t target, arr_t *deps);

int targets_get_build_order(const targets_t *targets, arr_t *order);

int targets_print(const targets_t *targets, lnode_t start, print_dst_t dst);

#endif
