#include "proj_graph.h"

#include "log.h"
#include "mem.h"

typedef struct proj_graph_s {
	uint cnt;
	arr_t offsets;
	arr_t edges;
} proj_graph_t;

typedef struct proj_graph_frame_s {
	uint node;
	uint next;
} proj_graph_frame_t;

typedef strv_t (*proj_graph_name_cb)(const proj_t *proj, uint node);

static uint proj_graph_edge_begin(const proj_graph_t *graph, uint node)
{
	return *(uint *)arr_get(&graph->offsets, node);
}

static uint proj_graph_edge_end(const proj_graph_t *graph, uint node)
{
	return *(uint *)arr_get(&graph->offsets, node + 1);
}

static uint proj_graph_edge_get(const proj_graph_t *graph, uint edge)
{
	return *(uint *)arr_get(&graph->edges, edge);
}

static void proj_graph_free(proj_graph_t *graph)
{
	arr_free(&graph->offsets);
	arr_free(&graph->edges);
}

static int proj_graph_init_offsets(proj_graph_t *graph, uint cnt, alloc_t alloc)
{
	graph->cnt = cnt;

	if (arr_init(&graph->offsets, cnt + 1, sizeof(uint), alloc) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to initialize graph");
		return 1;
	}

	graph->offsets.cnt = cnt + 1;
	mem_set(graph->offsets.data, 0, graph->offsets.size * graph->offsets.cnt);

	return 0;
}

static int proj_graph_init_edges(proj_graph_t *graph, uint edges_cap, alloc_t alloc)
{
	if (arr_init(&graph->edges, edges_cap > 0 ? edges_cap : 1, sizeof(uint), alloc) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to initialize graph");
		return 1;
	}

	return 0;
}

static int proj_graph_marks_init(arr_t *marks, uint cnt, alloc_t alloc)
{
	if (arr_init(marks, cnt > 0 ? cnt : 1, sizeof(uint), alloc) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to initialize graph");
		return 1;
	}

	marks->cnt = cnt;
	if (cnt > 0) {
		mem_set(marks->data, 0, marks->size * cnt);
	}

	return 0;
}

static void proj_graph_marks_reset(arr_t *marks)
{
	if (marks->cnt > 0) {
		mem_set(marks->data, 0, marks->size * marks->cnt);
	}
}

static void proj_graph_marks_next(arr_t *marks, uint *stamp)
{
	if (++*stamp == 0) {
		proj_graph_marks_reset(marks); // LCOV_EXCL_LINE
		*stamp = 1;		       // LCOV_EXCL_LINE
	}
}

static int proj_graph_count_edge(arr_t *marks, uint stamp, uint dep, uint *cnt)
{
	uint *mark = arr_get(marks, dep);
	if (*mark == stamp) {
		return 0;
	}

	*mark = stamp;
	(*cnt)++;

	return 0;
}

static int proj_graph_store_edge(proj_graph_t *graph, arr_t *marks, uint stamp, uint *edge, uint dep)
{
	uint *mark = arr_get(marks, dep);
	if (*mark == stamp) {
		return 0;
	}

	*mark					   = stamp;
	*(uint *)arr_get(&graph->edges, (*edge)++) = dep;

	return 0;
}

static int proj_graph_build_targets(const proj_t *proj, proj_graph_t *graph, alloc_t alloc)
{
	if (proj_graph_init_offsets(graph, proj->targets.cnt, alloc)) {
		return 1;
	}

	arr_t marks = {0};
	if (proj_graph_marks_init(&marks, proj->targets.cnt, alloc)) {
		proj_graph_free(graph);
		return 1;
	}

	uint stamp = 1;
	uint total = 0;
	target_t *target;
	list_node_t i = 0;
	list_foreach_all(&proj->targets, i, target)
	{
		uint cnt = 0;
		if (!target->has_deps) {
			*(uint *)arr_get(&graph->offsets, i) = 0;
			proj_graph_marks_next(&marks, &stamp);
			continue;
		}

		list_node_t deps = target->deps;
		list_node_t *dep;
		list_foreach(&proj->deps, deps, dep)
		{
			proj_graph_count_edge(&marks, stamp, *dep, &cnt);
		}

		*(uint *)arr_get(&graph->offsets, i) = cnt;
		total += cnt;
		proj_graph_marks_next(&marks, &stamp);
	}

	uint offset = 0;
	for (uint node = 0; node < proj->targets.cnt; node++) {
		uint cnt				= *(uint *)arr_get(&graph->offsets, node);
		*(uint *)arr_get(&graph->offsets, node) = offset;
		offset += cnt;
	}
	*(uint *)arr_get(&graph->offsets, proj->targets.cnt) = offset;

	if (proj_graph_init_edges(graph, total, alloc)) {
		arr_free(&marks);
		arr_free(&graph->offsets);
		return 1;
	}
	graph->edges.cnt = total;

	proj_graph_marks_reset(&marks);
	stamp = 1;
	i     = 0;
	list_foreach_all(&proj->targets, i, target)
	{
		if (!target->has_deps) {
			proj_graph_marks_next(&marks, &stamp);
			continue;
		}

		uint edge	 = proj_graph_edge_begin(graph, i);
		list_node_t deps = target->deps;
		list_node_t *dep;
		list_foreach(&proj->deps, deps, dep)
		{
			proj_graph_store_edge(graph, &marks, stamp, &edge, *dep);
		}

		proj_graph_marks_next(&marks, &stamp);
	}

	arr_free(&marks);
	return 0;
}

static int proj_graph_build_packages(const proj_t *proj, proj_graph_t *graph, alloc_t alloc)
{
	if (proj_graph_init_offsets(graph, proj->pkgs.cnt, alloc)) {
		return 1;
	}

	arr_t marks = {0};
	if (proj_graph_marks_init(&marks, proj->pkgs.cnt, alloc)) {
		proj_graph_free(graph);
		return 1;
	}

	uint stamp = 1;
	uint total = 0;
	const pkg_t *pkg;
	uint i = 0;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		uint cnt = 0;
		if (!pkg->has_targets) {
			*(uint *)arr_get(&graph->offsets, i) = 0;
			proj_graph_marks_next(&marks, &stamp);
			continue;
		}

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
				const target_t *dep_target = proj_get_target(proj, *dep);
				if (dep_target == NULL || dep_target->pkg == i) {
					continue;
				}

				proj_graph_count_edge(&marks, stamp, dep_target->pkg, &cnt);
			}
		}

		*(uint *)arr_get(&graph->offsets, i) = cnt;
		total += cnt;
		proj_graph_marks_next(&marks, &stamp);
	}

	uint offset = 0;
	for (uint node = 0; node < proj->pkgs.cnt; node++) {
		uint cnt				= *(uint *)arr_get(&graph->offsets, node);
		*(uint *)arr_get(&graph->offsets, node) = offset;
		offset += cnt;
	}
	*(uint *)arr_get(&graph->offsets, proj->pkgs.cnt) = offset;

	if (proj_graph_init_edges(graph, total, alloc)) {
		arr_free(&marks);
		arr_free(&graph->offsets);
		return 1;
	}
	graph->edges.cnt = total;

	proj_graph_marks_reset(&marks);
	stamp = 1;
	i     = 0;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		if (!pkg->has_targets) {
			proj_graph_marks_next(&marks, &stamp);
			continue;
		}

		uint edge = proj_graph_edge_begin(graph, i);
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
				const target_t *dep_target = proj_get_target(proj, *dep);
				if (dep_target == NULL || dep_target->pkg == i) {
					continue;
				}

				proj_graph_store_edge(graph, &marks, stamp, &edge, dep_target->pkg);
			}
		}

		proj_graph_marks_next(&marks, &stamp);
	}

	arr_free(&marks);
	return 0;
}

static strv_t proj_graph_target_name(const proj_t *proj, uint node)
{
	const target_t *target = proj_get_target(proj, node);
	return proj_get_str(proj, target->strs + TGT_STR_NAME);
}

static strv_t proj_graph_pkg_name(const proj_t *proj, uint node)
{
	const pkg_t *pkg = proj_get_pkg(proj, node);
	return proj_get_str(proj, pkg->strs + PKG_STR_NAME);
}

static int proj_graph_log_cycle(const proj_t *proj, uint node, uint dep, proj_graph_name_cb name, const char *msg)
{
	strv_t node_name = name(proj, node);
	strv_t dep_name	 = name(proj, dep);

	log_error(
		"cbuild", "proj", NULL, "%s: cycle detected: %.*s - %.*s", msg, node_name.len, node_name.data, dep_name.len, dep_name.data);

	return 1;
}

static int proj_graph_walk(const proj_graph_t *graph, const proj_t *proj, const arr_t *roots, arr_t *order, proj_graph_name_cb name,
			   const char *cycle_msg, alloc_t alloc)
{
	enum {
		UNVISITED,
		VISITING,
		DONE
	};

	arr_t state = {0};
	if (arr_init(&state, graph->cnt > 0 ? graph->cnt : 1, sizeof(uint8_t), alloc) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to traverse graph");
		return 1;
	}

	state.cnt = graph->cnt;
	if (graph->cnt > 0) {
		mem_set(state.data, 0, state.size * graph->cnt);
	}

	arr_t stack = {0};
	if (arr_init(&stack, graph->cnt > 0 ? graph->cnt : 1, sizeof(proj_graph_frame_t), alloc) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to traverse graph");
		arr_free(&state);
		return 1;
	}

	uint roots_cnt = roots ? roots->cnt : graph->cnt;
	for (uint i = 0; i < roots_cnt; i++) {
		uint node	    = roots ? *(uint *)arr_get(roots, i) : i;
		uint8_t *node_state = arr_get(&state, node);
		if (*node_state != UNVISITED) {
			continue;
		}

		proj_graph_frame_t *frame = arr_add(&stack, NULL);
		if (frame == NULL) {
			log_error("cbuild", "proj", NULL, "failed to traverse graph"); // LCOV_EXCL_LINE
			arr_free(&stack);					       // LCOV_EXCL_LINE
			arr_free(&state);					       // LCOV_EXCL_LINE
			return 1;						       // LCOV_EXCL_LINE
		}

		*frame	    = (proj_graph_frame_t){.node = node, .next = proj_graph_edge_begin(graph, node)};
		*node_state = VISITING;

		while (stack.cnt > 0) {
			proj_graph_frame_t *cur = arr_get(&stack, stack.cnt - 1);
			uint edge_end		= proj_graph_edge_end(graph, cur->node);

			if (cur->next < edge_end) {
				uint dep	   = proj_graph_edge_get(graph, cur->next++);
				uint8_t *dep_state = arr_get(&state, dep);

				switch (*dep_state) {
				case UNVISITED: {
					frame = arr_add(&stack, NULL);
					if (frame == NULL) {
						log_error("cbuild", "proj", NULL, "failed to traverse graph"); // LCOV_EXCL_LINE
						arr_free(&stack);					       // LCOV_EXCL_LINE
						arr_free(&state);					       // LCOV_EXCL_LINE
						return 1;						       // LCOV_EXCL_LINE
					}

					*frame	   = (proj_graph_frame_t){.node = dep, .next = proj_graph_edge_begin(graph, dep)};
					*dep_state = VISITING;
					break;
				}
				case VISITING: {
					uint node = cur->node;
					arr_free(&stack);
					arr_free(&state);
					return proj_graph_log_cycle(proj, node, dep, name, cycle_msg);
				}
				default:
					break;
				}

				continue;
			}

			*(uint8_t *)arr_get(&state, cur->node) = DONE;
			if (order && arr_addu(order, &cur->node, NULL)) {
				log_error("cbuild", "proj", NULL, "failed to traverse graph");
				arr_free(&stack);
				arr_free(&state);
				return 1;
			}

			stack.cnt--;
		}
	}

	arr_free(&stack);
	arr_free(&state);

	return 0;
}

static void proj_graph_reverse(arr_t *values)
{
	uint *data = values->data;
	for (uint i = 0; i < values->cnt / 2; i++) {
		uint tmp		  = data[i];
		data[i]			  = data[values->cnt - 1 - i];
		data[values->cnt - 1 - i] = tmp;
	}
}

static int proj_graph_walk_target_deps(const proj_t *proj, const arr_t *roots, arr_t *order, alloc_t alloc)
{
	enum {
		UNVISITED,
		VISITING,
		DONE
	};

	arr_t state = {0};
	if (arr_init(&state, proj->targets.cnt > 0 ? proj->targets.cnt : 1, sizeof(uint8_t), alloc) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get target dependencies");
		return 1;
	}

	state.cnt = proj->targets.cnt;
	if (proj->targets.cnt > 0) {
		mem_set(state.data, 0, state.size * state.cnt);
	}

	arr_t stack = {0};
	if (arr_init(&stack, proj->targets.cnt > 0 ? proj->targets.cnt : 1, sizeof(proj_graph_frame_t), alloc) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get target dependencies");
		arr_free(&state);
		return 1;
	}

	for (uint i = 0; i < roots->cnt; i++) {
		uint node	    = *(uint *)arr_get(roots, i);
		uint8_t *node_state = arr_get(&state, node);
		if (*node_state != UNVISITED) {
			continue;
		}

		const target_t *target	  = proj_get_target(proj, node);
		proj_graph_frame_t *frame = arr_add(&stack, NULL);
		if (frame == NULL) {
			log_error("cbuild", "proj", NULL, "failed to get target dependencies"); // LCOV_EXCL_LINE
			arr_free(&stack);							// LCOV_EXCL_LINE
			arr_free(&state);							// LCOV_EXCL_LINE
			return 1;								// LCOV_EXCL_LINE
		}

		*frame = (proj_graph_frame_t){
			.node = node,
			.next = target->has_deps ? target->deps : (uint)-1,
		};
		*node_state = VISITING;

		while (stack.cnt > 0) {
			proj_graph_frame_t *cur = arr_get(&stack, stack.cnt - 1);
			if (cur->next < proj->deps.cnt) {
				list_node_t dep_it = cur->next;
				uint *dep	   = list_get(&proj->deps, dep_it);
				list_get_next(&proj->deps, dep_it, &cur->next);

				if (dep == NULL) {
					log_error("cbuild", "proj", NULL, "failed to get target dependencies"); // LCOV_EXCL_LINE
					arr_free(&stack);							// LCOV_EXCL_LINE
					arr_free(&state);							// LCOV_EXCL_LINE
					return 1;								// LCOV_EXCL_LINE
				}

				uint8_t *dep_state = arr_get(&state, *dep);
				switch (*dep_state) {
				case UNVISITED: {
					target = proj_get_target(proj, *dep);
					frame  = arr_add(&stack, NULL);
					if (frame == NULL) {
						log_error("cbuild", "proj", NULL, "failed to get target dependencies"); // LCOV_EXCL_LINE
						arr_free(&stack);							// LCOV_EXCL_LINE
						arr_free(&state);							// LCOV_EXCL_LINE
						return 1;								// LCOV_EXCL_LINE
					}

					*frame = (proj_graph_frame_t){
						.node = *dep,
						.next = target->has_deps ? target->deps : (uint)-1,
					};
					*dep_state = VISITING;
					break;
				}
				case VISITING: {
					uint node = cur->node;
					arr_free(&stack);
					arr_free(&state);
					return proj_graph_log_cycle(
						proj, node, *dep, proj_graph_target_name, "failed to get target dependencies");
				}
				default:
					break;
				}

				continue;
			}

			*(uint8_t *)arr_get(&state, cur->node) = DONE;
			if (arr_addu(order, &cur->node, NULL)) {
				log_error("cbuild", "proj", NULL, "failed to get target dependencies");
				arr_free(&stack);
				arr_free(&state);
				return 1;
			}

			stack.cnt--;
		}
	}

	arr_free(&stack);
	arr_free(&state);
	return 0;
}

int proj_graph_transitive_deps(const proj_t *proj, list_node_t target, arr_t *deps, alloc_t alloc)
{
	if (proj == NULL || deps == NULL) {
		return 1;
	}

	const target_t *tgt = proj_get_target(proj, target);
	if (tgt == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get target dependencies: %d", target);
		return 1;
	}

	if (!tgt->has_deps) {
		return 0;
	}

	arr_t roots = {0};
	if (arr_init(&roots, 2, sizeof(uint), alloc) == NULL) {
		log_error("cbuild", "proj", NULL, "failed to get target dependencies");
		return 1;
	}

	list_node_t i = tgt->deps;
	list_node_t *dep;
	list_foreach(&proj->deps, i, dep)
	{
		if (arr_addu(&roots, dep, NULL)) {
			log_error("cbuild", "proj", NULL, "failed to get target dependencies");
			arr_free(&roots);
			return 1;
		}
	}

	proj_graph_reverse(&roots);

	int ret = proj_graph_walk_target_deps(proj, &roots, deps, alloc);
	if (!ret) {
		proj_graph_reverse(deps);
	}

	arr_free(&roots);

	return ret;
}

int proj_graph_toposort_packages(const proj_t *proj, arr_t *order, alloc_t alloc)
{
	if (proj == NULL || order == NULL) {
		return 1;
	}

	proj_graph_t graph = {0};
	if (proj_graph_build_packages(proj, &graph, alloc)) {
		return 1;
	}

	int ret = proj_graph_walk(&graph, proj, NULL, order, proj_graph_pkg_name, "failed to get package build order", alloc);
	proj_graph_free(&graph);

	return ret;
}

int proj_graph_toposort_targets(const proj_t *proj, arr_t *order, alloc_t alloc)
{
	if (proj == NULL || order == NULL) {
		return 1;
	}

	proj_graph_t graph = {0};
	if (proj_graph_build_targets(proj, &graph, alloc)) {
		return 1;
	}

	int ret = proj_graph_walk(&graph, proj, NULL, order, proj_graph_target_name, "failed to get target build order", alloc);
	proj_graph_free(&graph);

	return ret;
}
