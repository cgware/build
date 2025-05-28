#ifndef targets_H
#define targets_H

#include "list.h"
#include "strbuf.h"
#include "target.h"

typedef struct targets_s {
	strbuf_t names;
	strbuf_t files;
	list_t targets;
	list_t deps;
	alloc_t alloc;
} targets_t;

targets_t *targets_init(targets_t *targets, uint targets_cap, alloc_t alloc);
void targets_free(targets_t *targets);

target_t *targets_target(targets_t *targets, strv_t name, list_node_t *id);
int targets_app(targets_t *targets, list_node_t list, list_node_t id);

target_t *targets_get(const targets_t *targets, list_node_t id);

target_t *targets_add_dep(targets_t *targets, list_node_t id, strv_t dep);
int targets_get_deps(const targets_t *targets, list_node_t target, arr_t *deps);

int targets_get_build_order(const targets_t *targets, arr_t *order);

size_t targets_print(const targets_t *targets, list_node_t start, dst_t dst);

#endif
