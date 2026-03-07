#ifndef REGISTRY_H
#define REGISTRY_H

#include "arr.h"
#include "strvbuf.h"

typedef struct registry_s {
	arr_t pkgs;
	arr_t tgts;
	arr_t ops;
	strvbuf_t strs;
} registry_t;

registry_t *registry_init(registry_t *registry, uint cap, alloc_t alloc);
void registry_free(registry_t *registry);

int registry_add_pkg(registry_t *registry, strv_t name, uint *id);
strv_t registry_get_pkg(const registry_t *registry, uint id);
int registry_find_pkg(const registry_t *registry, strv_t name, uint *id);

int registry_add_tgt(registry_t *registry, uint pkg, strv_t name, uint *id);
strv_t registry_get_tgt(const registry_t *registry, uint id);
int registry_find_tgt(const registry_t *registry, uint pkg, strv_t name, uint *id);

#endif
