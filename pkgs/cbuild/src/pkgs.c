#include "pkgs.h"

#include "log.h"

pkgs_t *pkgs_init(pkgs_t *pkgs, uint pkgs_cap, alloc_t alloc)
{
	if (pkgs == NULL) {
		return NULL;
	}

	if (strbuf_init(&pkgs->names, pkgs_cap, pkgs_cap * 8, alloc) == NULL ||
	    arr_init(&pkgs->pkgs, pkgs_cap, sizeof(pkg_t), alloc) == NULL ||
	    list_init(&pkgs->deps, pkgs_cap, sizeof(lnode_t), alloc) == NULL) {
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

	list_free(&pkgs->deps);
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
	if (strbuf_get_index(&pkgs->names, name, &index) == 0) {
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

	return pkg_init(pkg);
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

int pkgs_add_dep(pkgs_t *pkgs, uint id, strv_t dep)
{
	if (pkgs == NULL) {
		return 1;
	}

	uint dep_id;
	if (strbuf_get_index(&pkgs->names, dep, &dep_id) && pkgs_add_pkg(pkgs, dep, &dep_id) == NULL) {
		return 1;
	}

	pkg_t *pkg = arr_get(&pkgs->pkgs, id);
	if (pkg == NULL) {
		log_error("build", "pkgs", NULL, "package is not in dependecy list");
		return 1;
	}

	lnode_t node;
	list_add_next_node(&pkgs->deps, pkg->deps, node);

	uint *data = list_get_data(&pkgs->deps, node);
	if (data == NULL) {
		log_error("build", "pkgs", NULL, "failed to add package dependecy");
		return 1;
	}

	*data = dep_id;

	return 0;
}

static int add_unique_pkg(arr_t *arr, const uint *pkg)
{
	if (arr_index(arr, pkg) < arr->cnt) {
		return 0;
	}

	uint *data = arr_add(arr);
	if (data == NULL) {
		return 1;
	}

	*data = *pkg;
	return 0;
}

static int get_pkg_deps(const pkgs_t *pkgs, uint id, arr_t *arr)
{
	int ret = 0;

	const pkg_t *pkg = arr_get(&pkgs->pkgs, id);

	const uint *dep;
	list_foreach(&pkgs->deps, pkg->deps, dep)
	{
		get_pkg_deps(pkgs, *dep, arr);
		ret |= add_unique_pkg(arr, dep);
	}

	return ret;
}

int pkgs_get_pkg_deps(const pkgs_t *pkgs, uint pkg, arr_t *deps)
{
	if (pkgs == NULL || deps == NULL) {
		return 1;
	}

	deps->cnt = 0;

	return get_pkg_deps(pkgs, pkg, deps);
}

int pkgs_get_build_order(const pkgs_t *pkgs, arr_t *order)
{
	if (pkgs == NULL || order == NULL) {
		return 1;
	}

	int ret = 0;
	for (uint i = 0; i < pkgs->pkgs.cnt; i++) {
		ret |= get_pkg_deps(pkgs, i, order);
		ret |= add_unique_pkg(order, &i);
	}

	return ret;
}

int pkgs_print(const pkgs_t *pkgs, print_dst_t dst)
{
	int off = dst.off;

	const pkg_t *pkg;
	arr_foreach(&pkgs->pkgs, pkg)
	{
		dst.off += pkg_print(pkg, dst);

		dst.off += c_dprintf(dst, "DEPS:");
		const uint *dep;
		list_foreach(&pkgs->deps, pkg->deps, dep)
		{
			strv_t name = strbuf_get(&pkgs->names, *dep);
			dst.off += c_dprintf(dst, " %.*s", name.len, name.data);
		}
		dst.off += c_dprintf(dst, "\n");
		dst.off += c_dprintf(dst, "\n");
	}

	return dst.off - off;
}
