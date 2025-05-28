#include "pkgs.h"

#include "log.h"

pkgs_t *pkgs_init(pkgs_t *pkgs, uint pkgs_cap, alloc_t alloc)
{
	if (pkgs == NULL) {
		return NULL;
	}

	if (strvbuf_init(&pkgs->strs, pkgs_cap, 16 * 4, alloc) == NULL || arr_init(&pkgs->pkgs, pkgs_cap, sizeof(pkg_t), alloc) == NULL) {
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
	strvbuf_free(&pkgs->strs);
}

static pkg_t *get_pkg(const pkgs_t *pkgs, strv_t name, list_node_t *node)
{
	pkg_t *pkg;
	uint i = 0;
	arr_foreach(&pkgs->pkgs, i, pkg)
	{
		if (strv_eq(strvbuf_get(&pkgs->strs, pkg->name), name)) {
			if (node) {
				*node = i;
			}
			return pkg;
		}
	}

	return NULL;
}

pkg_t *pkgs_add_pkg(pkgs_t *pkgs, strv_t name, uint *id)
{
	if (pkgs == NULL) {
		return NULL;
	}

	pkg_t *pkg = get_pkg(pkgs, name, id);
	if (pkg) {
		return pkg;
	}

	size_t strs_used = pkgs->strs.used;

	size_t name_off;
	if (strvbuf_add(&pkgs->strs, name, &name_off)) {
		return NULL;
	}

	size_t dir_off;
	if (strvbuf_add(&pkgs->strs, STRV_NULL, &dir_off)) {
		strvbuf_reset(&pkgs->strs, strs_used);
		return NULL;
	}

	size_t src_off;
	if (strvbuf_add(&pkgs->strs, STRV_NULL, &src_off)) {
		strvbuf_reset(&pkgs->strs, strs_used);
		return NULL;
	}

	size_t inc_off;
	if (strvbuf_add(&pkgs->strs, STRV_NULL, &inc_off)) {
		strvbuf_reset(&pkgs->strs, strs_used);
		return NULL;
	}

	uint tmp;
	pkg = arr_add(&pkgs->pkgs, &tmp);
	if (pkg == NULL) {
		strvbuf_reset(&pkgs->strs, strs_used);
		return NULL;
	}

	if (id) {
		*id = tmp;
	}

	pkg_init(pkg, tmp);
	pkg->name = name_off;
	pkg->dir  = dir_off;
	pkg->src  = src_off;
	pkg->inc  = inc_off;
	return pkg;
}

int pkgs_set_str(pkgs_t *pkgs, size_t off, strv_t val)
{
	if (pkgs == NULL) {
		return 1;
	}

	size_t old_len = pkgs->strs.used;

	if (strvbuf_set(&pkgs->strs, off, val)) {
		return 1;
	}

	int diff = (int)(pkgs->strs.used - old_len);

	pkg_t *pkg;
	uint i = 0;
	arr_foreach(&pkgs->pkgs, i, pkg)
	{
		if (pkg->name > off) {
			pkg->name += diff;
		}
		if (pkg->dir > off) {
			pkg->dir += diff;
		}
		if (pkg->src > off) {
			pkg->src += diff;
		}
		if (pkg->inc > off) {
			pkg->inc += diff;
		}
	}

	return 0;
}

strv_t pkgs_get_pkg_name(const pkgs_t *pkgs, uint id)
{
	if (pkgs == NULL) {
		return STRV_NULL;
	}

	const pkg_t *pkg = pkgs_get_pkg(pkgs, id);
	if (pkg == NULL) {
		return STRV_NULL;
	}

	return strvbuf_get(&pkgs->strs, pkg->name);
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
		strv_t name = strvbuf_get(&pkgs->strs, pkg->name);
		strv_t dir  = strvbuf_get(&pkgs->strs, pkg->dir);
		strv_t src  = strvbuf_get(&pkgs->strs, pkg->src);
		strv_t inc  = strvbuf_get(&pkgs->strs, pkg->inc);
		dst.off += dputf(dst,
				 "[package]\n"
				 "ID: %d\n"
				 "NAME: %.*s\n"
				 "DIR: %.*s\n"
				 "SRC: %.*s\n"
				 "INC: %.*s\n",
				 pkg->id,
				 name.len,
				 name.data,
				 dir.len,
				 dir.data,
				 src.len,
				 src.data,
				 inc.len,
				 inc.data);
		dst.off += pkg_print(pkg, targets, dst);
		dst.off += dputs(dst, STRV("\n"));
	}

	return dst.off - off;
}
