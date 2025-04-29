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
	    list_init(&targets->deps, targets_cap, sizeof(lnode_t), alloc) == NULL) {
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
	list_foreach_all(&targets->targets, target)
	{
		target_free(target);
	}

	list_free(&targets->targets);
	strbuf_free(&targets->files);
	strbuf_free(&targets->names);
}

target_t *targets_add(targets_t *targets, lnode_t *list, strv_t name, lnode_t *id)
{
	if (targets == NULL || list == NULL) {
		return NULL;
	}

	uint index;
	if (strbuf_find(&targets->names, name, &index) == 0) {
		if (id) {
			*id = index;
		}
		list_set_next_node(&targets->targets, *list, index);
		return list_get_data(&targets->targets, index);
	}

	if (strbuf_add(&targets->names, name, &index)) {
		return NULL;
	}

	if (strbuf_add(&targets->files, STRV(""), NULL)) {
		return NULL;
	}

	lnode_t node;
	list_add_next_node(&targets->targets, *list, node);

	if (id) {
		*id = index;
	}

	return target_init(list_get_data(&targets->targets, node));
}

target_t *targets_get(const targets_t *targets, lnode_t id)
{
	if (targets == NULL) {
		return NULL;
	}

	return list_get_data(&targets->targets, id);
}

target_t *targets_add_dep(targets_t *targets, lnode_t id, strv_t dep)
{
	if (targets == NULL) {
		return NULL;
	}

	uint dep_id;
	lnode_t list = LIST_END;
	if (strbuf_find(&targets->names, dep, &dep_id) && targets_add(targets, &list, dep, &dep_id) == NULL) {
		return NULL;
	}

	target_t *target = list_get_data(&targets->targets, id);
	if (target == NULL) {
		log_error("build", "targets", NULL, "target not found: %d", id);
		return NULL;
	}

	lnode_t node;
	list_add_next_node(&targets->deps, target->deps, node);

	uint *data = list_get_data(&targets->deps, node);
	if (data == NULL) {
		log_error("build", "targets", NULL, "failed to add target dependency");
		return NULL;
	}

	*data = dep_id;

	return target;
}

static int get_target_deps(const targets_t *targets, lnode_t id, arr_t *arr)
{
	const target_t *target = list_get_data(&targets->targets, id);
	if (target == NULL) {
		return 1;
	}

	int ret = 0;

	const uint *dep;
	list_foreach(&targets->deps, target->deps, dep)
	{
		get_target_deps(targets, *dep, arr);
		ret |= arr_addu(arr, dep);
	}

	return ret;
}

int targets_get_deps(const targets_t *targets, lnode_t target, arr_t *deps)
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
		ret |= arr_addu(order, &i);
	}

	return ret;
}

int targets_print(const targets_t *targets, lnode_t start, print_dst_t dst)
{
	int off = dst.off;

	const target_t *target;
	list_foreach(&targets->targets, start, target)
	{
		dst.off += target_print(target, dst);

		strv_t name = strbuf_get(&targets->names, _i);
		strv_t file = strbuf_get(&targets->files, _i);

		dst.off += c_dprintf(dst,
				     "NAME: %.*s\n"
				     "FILE: %.*s\n",
				     name.len,
				     name.data,
				     file.len,
				     file.data);

		dst.off += c_dprintf(dst, "DEPS:");
		const uint *dep;
		list_foreach(&targets->deps, target->deps, dep)
		{
			strv_t name = strbuf_get(&targets->names, *dep);
			dst.off += c_dprintf(dst, " %.*s", name.len, name.data);
		}
		dst.off += c_dprintf(dst, "\n");
		dst.off += c_dprintf(dst, "\n");
	}

	return dst.off - off;
}
