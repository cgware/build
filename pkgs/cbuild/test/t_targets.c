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

TEST(targets_add)
{
	START;

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	lnode_t list = LIST_END;

	EXPECT_EQ(targets_add(NULL, NULL, STRV_NULL, NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(targets_add(&targets, &list, STRV_NULL, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(targets_add(&targets, &list, STRV_NULL, NULL), NULL);

	targets_free(&targets);

	END;
}

TEST(targets_add_oom)
{
	START;

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	strbuf_free(&targets.names);
	strbuf_init(&targets.names, 1, 8, ALLOC_STD);

	lnode_t list = LIST_END;

	mem_oom(1);
	EXPECT_EQ(targets_add(&targets, &list, STRV_NULL, NULL), NULL);
	mem_oom(0);

	targets_free(&targets);

	END;
}

TEST(targets_add_exist)
{
	START;

	targets_t targets = {0};
	log_set_quiet(0, 1);
	targets_init(&targets, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	lnode_t list = LIST_END;

	target_t *target = targets_add(&targets, &list, STRV(""), NULL);

	uint id;
	EXPECT_EQ(targets_add(&targets, &list, STRV(""), &id), target);
	EXPECT_EQ(id, 0);

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

	lnode_t list = LIST_END;

	lnode_t id;
	targets_add(&targets, &list, STRV("exe"), &id);
	targets_add(&targets, &list, STRV("lib"), NULL);

	EXPECT_EQ(targets_add_dep(NULL, LIST_END, STRV_NULL), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(targets_add_dep(&targets, LIST_END, STRV("")), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(targets_add_dep(&targets, id, STRV("lib")), NULL);
	mem_oom(0);
	EXPECT_NE(targets_add_dep(&targets, id, STRV("lib")), NULL);

	targets_free(&targets);

	END;
}

TEST(targets_get)
{
	START;

	targets_t targets = {0};
	targets_init(&targets, 1, ALLOC_STD);

	lnode_t list = LIST_END;

	target_t *target = targets_add(&targets, &list, STRV("target"), NULL);

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

	lnode_t list = LIST_END;

	lnode_t target;

	targets_add(&targets, &list, STRV("target0"), &target);
	targets_add_dep(&targets, target, STRV("target1"));

	arr_t deps = {0};
	log_set_quiet(0, 1);
	arr_init(&deps, 0, sizeof(uint), ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(targets_get_deps(NULL, LIST_END, NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(targets_get_deps(&targets, LIST_END, &deps), 1);
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

	lnode_t list = LIST_END;

	lnode_t target;

	targets_add(&targets, &list, STRV("target0"), &target);
	targets_add_dep(&targets, target, STRV("target1"));
	targets_add(&targets, &list, STRV("target1"), NULL);
	targets_add(&targets, &list, STRV("target2"), NULL);

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

	lnode_t list = LIST_END;

	uint id;
	targets_add(&targets, &list, STRV("exe"), &id);
	targets_add_dep(&targets, id, STRV("lib"));

	char buf[256] = {0};
	EXPECT_EQ(targets_print(&targets, list, PRINT_DST_BUF(buf, sizeof(buf), 0)), 51);
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
	RUN(targets_add);
	RUN(targets_add_oom);
	RUN(targets_add_exist);
	RUN(targets_add_dep);
	RUN(targets_get);
	RUN(targets_get_deps);
	RUN(targets_get_build_order);
	RUN(targets_print);

	SEND;
}
