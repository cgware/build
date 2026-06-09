#include "proj.h"
#include "proj_graph.h"

#include "log.h"
#include "test.h"

typedef struct fail_alloc_ctx_s {
	uint fail_at;
	uint cnt;
} fail_alloc_ctx_t;

static void *fail_alloc_alloc(alloc_t *alloc, size_t size)
{
	fail_alloc_ctx_t *ctx = alloc->priv;
	if (ctx && ctx->fail_at && ++ctx->cnt == ctx->fail_at) {
		return NULL;
	}

	return alloc_alloc_std(alloc, size);
}

static int fail_alloc_realloc(alloc_t *alloc, void **ptr, size_t *old_size, size_t new_size)
{
	fail_alloc_ctx_t *ctx = alloc->priv;
	if (ctx && ctx->fail_at && ++ctx->cnt == ctx->fail_at) {
		return 1;
	}

	return alloc_realloc_std(alloc, ptr, old_size, new_size);
}

static void fail_alloc_free(alloc_t *alloc, void *ptr, size_t size)
{
	alloc_free_std(alloc, ptr, size);
}

TEST(proj_graph_transitive_deps)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 5, 5, ALLOC_STD);

	uint p1, p2, p3, p4, p5;
	uint t1, t2, t3, t4, t5;

	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);
	proj_add_pkg(&proj, &p4);
	proj_add_pkg(&proj, &p5);

	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);
	proj_add_target(&proj, p3, &t3);
	proj_add_target(&proj, p4, &t4);
	proj_add_target(&proj, p5, &t5);

	arr_t deps = {0};
	arr_init(&deps, 5, sizeof(uint), ALLOC_STD);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_graph_transitive_deps(&proj, proj.targets.cnt, &deps, ALLOC_STD), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_graph_transitive_deps(&proj, t2, &deps, ALLOC_STD), 0);

	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t2, t3);
	proj_add_dep(&proj, t2, t4);
	proj_add_dep(&proj, t3, t4);

	EXPECT_EQ(proj_graph_transitive_deps(NULL, proj.targets.cnt, NULL, ALLOC_STD), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_graph_transitive_deps(&proj, proj.targets.cnt, NULL, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	EXPECT_EQ(proj_graph_transitive_deps(&proj, t4, &deps, ALLOC_STD), 0);
	EXPECT_EQ(deps.cnt, 0);

	EXPECT_EQ(proj_graph_transitive_deps(&proj, t2, &deps, ALLOC_STD), 0);
	EXPECT_EQ(deps.cnt, 2);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), t3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), t4);

	arr_free(&deps);
	proj_free(&proj);

	END;
}

TEST(proj_graph_transitive_deps_cycle)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 2, 2, ALLOC_STD);

	uint p1, p2;
	uint t1, t2;
	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);

	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t2, t1);

	arr_t deps = {0};
	arr_init(&deps, 2, sizeof(uint), ALLOC_STD);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_graph_transitive_deps(&proj, t1, &deps, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	arr_free(&deps);
	proj_free(&proj);

	END;
}

TEST(proj_graph_pdeps)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 4, 4, ALLOC_STD);

	uint p1, p2, p3;
	uint t1, t2, t3;

	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);

	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);
	proj_add_target(&proj, p3, &t3);

	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t1, t3);
	proj_add_dep(&proj, t3, t2);

	arr_t deps = {0};
	arr_init(&deps, 2, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_graph_transitive_deps(&proj, t1, &deps, ALLOC_STD), 0);

	EXPECT_EQ(deps.cnt, 2);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), t3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), t2);

	arr_free(&deps);
	proj_free(&proj);

	END;
}

TEST(proj_graph_rdeps)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 4, 4, ALLOC_STD);

	uint p1, p2, p3, p4;
	uint t1, t2, t3, t4;

	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);
	proj_add_pkg(&proj, &p4);

	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);
	proj_add_target(&proj, p3, &t3);
	proj_add_target(&proj, p4, &t4);

	proj_add_dep(&proj, t2, t1);
	proj_add_dep(&proj, t3, t1);
	proj_add_dep(&proj, t4, t2);
	proj_add_dep(&proj, t4, t3);

	arr_t deps = {0};
	arr_init(&deps, 3, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_graph_transitive_deps(&proj, t4, &deps, ALLOC_STD), 0);

	EXPECT_EQ(deps.cnt, 3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), t2);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), t3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 2), t1);

	arr_free(&deps);
	proj_free(&proj);

	END;
}

TEST(proj_graph_toposort_packages_no_target)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 3, 4, ALLOC_STD);

	proj_add_pkg(&proj, NULL);

	arr_t order = {0};
	arr_init(&order, 1, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_graph_toposort_packages(NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(proj_graph_toposort_packages(&proj, &order, ALLOC_STD), 0);

	EXPECT_EQ(order.cnt, 1);
	EXPECT_EQ(*(uint *)arr_get(&order, 0), 0);

	arr_free(&order);
	proj_free(&proj);

	END;
}

TEST(proj_graph_toposort_packages)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 3, 4, ALLOC_STD);

	uint p1, p2, p3;
	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);

	uint t1, t2, t3, t4;
	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p1, &t2);
	proj_add_target(&proj, p2, &t3);
	proj_add_target(&proj, p3, &t4);

	proj_add_dep(&proj, t1, t3);
	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t2, t3);
	proj_add_dep(&proj, t3, t4);

	arr_t order = {0};
	arr_init(&order, 2, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_graph_toposort_packages(&proj, &order, ALLOC_STD), 0);

	EXPECT_EQ(order.cnt, 3);
	EXPECT_EQ(*(uint *)arr_get(&order, 0), 2);
	EXPECT_EQ(*(uint *)arr_get(&order, 1), 1);
	EXPECT_EQ(*(uint *)arr_get(&order, 2), 0);

	proj_add_dep(&proj, t4, t3);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_graph_toposort_packages(&proj, &order, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	arr_free(&order);
	proj_free(&proj);

	END;
}

TEST(proj_graph_toposort_targets)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 3, 4, ALLOC_STD);

	uint p1, p2, p3;
	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);

	uint t1, t2, t3, t4;
	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p1, &t2);
	proj_add_target(&proj, p2, &t3);
	proj_add_target(&proj, p3, &t4);

	proj_add_dep(&proj, t1, t3);
	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t2, t3);
	proj_add_dep(&proj, t3, t4);

	arr_t order = {0};
	arr_init(&order, 2, sizeof(uint), ALLOC_STD);

	EXPECT_EQ(proj_graph_toposort_targets(NULL, NULL, ALLOC_STD), 1);
	EXPECT_EQ(proj_graph_toposort_targets(&proj, &order, ALLOC_STD), 0);

	EXPECT_EQ(order.cnt, 4);
	EXPECT_EQ(*(uint *)arr_get(&order, 0), 3);
	EXPECT_EQ(*(uint *)arr_get(&order, 1), 2);
	EXPECT_EQ(*(uint *)arr_get(&order, 2), 1);
	EXPECT_EQ(*(uint *)arr_get(&order, 3), 0);

	proj_add_dep(&proj, t4, t3);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_graph_toposort_targets(&proj, &order, ALLOC_STD), 1);
	log_set_quiet(0, 0);

	arr_free(&order);
	proj_free(&proj);

	END;
}

TEST(proj_graph_alloc_failures)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 3, 4, ALLOC_STD);

	uint p1, p2, p3;
	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);

	uint t1, t2, t3;
	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);
	proj_add_target(&proj, p3, &t3);

	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t1, t3);
	proj_add_dep(&proj, t2, t3);

	log_set_quiet(0, 1);

	int success = 0;
	for (uint fail_at = 1; fail_at < 128; fail_at++) {
		fail_alloc_ctx_t ctx = {.fail_at = fail_at};
		alloc_t alloc = {.alloc = fail_alloc_alloc, .realloc = fail_alloc_realloc, .free = fail_alloc_free, .priv = &ctx};

		arr_t deps = {0};
		if (arr_init(&deps, 1, sizeof(uint), alloc) == NULL) {
			continue;
		}
		int ret = proj_graph_transitive_deps(&proj, t1, &deps, alloc);
		arr_free(&deps);
		if (ret == 0) {
			success = 1;
		}
	}
	EXPECT_EQ(success, 1);

	success = 0;
	for (uint fail_at = 1; fail_at < 128; fail_at++) {
		fail_alloc_ctx_t ctx = {.fail_at = fail_at};
		alloc_t alloc = {.alloc = fail_alloc_alloc, .realloc = fail_alloc_realloc, .free = fail_alloc_free, .priv = &ctx};

		arr_t order = {0};
		if (arr_init(&order, 1, sizeof(uint), alloc) == NULL) {
			continue;
		}
		int ret = proj_graph_toposort_packages(&proj, &order, alloc);
		arr_free(&order);
		if (ret == 0) {
			success = 1;
		}
	}
	EXPECT_EQ(success, 1);

	success = 0;
	for (uint fail_at = 1; fail_at < 128; fail_at++) {
		fail_alloc_ctx_t ctx = {.fail_at = fail_at};
		alloc_t alloc = {.alloc = fail_alloc_alloc, .realloc = fail_alloc_realloc, .free = fail_alloc_free, .priv = &ctx};

		arr_t order = {0};
		if (arr_init(&order, 1, sizeof(uint), alloc) == NULL) {
			continue;
		}
		int ret = proj_graph_toposort_targets(&proj, &order, alloc);
		arr_free(&order);
		if (ret == 0) {
			success = 1;
		}
	}
	EXPECT_EQ(success, 1);

	log_set_quiet(0, 0);

	proj_free(&proj);

	END;
}

TEST(proj_graph_alloc_failures_expand)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 4, 4, ALLOC_STD);

	uint p1, p2, p3, p4;
	proj_add_pkg(&proj, &p1);
	proj_add_pkg(&proj, &p2);
	proj_add_pkg(&proj, &p3);
	proj_add_pkg(&proj, &p4);

	uint t1, t2, t3, t4;
	proj_add_target(&proj, p1, &t1);
	proj_add_target(&proj, p2, &t2);
	proj_add_target(&proj, p3, &t3);
	proj_add_target(&proj, p4, &t4);

	proj_add_dep(&proj, t1, t2);
	proj_add_dep(&proj, t1, t3);
	proj_add_dep(&proj, t1, t4);

	log_set_quiet(0, 1);

	int fail = 0;
	int success = 0;
	for (uint fail_at = 1; fail_at < 256; fail_at++) {
		fail_alloc_ctx_t ctx = {.fail_at = fail_at};
		alloc_t alloc = {.alloc = fail_alloc_alloc, .realloc = fail_alloc_realloc, .free = fail_alloc_free, .priv = &ctx};

		arr_t deps = {0};
		if (arr_init(&deps, 1, sizeof(uint), alloc) == NULL) {
			continue;
		}
		int ret = proj_graph_transitive_deps(&proj, t1, &deps, alloc);
		arr_free(&deps);
		if (ret != 0) {
			fail = 1;
		} else {
			success = 1;
		}
	}
	EXPECT_EQ(fail, 1);
	EXPECT_EQ(success, 1);

	fail = 0;
	success = 0;
	for (uint fail_at = 1; fail_at < 256; fail_at++) {
		fail_alloc_ctx_t ctx = {.fail_at = fail_at};
		alloc_t alloc = {.alloc = fail_alloc_alloc, .realloc = fail_alloc_realloc, .free = fail_alloc_free, .priv = &ctx};

		arr_t order = {0};
		if (arr_init(&order, 1, sizeof(uint), alloc) == NULL) {
			continue;
		}
		int ret = proj_graph_toposort_packages(&proj, &order, alloc);
		arr_free(&order);
		if (ret != 0) {
			fail = 1;
		} else {
			success = 1;
		}
	}
	EXPECT_EQ(fail, 1);
	EXPECT_EQ(success, 1);

	log_set_quiet(0, 0);

	proj_free(&proj);

	END;
}

STEST(proj_graph)
{
	SSTART;

	RUN(proj_graph_transitive_deps);
	RUN(proj_graph_transitive_deps_cycle);
	RUN(proj_graph_pdeps);
	RUN(proj_graph_rdeps);
	RUN(proj_graph_toposort_packages_no_target);
	RUN(proj_graph_toposort_packages);
	RUN(proj_graph_toposort_targets);
	RUN(proj_graph_alloc_failures);
	RUN(proj_graph_alloc_failures_expand);

	SEND;
}
