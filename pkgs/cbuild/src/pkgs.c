#include "pkgs.h"

#include "log.h"

pkgs_t *pkgs_init(pkgs_t *pkgs, uint pkgs_cap, alloc_t alloc)
{
	if (pkgs == NULL) {
		return NULL;
	}

	if (strbuf_init(&pkgs->names, pkgs_cap, pkgs_cap * 8, alloc) == NULL ||
	    arr_init(&pkgs->pkgs, pkgs_cap, sizeof(pkg_t), alloc) == NULL) {
		log_error("build", "proj", NULL, "failed to initialize packages");
		return NULL;
	}

	pkgs->alloc = alloc;

	return pkgs;
}

void pkgs_free(pkgs_t *pkgs)
{
	if (pkgs == NULL) {
		return;
	}

	pkg_t *pkg;
	uint i = 0;
	arr_foreach(&pkgs->pkgs, i, pkg)
	{
		pkg_free(pkg);
	}

	arr_free(&pkgs->pkgs);
	strbuf_free(&pkgs->names);
}

pkg_t *pkgs_add_pkg(pkgs_t *pkgs, strv_t name, uint *id)
{
	if (pkgs == NULL) {
		return NULL;
	}

	uint index;
	if (strbuf_find(&pkgs->names, name, &index) == 0) {
		if (id) {
			*id = index;
		}
		return arr_get(&pkgs->pkgs, index);
	}

	if (strbuf_add(&pkgs->names, name, &index)) {
		return NULL;
	}

	pkg_t *pkg = arr_add(&pkgs->pkgs, NULL);
	if (pkg == NULL) {
		return NULL;
	}

	if (id) {
		*id = index;
	}

	return pkg_init(pkg, index);
}

strv_t pkgs_get_pkg_name(const pkgs_t *pkgs, uint id)
{
	if (pkgs == NULL) {
		return STRV_NULL;
	}

	return strbuf_get(&pkgs->names, id);
}

pkg_t *pkgs_get_pkg(const pkgs_t *pkgs, uint id)
{
	if (pkgs == NULL) {
		return NULL;
	}

	pkg_t *pkg = arr_get(&pkgs->pkgs, id);
	if (pkg == NULL) {
		log_error("cbuild", "pkgs", NULL, "failed to get package");
		return NULL;
	}

	return pkg;
}

static int get_target_deps(const targets_t *targets, const target_t *target, arr_t *arr)
{
	if (!target->has_deps) {
		return 0;
	}

	int ret = 0;

	const uint *dep;
	list_node_t i = target->deps;
	list_foreach(&targets->deps, i, dep)
	{
		const target_t *dep_target = targets_get(targets, *dep);
		get_target_deps(targets, dep_target, arr);
		ret |= arr_addu(arr, &targets_get(targets, *dep)->pkg, NULL);
	}

	return ret;
}

static int get_pkg_targets(const pkgs_t *pkgs, const targets_t *targets, uint id, arr_t *arr)
{
	const pkg_t *pkg = arr_get(&pkgs->pkgs, id);

	if (!pkg->has_targets) {
		return 0;
	}

	int ret = 0;
	const target_t *target;
	list_node_t i = pkg->targets;
	list_foreach(&targets->targets, i, target)
	{
		get_target_deps(targets, target, arr);
		ret |= arr_addu(arr, &target->pkg, NULL);
	}

	return ret;
}

int pkgs_get_build_order(const pkgs_t *pkgs, const targets_t *targets, arr_t *order)
{
	if (pkgs == NULL || order == NULL) {
		return 1;
	}

	int ret = 0;
	for (uint i = 0; i < pkgs->pkgs.cnt; i++) {
		ret |= get_pkg_targets(pkgs, targets, i, order);
		ret |= arr_addu(order, &i, NULL);
	}

	return ret;
}

size_t pkgs_print(const pkgs_t *pkgs, const targets_t *targets, dst_t dst)
{
	size_t off = dst.off;

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&pkgs->pkgs, i, pkg)
	{
		dst.off += pkg_print(pkg, targets, dst);
		dst.off += dputs(dst, STRV("\n"));
	}

	return dst.off - off;
}
