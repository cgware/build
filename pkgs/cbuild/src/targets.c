#include "targets.h"

#include "log.h"

targets_t *targets_init(targets_t *targets, uint targets_cap, alloc_t alloc)
{
	if (targets == NULL) {
		return NULL;
	}

	if (strbuf_init(&targets->names, targets_cap, targets_cap * 8, alloc) == NULL ||
	    strbuf_init(&targets->files, targets_cap, targets_cap * 8, alloc) == NULL ||
	    list_init(&targets->targets, targets_cap, sizeof(target_t), alloc) == NULL ||
	    list_init(&targets->deps, targets_cap, sizeof(list_node_t), alloc) == NULL) {
		log_error("build", "targets", NULL, "failed to initialize targets");
		return NULL;
	}

	targets->alloc = alloc;

	return targets;
}

void targets_free(targets_t *targets)
{
	if (targets == NULL) {
		return;
	}

	list_free(&targets->deps);
	target_t *target;
	list_node_t i = 0;
	list_foreach_all(&targets->targets, i, target)
	{
		target_free(target);
	}

	list_free(&targets->targets);
	strbuf_free(&targets->files);
	strbuf_free(&targets->names);
}

target_t *targets_target(targets_t *targets, strv_t name, list_node_t *id)
{
	if (targets == NULL) {
		return NULL;
	}

	uint index;
	if (strbuf_find(&targets->names, name, &index) == 0) {
		if (id) {
			*id = index;
		}
		return targets_get(targets, index);
	}

	if (strbuf_add(&targets->names, name, &index)) {
		return NULL;
	}

	if (strbuf_add(&targets->files, STRV(""), NULL)) {
		return NULL;
	}

	list_node_t node;
	target_t *target = list_node(&targets->targets, &node);
	if (target == NULL) {
		log_error("cbuild", "targets", NULL, "failed to create target");
		return NULL;
	}

	if (id) {
		*id = index;
	}

	return target_init(target);
}

int targets_app(targets_t *targets, list_node_t list, list_node_t id)
{
	if (targets == NULL) {
		return 1;
	}

	if (list_app(&targets->targets, list, id)) {
		log_error("cbuild", "targets", NULL, "failed to add target");
		return 1;
	}

	return 0;
}

target_t *targets_get(const targets_t *targets, list_node_t id)
{
	if (targets == NULL) {
		return NULL;
	}

	target_t *target = list_get(&targets->targets, id);
	if (target == NULL) {
		log_error("cbuild", "targets", NULL, "failed to get target");
		return NULL;
	}

	return target;
}

target_t *targets_add_dep(targets_t *targets, list_node_t id, strv_t dep)
{
	if (targets == NULL) {
		return NULL;
	}

	uint dep_id;
	if (strbuf_find(&targets->names, dep, &dep_id) && targets_target(targets, dep, &dep_id) == NULL) {
		return NULL;
	}

	target_t *target = targets_get(targets, id);
	if (target == NULL) {
		log_error("cbuild", "targets", NULL, "target not found: %d", id);
		return NULL;
	}

	list_node_t node;
	uint *data = list_node(&targets->deps, &node);
	if (data == NULL) {
		log_error("cbuild", "targets", NULL, "failed to create target dependency");
		return NULL;
	}

	if (target->has_deps) {
		if (list_app(&targets->deps, target->deps, node)) {
			log_error("cbuild", "targets", NULL, "failed to add target dependency");
			return NULL;
		}
	} else {
		target->deps	 = node;
		target->has_deps = 1;
	}

	*data = dep_id;

	return target;
}

static int get_target_deps(const targets_t *targets, list_node_t id, arr_t *arr)
{
	const target_t *target = targets_get(targets, id);
	if (target == NULL) {
		log_error("cbuild", "targets", NULL, "failed to get target deps");
		return 1;
	}

	if (!target->has_deps) {
		return 0;
	}

	int ret = 0;

	const uint *dep;
	list_node_t i = target->deps;
	list_foreach(&targets->deps, i, dep)
	{
		get_target_deps(targets, *dep, arr);
		ret |= arr_addu(arr, dep, NULL);
	}

	return ret;
}

int targets_get_deps(const targets_t *targets, list_node_t target, arr_t *deps)
{
	if (targets == NULL || deps == NULL) {
		return 1;
	}

	deps->cnt = 0;

	return get_target_deps(targets, target, deps);
}

int targets_get_build_order(const targets_t *targets, arr_t *order)
{
	if (targets == NULL || order == NULL) {
		return 1;
	}

	int ret = 0;
	for (uint i = 0; i < targets->targets.cnt; i++) {
		ret |= get_target_deps(targets, i, order);
		ret |= arr_addu(order, &i, NULL);
	}

	return ret;
}

size_t targets_print(const targets_t *targets, list_node_t start, dst_t dst)
{
	size_t off = dst.off;

	const target_t *target;
	uint i = start;
	list_foreach(&targets->targets, i, target)
	{
		dst.off += target_print(target, dst);

		strv_t name = strbuf_get(&targets->names, i);
		strv_t file = strbuf_get(&targets->files, i);

		dst.off += dputf(dst,
				 "NAME: %.*s\n"
				 "FILE: %.*s\n",
				 name.len,
				 name.data,
				 file.len,
				 file.data);

		dst.off += dputs(dst, STRV("DEPS:"));
		if (target->has_deps) {
			const uint *dep;
			list_node_t j = target->deps;
			list_foreach(&targets->deps, j, dep)
			{
				dst.off += dputs(dst, STRV(" "));
				dst.off += dputs(dst, strbuf_get(&targets->names, *dep));
			}
		}
		dst.off += dputs(dst, STRV("\n"));
		dst.off += dputs(dst, STRV("\n"));
	}

	return dst.off - off;
}
