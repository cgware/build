#include "registry.h"

#include "log.h"

registry_t *registry_init(registry_t *registry, uint cap, alloc_t alloc)
{
	if (registry == NULL) {
		return NULL;
	}

	if (arr_init(&registry->pkgs, cap, sizeof(size_t), alloc) == NULL ||
	    arr_init(&registry->tgts, cap, sizeof(size_t), alloc) == NULL || strvbuf_init(&registry->strs, cap * 2, 16, alloc) == NULL) {
		return NULL;
	}

	return registry;
}

void registry_free(registry_t *registry)
{
	if (registry == NULL) {
		return;
	}

	arr_free(&registry->pkgs);
	arr_free(&registry->tgts);
	strvbuf_free(&registry->strs);
}

int registry_add_pkg(registry_t *registry, strv_t name, uint *id)
{
	if (registry == NULL) {
		return 1;
	}

	if (registry_find_pkg(registry, name, id) == 0) {
		return 0;
	}

	size_t off;
	if (strvbuf_add(&registry->strs, name, &off)) {
		log_error("cbuild", "registry", NULL, "failed to add package name: %.*s", name.len, name.data);
		return 1;
	}

	uint pkgs_cnt;
	size_t *pkg = arr_add(&registry->pkgs, &pkgs_cnt);
	if (pkg == NULL) {
		log_error("cbuild", "registry", NULL, "failed to add package");
		strvbuf_reset(&registry->strs, off);
		return 1;
	}

	*pkg = off;

	if (id) {
		*id = pkgs_cnt;
	}

	return 0;
}

strv_t registry_get_pkg(const registry_t *registry, uint id)
{
	if (registry == NULL) {
		return STRV_NULL;
	}

	size_t *off = arr_get(&registry->pkgs, id);
	if (off == NULL) {
		log_error("cbuild", "registry", NULL, "invalid package id: %d", id);
		return STRV_NULL;
	}

	return strvbuf_get(&registry->strs, *off);
}

int registry_find_pkg(const registry_t *registry, strv_t name, uint *id)
{
	if (registry == NULL) {
		return 1;
	}

	uint i = 0;
	size_t *pkg;
	arr_foreach(&registry->pkgs, i, pkg)
	{
		strv_t pkg_name = strvbuf_get(&registry->strs, *pkg);
		if (strv_eq(pkg_name, name)) {
			if (id) {
				*id = i;
			}
			return 0;
		}
	}

	return 1;
}

int registry_add_tgt(registry_t *registry, strv_t name, uint *id)
{
	if (registry == NULL) {
		return 1;
	}

	if (registry_find_tgt(registry, name, id) == 0) {
		return 0;
	}

	size_t off;
	if (strvbuf_add(&registry->strs, name, &off)) {
		log_error("cbuild", "registry", NULL, "failed to add target name: %.*s", name.len, name.data);
		return 1;
	}

	uint tgts_cnt;
	size_t *tgt = arr_add(&registry->tgts, &tgts_cnt);
	if (tgt == NULL) {
		log_error("cbuild", "registry", NULL, "failed to add target");
		strvbuf_reset(&registry->strs, off);
		return 1;
	}

	*tgt = off;

	if (id) {
		*id = tgts_cnt;
	}

	return 0;
}

strv_t registry_get_tgt(const registry_t *registry, uint id)
{
	if (registry == NULL) {
		return STRV_NULL;
	}

	size_t *off = arr_get(&registry->tgts, id);
	if (off == NULL) {
		log_error("cbuild", "registry", NULL, "invalid target id: %d", id);
		return STRV_NULL;
	}

	return strvbuf_get(&registry->strs, *off);
}

int registry_find_tgt(const registry_t *registry, strv_t name, uint *id)
{
	if (registry == NULL) {
		return 1;
	}

	uint i = 0;
	size_t *tgt;
	arr_foreach(&registry->tgts, i, tgt)
	{
		strv_t tgt_name = strvbuf_get(&registry->strs, *tgt);
		if (strv_eq(tgt_name, name)) {
			if (id) {
				*id = i;
			}
			return 0;
		}
	}

	return 1;
}
