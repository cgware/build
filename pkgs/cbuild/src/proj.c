#include "proj.h"

#include "log.h"

proj_t *proj_init(proj_t *proj, uint pkgs_cap, uint targets_cap, alloc_t alloc)
{
	if (proj == NULL) {
		return NULL;
	}

	if (strbuf_init(&proj->strs, 2 + __PKG_STR_CNT * pkgs_cap + __TARGET_STR_CNT * targets_cap, 16, alloc) == NULL ||
	    arr_init(&proj->pkgs, pkgs_cap, sizeof(pkg_t), alloc) == NULL ||
	    list_init(&proj->targets, targets_cap, sizeof(target_t), alloc) == NULL ||
	    list_init(&proj->deps, targets_cap, sizeof(list_node_t), alloc) == NULL) {
		return NULL;
	}

	strbuf_add(&proj->strs, STRV_NULL, &proj->name);
	strbuf_add(&proj->strs, STRV_NULL, &proj->outdir);

	return proj;
}

void proj_free(proj_t *proj)
{
	if (proj == NULL) {
		return;
	}

	list_free(&proj->deps);
	list_free(&proj->targets);
	arr_free(&proj->pkgs);
	strbuf_free(&proj->strs);
}

pkg_t *proj_add_pkg(proj_t *proj, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	uint tmp;
	pkg_t *pkg = arr_add(&proj->pkgs, &tmp);
	if (pkg == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add package");
		return NULL;
	}

	uint strs_cnt = proj->strs.off.cnt;

	for (uint i = 0; i < __PKG_STR_CNT; i++) {
		if (strbuf_add(&proj->strs, STRV_NULL, NULL)) {
			arr_reset(&proj->pkgs, tmp);
			strbuf_reset(&proj->strs, strs_cnt);
			log_error("cbuild", "proj", NULL, "failed to add package strings");
			return NULL;
		}
	}

	pkg->strs	 = strs_cnt;
	pkg->has_targets = 0;

	id ? *id = tmp : (uint)0;

	return pkg;
}

pkg_t *proj_get_pkg(const proj_t *proj, uint id)
{
	if (proj == NULL) {
		return NULL;
	}

	pkg_t *pkg = arr_get(&proj->pkgs, id);
	if (pkg == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get package");
		return NULL;
	}

	return pkg;
}

pkg_t *proj_find_pkg(const proj_t *proj, strv_t name, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	uint i = 0;
	pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		if (strv_eq(proj_get_str(proj, pkg->strs + PKG_NAME), name)) {
			id ? *id = i : (uint)0;
			return pkg;
		}
	}

	return NULL;
}

target_t *proj_add_target(proj_t *proj, uint pkg, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	pkg_t *p = proj_get_pkg(proj, pkg);
	if (p == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add target to the package: %d", pkg);
		return NULL;
	}

	list_node_t tmp;
	target_t *target = list_node(&proj->targets, &tmp);
	if (target == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add target");
		return NULL;
	}

	uint strs_cnt = proj->strs.off.cnt;

	for (uint i = 0; i < __TARGET_STR_CNT; i++) {
		if (strbuf_add(&proj->strs, STRV_NULL, NULL)) {
			list_reset(&proj->targets, tmp);
			strbuf_reset(&proj->strs, strs_cnt);
			log_error("cbuild", "proj", NULL, "failed to add target strings");
			return NULL;
		}
	}

	if (p->has_targets) {
		list_app(&proj->targets, p->targets, tmp);
	} else {
		p->targets     = tmp;
		p->has_targets = 1;
	}

	target->type	 = TARGET_TYPE_UNKNOWN;
	target->strs	 = strs_cnt;
	target->has_deps = 0;
	target->pkg	 = pkg;

	id ? *id = tmp : (uint)0;

	return target;
}

target_t *proj_get_target(const proj_t *proj, uint id)
{
	if (proj == NULL) {
		return NULL;
	}

	target_t *target = list_get(&proj->targets, id);
	if (target == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get target");
		return NULL;
	}

	return target;
}

target_t *proj_find_target(const proj_t *proj, uint pkg, strv_t name, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	const pkg_t *p = arr_get(&proj->pkgs, pkg);
	if (p == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get package");
		return NULL;
	}

	if (!p->has_targets) {
		return NULL;
	}

	target_t *target;
	list_node_t i = p->targets;
	list_foreach(&proj->targets, i, target)
	{
		if (strv_eq(proj_get_str(proj, target->strs + TARGET_NAME), name)) {
			id ? *id = i : (uint)0;
			return target;
		}
	}

	return NULL;
}

int proj_set_str(proj_t *proj, uint id, strv_t val)
{
	if (proj == NULL) {
		return 1;
	}

	if (strbuf_set(&proj->strs, id, val)) {
		log_error("cbuild", "proj", NULL, "failed to set string: %d: '%.*s'", id, val.len, val.data);
		return 1;
	}

	return 0;
}

strv_t proj_get_str(const proj_t *proj, uint id)
{
	if (proj == NULL) {
		return STRV_NULL;
	}

	return strbuf_get(&proj->strs, id);
}

int proj_add_dep(proj_t *proj, uint target, uint dep)
{
	if (proj == NULL) {
		return 1;
	}

	target_t *t = proj_get_target(proj, target);
	if (t == NULL || proj_get_target(proj, dep) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add dependency: %d -> %d", target, dep);
		return 1;
	}

	list_node_t node;
	uint *data = list_node(&proj->deps, &node);
	if (data == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add dependency");
		return 1;
	}

	*data = dep;
	if (t->has_deps) {
		list_app(&proj->deps, t->deps, node);
	} else {
		t->deps	    = node;
		t->has_deps = 1;
	}

	return 0;
}

int proj_get_deps(const proj_t *proj, list_node_t target, arr_t *deps)
{
	if (proj == NULL) {
		return 1;
	}

	if (proj_get_target(proj, target) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get target dependencies: %d", target);
		return 1;
	}

	if (proj->deps.cnt == 0) {
		return 0;
	}

	arr_t visited = {0};
	arr_init(&visited, proj->targets.cnt, sizeof(uint8_t), ALLOC_STD);
	for (uint i = 0; i < proj->targets.cnt; i++) {
		*(uint8_t *)arr_add(&visited, NULL) = 0;
	}

	arr_t queue = {0};
	arr_init(&queue, proj->deps.cnt * 2, sizeof(list_node_t), ALLOC_STD);
	*(list_node_t *)arr_add(&queue, NULL) = target;

	uint front = 0;

	while (front < queue.cnt) {
		list_node_t current = *(list_node_t *)arr_get(&queue, front++);
		uint8_t *v	    = arr_get(&visited, current);

		if (*v) {
			continue;
		}
		*v = 1;

		if (current != target) {
			arr_addu(deps, &current, NULL);
		}

		const target_t *tgt = list_get(&proj->targets, current);
		if (!tgt->has_deps) {
			continue;
		}

		const list_node_t *dep_target_id;
		list_node_t i = tgt->deps;
		list_foreach(&proj->deps, i, dep_target_id)
		{
			uint8_t *visited_to = arr_get(&visited, *dep_target_id);
			if (!*visited_to) {
				*(uint *)arr_add(&queue, NULL) = *dep_target_id;
			}
		}
	}

	arr_free(&visited);
	arr_free(&queue);
	return 0;
}

int proj_get_pkg_build_order(const proj_t *proj, arr_t *order, alloc_t alloc)
{
	if (proj == NULL || order == NULL) {
		return 1;
	}

	enum {
		UNVISITED,
		VISITING,
		DONE
	};

	arr_t stack = {0};
	arr_init(&stack, proj->targets.cnt, sizeof(list_node_t), alloc);

	pkg_t *pkg;
	uint i = 0;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		pkg->state			      = UNVISITED;
		*(list_node_t *)arr_add(&stack, NULL) = i;
	}

	while (stack.cnt > 0) {
		list_node_t *pkg_id = arr_get(&stack, stack.cnt - 1);
		pkg		    = arr_get(&proj->pkgs, *pkg_id);

		if (pkg->state == VISITING) {
			stack.cnt--;
			arr_addu(order, pkg_id, NULL);
			pkg->state = DONE;
			continue;
		}

		pkg->state = VISITING;

		const pkg_t *pkg = arr_get(&proj->pkgs, *pkg_id);
		const target_t *target;
		list_node_t targets = pkg->targets;
		list_foreach(&proj->targets, targets, target)
		{
			if (!target->has_deps) {
				continue;
			}

			list_node_t deps = target->deps;
			list_node_t *dep;
			list_foreach(&proj->deps, deps, dep)
			{
				const target_t *dep_target = list_get(&proj->targets, *dep);
				const pkg_t *dep_pkg	   = arr_get(&proj->pkgs, dep_target->pkg);

				switch (dep_pkg->state) {
				case VISITING:
					if (*pkg_id == dep_target->pkg) {
						break;
					}
					log_error("cbuild", "proj", NULL, "failed to get package build order: cycle detected");
					arr_free(&stack);
					return 1;
				case UNVISITED:
					*(list_node_t *)arr_add(&stack, NULL) = dep_target->pkg;
				default:
					break;
				}
			}
		}
	}

	arr_free(&stack);

	return 0;
}

static const char *target_type_str[] = {
	[TARGET_TYPE_UNKNOWN] = "UNKNOWN",
	[TARGET_TYPE_EXE]     = "EXE",
	[TARGET_TYPE_LIB]     = "LIB",
	[TARGET_TYPE_EXT]     = "EXT",
	[TARGET_TYPE_TST]     = "TEST",
};

size_t proj_print(const proj_t *proj, dst_t dst)
{
	if (proj == NULL) {
		return 0;
	}

	size_t off = dst.off;

	strv_t name   = proj_get_str(proj, proj->name);
	strv_t outdir = proj_get_str(proj, proj->outdir);

	dst.off += dputf(dst,
			 "[project]\n"
			 "NAME: %.*s\n"
			 "OUTDIR: %.*s\n",
			 name.len,
			 name.data,
			 outdir.len,
			 outdir.data);

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		dst.off += dputf(dst, "\n[pkg]\n");

		strv_t pkg_name = proj_get_str(proj, pkg->strs + PKG_NAME);
		strv_t path	= proj_get_str(proj, pkg->strs + PKG_PATH);
		strv_t src	= proj_get_str(proj, pkg->strs + PKG_SRC);
		strv_t inc	= proj_get_str(proj, pkg->strs + PKG_INC);
		strv_t test	= proj_get_str(proj, pkg->strs + PKG_TST);
		strv_t uri_str	= proj_get_str(proj, pkg->strs + PKG_URI_STR);
		strv_t uri_name = proj_get_str(proj, pkg->strs + PKG_URI_NAME);
		strv_t uri_dir	= proj_get_str(proj, pkg->strs + PKG_URI_DIR);

		dst.off += dputf(dst,
				 "NAME: %.*s\n"
				 "PATH: %.*s\n"
				 "SRC: %.*s\n"
				 "INC: %.*s\n"
				 "TEST: %.*s\n"
				 "URI_STR: %.*s\n"
				 "URI_NAME: %.*s\n"
				 "URI_DIR: %.*s\n",
				 pkg_name.len,
				 pkg_name.data,
				 path.len,
				 path.data,
				 src.len,
				 src.data,
				 inc.len,
				 inc.data,
				 test.len,
				 test.data,
				 uri_str.len,
				 uri_str.data,
				 uri_name.len,
				 uri_name.data,
				 uri_dir.len,
				 uri_dir.data);

		if (pkg->has_targets) {
			const target_t *target;
			list_node_t j = pkg->targets;
			list_foreach(&proj->targets, j, target)
			{
				dst.off += dputf(dst, "\n[target]\n");
				strv_t target_name = proj_get_str(proj, target->strs + TARGET_NAME);
				strv_t cmd	   = proj_get_str(proj, target->strs + TARGET_CMD);
				strv_t out	   = proj_get_str(proj, target->strs + TARGET_OUT);
				dst.off += dputf(dst,
						 "NAME: %.*s\n"
						 "TYPE: %s\n"
						 "CMD: %.*s\n"
						 "OUT: %.*s\n",
						 target_name.len,
						 target_name.data,
						 target_type_str[target->type],
						 cmd.len,
						 cmd.data,
						 out.len,
						 out.data);
				dst.off += dputf(dst, "DEPS:");

				if (target->has_deps) {
					const list_node_t *dep_target_id;
					list_node_t j = target->deps;
					list_foreach(&proj->deps, j, dep_target_id)
					{
						const target_t *dep_tgt = list_get(&proj->targets, *dep_target_id);
						const pkg_t *dep_pkg	= arr_get(&proj->pkgs, dep_tgt->pkg);

						strv_t dep_tgt_name = proj_get_str(proj, dep_tgt->strs + PKG_NAME);
						strv_t dep_pkg_name = proj_get_str(proj, dep_pkg->strs + TARGET_NAME);
						dst.off += dputf(dst,
								 " %.*s:%.*s",
								 dep_pkg_name.len,
								 dep_pkg_name.data,
								 dep_tgt_name.len,
								 dep_tgt_name.data);
					}
				}
				dst.off += dputf(dst, "\n");
			}
		}
	}

	return dst.off - off;
}
