#include "pkgs.h"

#include "log.h"

pkgs_t *pkgs_init(pkgs_t *pkgs, uint pkgs_cap, alloc_t alloc)
{
	if (pkgs == NULL) {
		return NULL;
	}

	if (arr_init(&pkgs->pkgs, pkgs_cap, sizeof(pkg_t), alloc) == NULL || targets_init(&pkgs->targets, pkgs_cap * 2, alloc) == NULL ||
	    strvbuf_init(&pkgs->strs, pkgs_cap, 16 * __PKG_STR_CNT, alloc) == NULL) {
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
	targets_free(&pkgs->targets);
	strvbuf_free(&pkgs->strs);
}

pkg_t *pkgs_add(pkgs_t *pkgs, uint *id)
{
	if (pkgs == NULL) {
		return NULL;
	}

	uint pkgs_cnt = pkgs->pkgs.cnt;

	uint tmp;
	pkg_t *pkg = arr_add(&pkgs->pkgs, &tmp);
	if (pkg == NULL) {
		return NULL;
	}

	size_t strs_used = pkgs->strs.used;

	for (uint i = 0; i < __PKG_STR_CNT; i++) {
		if (strvbuf_add(&pkgs->strs, STRV_NULL, &pkg->strs[i])) {
			arr_reset(&pkgs->pkgs, pkgs_cnt);
			strvbuf_reset(&pkgs->strs, strs_used);
			return NULL;
		}
	}

	if (id) {
		*id = tmp;
	}

	return pkg_init(pkg, tmp);
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
		for (uint j = 0; j < __PKG_STR_CNT; j++) {
			if (pkg->strs[j] > off) {
				pkg->strs[j] += diff;
			}
		}
	}

	return 0;
}

strv_t pkgs_get_name(const pkgs_t *pkgs, uint id)
{
	if (pkgs == NULL) {
		return STRV_NULL;
	}

	const pkg_t *pkg = pkgs_get(pkgs, id);
	if (pkg == NULL) {
		return STRV_NULL;
	}

	return strvbuf_get(&pkgs->strs, pkg->strs[PKG_NAME]);
}

pkg_t *pkgs_get(const pkgs_t *pkgs, uint id)
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

pkg_t *pkgs_find(const pkgs_t *pkgs, strv_t name, list_node_t *pkg)
{
	if (pkgs == NULL) {
		return NULL;
	}

	pkg_t *data;
	uint i = 0;
	arr_foreach(&pkgs->pkgs, i, data)
	{
		if (strv_eq(strvbuf_get(&pkgs->strs, data->strs[PKG_NAME]), name)) {
			if (pkg) {
				*pkg = i;
			}
			return data;
		}
	}

	return NULL;
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

static int get_pkg_targets(const pkgs_t *pkgs, uint id, arr_t *arr)
{
	const pkg_t *pkg = arr_get(&pkgs->pkgs, id);

	if (!pkg->has_targets) {
		return 0;
	}

	int ret = 0;
	const target_t *target;
	list_node_t i = pkg->targets;
	list_foreach(&pkgs->targets.targets, i, target)
	{
		get_target_deps(&pkgs->targets, target, arr);
		ret |= arr_addu(arr, &target->pkg, NULL);
	}

	return ret;
}

int pkgs_get_build_order(const pkgs_t *pkgs, arr_t *order)
{
	if (pkgs == NULL || order == NULL) {
		return 1;
	}

	int ret = 0;
	for (uint i = 0; i < pkgs->pkgs.cnt; i++) {
		ret |= get_pkg_targets(pkgs, i, order);
		ret |= arr_addu(order, &i, NULL);
	}

	return ret;
}

size_t pkgs_print(const pkgs_t *pkgs, dst_t dst)
{
	size_t off = dst.off;

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&pkgs->pkgs, i, pkg)
	{
		strv_t dir  = strvbuf_get(&pkgs->strs, pkg->strs[PKG_DIR]);
		strv_t name = strvbuf_get(&pkgs->strs, pkg->strs[PKG_NAME]);
		strv_t src  = strvbuf_get(&pkgs->strs, pkg->strs[PKG_SRC]);
		strv_t inc  = strvbuf_get(&pkgs->strs, pkg->strs[PKG_INC]);
		dst.off += dputf(dst,
				 "[package]\n"
				 "ID: %d\n"
				 "NAME: %.*s\n"
				 "DIR: %.*s\n"
				 "SRC: %.*s\n"
				 "INC: %.*s\n",
				 pkg->id,
				 dir.len,
				 dir.data,
				 name.len,
				 name.data,
				 src.len,
				 src.data,
				 inc.len,
				 inc.data);
		dst.off += pkg_print(pkg, &pkgs->targets, dst);
		dst.off += dputs(dst, STRV("\n"));
	}

	return dst.off - off;
}
