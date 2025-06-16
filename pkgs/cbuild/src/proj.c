#include "proj.h"

#include "log.h"

typedef struct dep_s {
	uint from;
	uint to;
} dep_t;

proj_t *proj_init(proj_t *proj, uint pkgs_cap, uint targets_cap, alloc_t alloc)
{
	if (proj == NULL) {
		return NULL;
	}

	if (arr_init(&proj->pkgs, pkgs_cap, sizeof(pkg_t), alloc) == NULL ||
	    arr_init(&proj->targets, targets_cap, sizeof(target_t), alloc) == NULL ||
	    strbuf_init(&proj->strs, pkgs_cap * __PKG_STR_CNT + targets_cap * __TARGET_STR_CNT, 16, alloc) == NULL ||
	    arr_init(&proj->deps, targets_cap, sizeof(dep_t), alloc) == NULL) {
		log_error("build", "proj", NULL, "failed to initialize project");
		return NULL;
	}

	proj->alloc = alloc;

	return proj;
}

void proj_free(proj_t *proj)
{
	if (proj == NULL) {
		return;
	}

	arr_free(&proj->targets);
	arr_free(&proj->pkgs);
	strbuf_free(&proj->strs);
	arr_free(&proj->deps);
}

pkg_t *proj_add_pkg(proj_t *proj, strv_t name, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	uint pkgs_cnt = proj->pkgs.cnt;

	pkg_t *pkg = arr_add(&proj->pkgs, id);
	if (pkg == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add package");
		return NULL;
	}

	uint strs_cnt = proj->strs.off.cnt;

	for (uint i = 0; i < __PKG_STR_CNT; i++) {
		if (strbuf_add(&proj->strs, STRV_NULL, NULL)) {
			arr_reset(&proj->pkgs, pkgs_cnt);
			strbuf_reset(&proj->strs, strs_cnt);
			log_error("cbuild", "proj", NULL, "failed to add package strings");
			return NULL;
		}
	}

	pkg->strs   = strs_cnt;
	pkg->inited = 0;
	proj_set_str(proj, pkg->strs + PKG_NAME, name);

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

	pkg_t *pkg;
	uint i = 0;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		if (strv_eq(strbuf_get(&proj->strs, pkg->strs + PKG_NAME), name)) {
			id ? *id = i : (uint)0;
			return pkg;
		}
	}

	return NULL;
}

target_t *proj_add_target(proj_t *proj, uint pkg, strv_t name, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	if (proj_get_pkg(proj, pkg) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add target to the package: %d", pkg);
		return NULL;
	}

	uint targets_cnt = proj->targets.cnt;

	uint tmp;
	target_t *target = arr_add(&proj->targets, &tmp);
	if (target == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add target");
		return NULL;
	}

	uint strs_cnt = proj->strs.off.cnt;

	for (uint i = 0; i < __TARGET_STR_CNT; i++) {
		if (strbuf_add(&proj->strs, STRV_NULL, NULL)) {
			arr_reset(&proj->targets, targets_cnt);
			strbuf_reset(&proj->strs, strs_cnt);
			log_error("cbuild", "proj", NULL, "failed to add target strings");
			return NULL;
		}
	}

	target->pkg    = pkg;
	target->type   = TARGET_TYPE_UNKNOWN;
	target->strs   = strs_cnt;
	target->inited = 0;
	proj_set_str(proj, target->strs + TARGET_NAME, name);

	id ? *id = tmp : (uint)0;

	return target;
}

target_t *proj_get_target(const proj_t *proj, uint id)
{
	if (proj == NULL) {
		return NULL;
	}

	target_t *target = arr_get(&proj->targets, id);
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

	if (proj_get_pkg(proj, pkg) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to find target in the package: %d", pkg);
		return NULL;
	}

	target_t *target;
	uint i = 0;
	arr_foreach(&proj->targets, i, target)
	{
		if (target->pkg != pkg) {
			continue;
		}

		if (strv_eq(strbuf_get(&proj->strs, target->strs + TARGET_NAME), name)) {
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

	if (proj_get_target(proj, target) == NULL || proj_get_target(proj, dep) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add dependency: %d -> %d", target, dep);
		return 1;
	}

	dep_t *d = arr_add(&proj->deps, NULL);
	if (d == NULL) {
		log_error("cbuild", "proj", NULL, "failed to add dependency");
		return 1;
	}

	*d = (dep_t){
		.from = dep,
		.to   = target,
	};

	return 0;
}

int proj_get_deps(const proj_t *proj, uint target, arr_t *deps)
{
	if (proj == NULL) {
		return 1;
	}

	if (proj_get_target(proj, target) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get target dependencies: %d", target);
		return 1;
	}

	arr_t visited = {0};
	arr_init(&visited, proj->targets.cnt, sizeof(uint8_t), ALLOC_STD);
	for (uint i = 0; i < proj->targets.cnt; i++) {
		*(uint8_t *)arr_add(&visited, NULL) = 0;
	}

	arr_t stack = {0};
	arr_init(&stack, proj->targets.cnt, sizeof(uint), ALLOC_STD);
	*(uint *)arr_add(&stack, NULL) = target;

	while (stack.cnt > 0) {
		uint current = *(uint *)arr_get(&stack, stack.cnt - 1);
		stack.cnt--;
		uint cnt   = stack.cnt;
		uint8_t *v = arr_get(&visited, current);

		if (*v) {
			continue;
		}
		*v = 1;

		for (uint i = 0; i < proj->deps.cnt; i++) {
			dep_t *dep = arr_get(&proj->deps, i);
			if (dep->to == current) {
				uint from	      = dep->from;
				uint8_t *visited_from = arr_get(&visited, from);
				if (!*visited_from) {
					*(uint *)arr_add(&stack, NULL) = from;
				}
			}
		}

		if (stack.cnt == cnt && current != target) {
			arr_addu(deps, &current, NULL);
		}
	}

	arr_free(&visited);
	arr_free(&stack);

	return 0;
}

size_t proj_print_deps(const proj_t *proj, dst_t dst)
{
	if (proj == NULL) {
		return 0;
	}

	size_t off = dst.off;

	const pkg_t *pkg;
	uint pkg_id = 0;
	arr_foreach(&proj->pkgs, pkg_id, pkg)
	{
		dst.off += dputs(dst, strbuf_get(&proj->strs, pkg->strs + PKG_NAME));
		dst.off += dputs(dst, STRV("\n"));

		const target_t *target;
		uint target_id = 0;
		arr_foreach(&proj->targets, target_id, target)
		{
			if (target->pkg != pkg_id) {
				continue;
			}

			dst.off += dputs(dst, STRV("  "));
			dst.off += dputs(dst, strbuf_get(&proj->strs, target->strs + TARGET_NAME));
			dst.off += dputs(dst, STRV(":"));

			const dep_t *dep;
			uint i = 0;
			arr_foreach(&proj->deps, i, dep)
			{
				if (dep->to != target_id) {
					continue;
				}

				const target_t *dtarget = proj_get_target(proj, dep->from);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);

				dst.off += dputs(dst, STRV(" "));
				dst.off += dputs(dst, strbuf_get(&proj->strs, dpkg->strs + PKG_NAME));
				dst.off += dputs(dst, STRV(":"));
				dst.off += dputs(dst, strbuf_get(&proj->strs, dtarget->strs + TARGET_NAME));
			}
			dst.off += dputs(dst, STRV("\n"));
		}
	}

	return dst.off - off;
}

int proj_get_pkg_build_order(const proj_t *proj, arr_t *order, alloc_t alloc)
{
	if (proj == NULL || order == NULL) {
		return 1;
	}

	arr_t indegrees = {0};
	arr_init(&indegrees, proj->targets.cnt, sizeof(uint), alloc);

	uint i;
	for (i = 0; i < proj->targets.cnt; i++) {
		*(uint *)arr_add(&indegrees, NULL) = 0;
	}

	uint *indegree;
	dep_t *dep;
	i = 0;
	arr_foreach(&proj->deps, i, dep)
	{
		indegree = arr_get(&indegrees, dep->to);
		(*indegree)++;
	}

	arr_t queue = {0};
	arr_init(&queue, proj->targets.cnt, sizeof(uint), alloc);

	i = 0;
	arr_foreach(&indegrees, i, indegree)
	{
		if (*indegree == 0) {
			*(uint *)arr_add(&queue, NULL) = i;
		}
	}

	uint front = 0;

	order->cnt = 0;
	uint cnt   = 0;

	while (front < queue.cnt) {
		uint node = *(uint *)arr_get(&queue, front++);

		target_t *target = proj_get_target(proj, node);
		arr_addu(order, &target->pkg, NULL);
		cnt++;

		i = 0;
		arr_foreach(&proj->deps, i, dep)
		{
			if (dep->from == node) {
				uint to	 = dep->to;
				indegree = arr_get(&indegrees, to);
				if (--*indegree == 0) {
					*(uint *)arr_add(&queue, NULL) = to;
				}
			}
		}
	}

	arr_free(&queue);
	arr_free(&indegrees);

	if (cnt != proj->targets.cnt) {
		log_error("cbuild", "proj", NULL, "failed to get package build order: cycle detected");
		return 1;
	}

	return 0;
}

int proj_get_target_build_order(const proj_t *proj, arr_t *order, alloc_t alloc)
{
	if (proj == NULL || order == NULL) {
		return 1;
	}

	arr_t indegrees = {0};
	arr_init(&indegrees, proj->targets.cnt, sizeof(uint), alloc);

	uint i;
	for (i = 0; i < proj->targets.cnt; i++) {
		*(uint *)arr_add(&indegrees, NULL) = 0;
	}

	uint *indegree;
	dep_t *dep;
	i = 0;
	arr_foreach(&proj->deps, i, dep)
	{
		indegree = arr_get(&indegrees, dep->to);
		(*indegree)++;
	}

	arr_t queue = {0};
	arr_init(&queue, proj->targets.cnt, sizeof(uint), alloc);

	i = 0;
	arr_foreach(&indegrees, i, indegree)
	{
		if (*indegree == 0) {
			*(uint *)arr_add(&queue, NULL) = i;
		}
	}

	uint front = 0;

	order->cnt = 0;

	while (front < queue.cnt) {
		uint node = *(uint *)arr_get(&queue, front++);

		*(uint *)arr_add(order, NULL) = node;

		i = 0;
		arr_foreach(&proj->deps, i, dep)
		{
			if (dep->from == node) {
				uint to	 = dep->to;
				indegree = arr_get(&indegrees, to);
				if (--*indegree == 0) {
					*(uint *)arr_add(&queue, NULL) = to;
				}
			}
		}
	}

	arr_free(&queue);
	arr_free(&indegrees);

	if (order->cnt != proj->targets.cnt) {
		log_error("cbuild", "proj", NULL, "failed to get target build order: cycle detected");
		return 1;
	}

	return 0;
}

static const char *target_type_str[] = {
	[TARGET_TYPE_UNKNOWN] = "UNKNOWN",
	[TARGET_TYPE_EXE]     = "EXE",
	[TARGET_TYPE_LIB]     = "LIB",
};

size_t proj_print(const proj_t *proj, dst_t dst)
{
	if (proj == NULL) {
		return 0;
	}

	size_t off = dst.off;

	dst.off += dputf(dst,
			 "[project]\n"
			 "DIR: %.*s\n"
			 "OUTDIR: %.*s\n"
			 "\n",
			 proj->dir.len,
			 proj->dir.data,
			 proj->outdir.len,
			 proj->outdir.data);

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		strv_t dir  = proj_get_str(proj, pkg->strs + PKG_DIR);
		strv_t name = proj_get_str(proj, pkg->strs + PKG_NAME);
		strv_t url  = proj_get_str(proj, pkg->strs + PKG_URL);
		strv_t src  = proj_get_str(proj, pkg->strs + PKG_SRC);
		strv_t inc  = proj_get_str(proj, pkg->strs + PKG_INC);
		dst.off += dputf(dst,
				 "[package]\n"
				 "NAME: %.*s\n"
				 "URL: %.*s\n"
				 "DIR: %.*s\n"
				 "SRC: %.*s\n"
				 "INC: %.*s\n",
				 name.len,
				 name.data,
				 url.len,
				 url.data,
				 dir.len,
				 dir.data,
				 src.len,
				 src.data,
				 inc.len,
				 inc.data);

		dst.off += dputs(dst, STRV("\n"));

		uint j = 0;
		const target_t *target;
		arr_foreach(&proj->targets, j, target)
		{
			if (target->pkg != i) {
				continue;
			}

			strv_t name = proj_get_str(proj, target->strs + TARGET_NAME);
			strv_t file = proj_get_str(proj, target->strs + TARGET_FILE);

			dst.off += dputf(dst,
					 "[target]\n"
					 "TYPE: %s\n"
					 "NAME: %.*s\n"
					 "FILE: %.*s\n",
					 target_type_str[target->type],
					 name.len,
					 name.data,
					 file.len,
					 file.data);

			dst.off += dputs(dst, STRV("DEPS:"));

			uint k = 0;
			dep_t *dep;
			arr_foreach(&proj->deps, k, dep)
			{
				if (dep->to != j) {
					continue;
				}

				const target_t *dtarget = proj_get_target(proj, dep->from);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
				dst.off += dputs(dst, STRV(" "));
				dst.off += dputs(dst, proj_get_str(proj, dpkg->strs + TARGET_NAME));
				dst.off += dputs(dst, STRV(":"));
				dst.off += dputs(dst, proj_get_str(proj, dtarget->strs + TARGET_NAME));
			}
			dst.off += dputs(dst, STRV("\n"));
			dst.off += dputs(dst, STRV("\n"));
		}
	}

	return dst.off - off;
}
