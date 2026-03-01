#include "registry.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(registry_init_free)
{
	START;

	registry_t registry = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(registry_init(NULL, 0, ALLOC_STD), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(registry_init(&registry, 1, ALLOC_STD), NULL);
	mem_oom(0);
	EXPECT_EQ(registry_init(&registry, 1, ALLOC_STD), &registry);

	registry_free(&registry);
	registry_free(NULL);

	END;
}

TEST(registry_add_pkg)
{
	START;

	registry_t registry = {0};

	registry_init(&registry, 1, ALLOC_STD);

	uint id;
	EXPECT_EQ(registry_add_pkg(NULL, STRV_NULL, NULL), 1);
	mem_oom(1);
	registry.strs.used = registry.strs.size;
	EXPECT_EQ(registry_add_pkg(&registry, STRV(""), NULL), 1);
	registry.strs.used = 0;
	registry.pkgs.cnt  = registry.pkgs.cap;
	EXPECT_EQ(registry_add_pkg(&registry, STRV(""), NULL), 1);
	registry.pkgs.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(registry_add_pkg(&registry, STRV(""), &id), 0);
	EXPECT_EQ(registry_add_pkg(&registry, STRV(""), &id), 0);
	EXPECT_EQ(id, 0);

	registry_free(&registry);

	END;
}

TEST(registry_get_pkg)
{
	START;

	registry_t registry = {0};

	registry_init(&registry, 1, ALLOC_STD);
	uint id;
	registry_add_pkg(&registry, STRV("pkg"), &id);

	EXPECT_EQ(registry_get_pkg(NULL, 0).data, NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(registry_get_pkg(&registry, registry.pkgs.cnt).data, NULL);
	log_set_quiet(0, 0);
	strv_t name = registry_get_pkg(&registry, id);
	EXPECT_STRN(name.data, "pkg", name.len);

	registry_free(&registry);

	END;
}

TEST(registry_find_pkg)
{
	START;

	registry_t registry = {0};

	registry_init(&registry, 1, ALLOC_STD);
	uint id;
	registry_add_pkg(&registry, STRV("pkg"), &id);

	uint pkg;
	EXPECT_EQ(registry_find_pkg(NULL, STRV_NULL, NULL), 1);
	EXPECT_EQ(registry_find_pkg(&registry, STRV_NULL, NULL), 1);
	EXPECT_EQ(registry_find_pkg(&registry, STRV("pkg"), &pkg), 0);
	EXPECT_EQ(pkg, id);

	registry_free(&registry);

	END;
}

TEST(registry_add_tgt)
{
	START;

	registry_t registry = {0};

	registry_init(&registry, 1, ALLOC_STD);

	uint id;
	EXPECT_EQ(registry_add_tgt(NULL, -1, STRV_NULL, NULL), 1);
	mem_oom(1);
	registry.strs.used = registry.strs.size;
	EXPECT_EQ(registry_add_tgt(&registry, -1, STRV(""), NULL), 1);
	registry.strs.used = 0;
	registry.tgts.cnt  = registry.tgts.cap;
	EXPECT_EQ(registry_add_tgt(&registry, -1, STRV(""), NULL), 1);
	registry.tgts.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(registry_add_tgt(&registry, -1, STRV(""), &id), 0);
	EXPECT_EQ(registry_add_tgt(&registry, -1, STRV(""), &id), 0);
	EXPECT_EQ(id, 0);

	registry_free(&registry);

	END;
}

TEST(registry_get_tgt)
{
	START;

	registry_t registry = {0};

	registry_init(&registry, 1, ALLOC_STD);
	uint pkg, id;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), &id);

	EXPECT_EQ(registry_get_tgt(NULL, 0).data, NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(registry_get_tgt(&registry, registry.tgts.cnt).data, NULL);
	log_set_quiet(0, 0);
	strv_t name = registry_get_tgt(&registry, id);
	EXPECT_STRN(name.data, "tgt", name.len);

	registry_free(&registry);

	END;
}

TEST(registry_find_tgt)
{
	START;

	registry_t registry = {0};

	registry_init(&registry, 1, ALLOC_STD);
	uint pkg, id;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), &id);

	uint tgt;
	EXPECT_EQ(registry_find_tgt(NULL, -1, STRV_NULL, NULL), 1);
	EXPECT_EQ(registry_find_tgt(&registry, pkg, STRV_NULL, NULL), 1);
	EXPECT_EQ(registry_find_tgt(&registry, pkg, STRV("tgt"), &tgt), 0);
	EXPECT_EQ(tgt, id);

	registry_free(&registry);

	END;
}

STEST(registry)
{
	SSTART;

	RUN(registry_init_free);
	RUN(registry_add_pkg);
	RUN(registry_get_pkg);
	RUN(registry_find_pkg);
	RUN(registry_add_tgt);
	RUN(registry_get_tgt);
	RUN(registry_find_tgt);

	SEND;
}
