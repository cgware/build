#include "targets.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(targets_init_free)
{
	START;

	targets_t targets = {0};

	EXPECT_EQ(targets_init(NULL, 0, ALLOC_STD), NULL);
	mem_oom(1);
	EXPECT_EQ(targets_init(&targets, 1, ALLOC_STD), NULL);
	mem_oom(0);
	EXPECT_EQ(targets_init(&targets, 1, ALLOC_STD), &targets);

	targets_free(&targets);
	targets_free(NULL);

	END;
}

TEST(targets_target)
{
	START;

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(targets_target(NULL, STRV_NULL, NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(targets_target(&targets, STRV_NULL, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(targets_target(&targets, STRV_NULL, NULL), NULL);

	targets_free(&targets);

	END;
}

TEST(targets_target_oom)
{
	START;

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 1, ALLOC_STD);
	log_set_quiet(0, 0);

	mem_oom(1);
	targets.strs.used = targets.strs.size - sizeof(size_t) * 0;
	EXPECT_EQ(targets_target(&targets, STRV_NULL, NULL), NULL);
	targets.strs.used = targets.strs.size - sizeof(size_t) * 1;
	EXPECT_EQ(targets_target(&targets, STRV_NULL, NULL), NULL);
	mem_oom(0);

	targets_free(&targets);

	END;
}

TEST(targets_target_oom_targets)
{
	START;

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	mem_oom(1);
	EXPECT_EQ(targets_target(&targets, STRV("t"), NULL), NULL);
	mem_oom(0);

	targets_free(&targets);

	END;
}

TEST(targets_target_exist)
{
	START;

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	target_t *target = targets_target(&targets, STRV(""), NULL);

	uint id;
	EXPECT_EQ(targets_target(&targets, STRV(""), &id), target);
	EXPECT_EQ(id, 0);

	targets_free(&targets);

	END;
}

TEST(targets_app)
{
	START;

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	list_node_t id, dep;
	targets_target(&targets, STRV("exe"), &id);
	targets_target(&targets, STRV("lib"), &dep);

	EXPECT_EQ(targets_app(NULL, targets.targets.cnt, dep), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(targets_app(&targets, targets.targets.cnt, dep), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(targets_app(&targets, id, dep), 0);

	targets_free(&targets);

	END;
}

TEST(targets_add_dep)
{
	START;

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	list_node_t id;
	targets_target(&targets, STRV("exe"), &id);
	targets_target(&targets, STRV("lib"), NULL);

	EXPECT_EQ(targets_add_dep(NULL, targets.targets.cnt, STRV_NULL), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(targets_add_dep(&targets, targets.targets.cnt + 1, STRV("")), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(targets_add_dep(&targets, id, STRV("lib")), NULL);
	mem_oom(0);

	target_t *target = targets_get(&targets, id);
	target->has_deps = 1;
	target->deps	 = targets.targets.cnt;
	log_set_quiet(0, 1);
	EXPECT_EQ(targets_add_dep(&targets, id, STRV("lib")), NULL);
	log_set_quiet(0, 0);
	target->has_deps = 0;

	EXPECT_NE(targets_add_dep(&targets, id, STRV("lib")), NULL);

	targets_free(&targets);

	END;
}

TEST(targets_get)
{
	START;

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	target_t *target = targets_target(&targets, STRV("target"), NULL);

	EXPECT_EQ(targets_get(NULL, 0), NULL)
	EXPECT_EQ(targets_get(&targets, 0), target);

	targets_free(&targets);

	END;
}

TEST(targets_get_deps)
{
	START;

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	list_node_t target;

	targets_target(&targets, STRV("target0"), &target);
	targets_add_dep(&targets, target, STRV("target1"));

	arr_t deps = {0};
	log_set_quiet(0, 1);
	arr_init(&deps, 0, sizeof(uint), ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(targets_get_deps(NULL, targets.targets.cnt, NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(targets_get_deps(&targets, targets.targets.cnt, &deps), 1);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(targets_get_deps(&targets, target, &deps), 1);
	mem_oom(0);
	EXPECT_EQ(targets_get_deps(&targets, target, &deps), 0);

	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), 1);

	arr_free(&deps);
	targets_free(&targets);

	END;
}

TEST(targets_get_build_order)
{
	START;

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	list_node_t target;

	targets_target(&targets, STRV("target0"), &target);
	targets_add_dep(&targets, target, STRV("target1"));
	targets_target(&targets, STRV("target1"), NULL);
	targets_target(&targets, STRV("target2"), NULL);

	arr_t deps = {0};
	log_set_quiet(0, 1);
	arr_init(&deps, 0, sizeof(uint), ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(targets_get_build_order(NULL, NULL), 1);
	mem_oom(1);
	EXPECT_EQ(targets_get_build_order(&targets, &deps), 1);
	mem_oom(0);
	EXPECT_EQ(targets_get_build_order(&targets, &deps), 0);

	EXPECT_EQ(deps.cnt, 3);
	EXPECT_EQ(*(uint *)arr_get(&deps, 0), 1);
	EXPECT_EQ(*(uint *)arr_get(&deps, 1), 0);
	EXPECT_EQ(*(uint *)arr_get(&deps, 2), 2);

	arr_free(&deps);
	targets_free(&targets);

	END;
}

TEST(targets_print)
{
	START;

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	list_node_t id;
	targets_target(&targets, STRV("exe"), &id);
	targets_add_dep(&targets, id, STRV("lib"));

	char buf[256] = {0};
	EXPECT_EQ(targets_print(&targets, id, DST_BUF(buf)), 51);
	EXPECT_STR(buf,
		   "[target]\n"
		   "TYPE: UNKNOWN\n"
		   "NAME: exe\n"
		   "FILE: \n"
		   "DEPS: lib\n"
		   "\n");

	targets_free(&targets);

	END;
}

STEST(targets)
{
	SSTART;

	RUN(targets_init_free);
	RUN(targets_target);
	RUN(targets_target_oom);
	RUN(targets_target_oom_targets);
	RUN(targets_target_exist);
	RUN(targets_app);
	RUN(targets_add_dep);
	RUN(targets_get);
	RUN(targets_get_deps);
	RUN(targets_get_build_order);
	RUN(targets_print);

	SEND;
}
