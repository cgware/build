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
	arr_foreach(&pkgs->pkgs, pkg)
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

	pkg_t *pkg = arr_add(&pkgs->pkgs);
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

	return arr_get(&pkgs->pkgs, id);
}

static int get_target_deps(const targets_t *targets, const target_t *target, arr_t *arr)
{
	int ret = 0;

	const uint *dep;
	list_foreach(&targets->deps, target->deps, dep)
	{
		const target_t *dep_target = list_get_data(&targets->targets, *dep);
		get_target_deps(targets, dep_target, arr);
		uint index = arr_addu(arr, &targets_get(targets, *dep)->pkg);
		ret |= index >= arr->cnt;
	}

	return ret;
}

static int get_pkg_targets(const pkgs_t *pkgs, const targets_t *targets, uint id, arr_t *arr)
{
	int ret = 0;

	const pkg_t *pkg = arr_get(&pkgs->pkgs, id);

	const target_t *target;
	list_foreach(&targets->targets, pkg->targets, target)
	{
		get_target_deps(targets, target, arr);
		uint index = arr_addu(arr, &target->pkg);
		ret |= index >= arr->cnt;
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
		uint index = arr_addu(order, &i);
		ret |= index >= order->cnt;
	}

	return ret;
}

int pkgs_print(const pkgs_t *pkgs, const targets_t *targets, print_dst_t dst)
{
	int off = dst.off;

	const pkg_t *pkg;
	arr_foreach(&pkgs->pkgs, pkg)
	{
		dst.off += pkg_print(pkg, targets, dst);
		dst.off += c_dprintf(dst, "\n");
	}

	return dst.off - off;
}
