#include "config.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(config_init_free)
{
	START;

	config_t config = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(config_init(NULL, 0, ALLOC_STD), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(config_init(&config, 1, ALLOC_STD), NULL);
	mem_oom(0);
	EXPECT_EQ(config_init(&config, 1, ALLOC_STD), &config);

	config_free(&config);
	config_free(NULL);

	END;
}

TEST(config_state)
{
	START;

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_state_t state;
	config_get_state(NULL, NULL);
	config_get_state(&config, &state);

	config_pkg(&config, 0, 0);

	config_set_state(NULL, state);
	config_set_state(&config, state);
	EXPECT_EQ(config.ops.cnt, 0);
	EXPECT_EQ(config.strs.used, 0);
	EXPECT_EQ(config.lists.cnt, 0);

	config_free(&config);

	END;
}

TEST(config_pkg)
{
	START;

	config_t config = {0};
	log_set_quiet(0, 1);
	config_init(&config, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(config_pkg(NULL, 0, 0), 1);
	mem_oom(1);
	EXPECT_EQ(config_pkg(&config, 0, 0), 1);
	mem_oom(0);
	EXPECT_EQ(config_pkg(&config, 0, 0), 0);

	config_free(&config);

	END;
}

TEST(config_str)
{
	START;

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	EXPECT_EQ(config_str(NULL, 0, 0, 0, 0, STRV_NULL), 1);
	mem_oom(1);
	config.strs.used = config.strs.size;
	EXPECT_EQ(config_str(&config, CONFIG_OP_TYPE_PKG_PATH, 0, -1, 0, STRV_NULL), 1);
	config.strs.used = 0;
	config.ops.cnt	 = config.ops.cap;
	EXPECT_EQ(config_str(&config, CONFIG_OP_TYPE_PKG_PATH, 0, -1, 0, STRV_NULL), 1);
	EXPECT_EQ(config_str(&config, __CONFIG_OP_TYPE_CNT, 0, -1, 0, STRV_NULL), 1);
	config.ops.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(config_str(&config, CONFIG_OP_TYPE_PKG_PATH, 0, -1, 0, STRV_NULL), 0);

	config_free(&config);

	END;
}

TEST(config_str_list)
{
	START;

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	uint deps;

	EXPECT_EQ(config_str_list(NULL, 0, 0, -1, 0, STRV_NULL, NULL), 1);
	mem_oom(1);
	config.strs.used = config.strs.size;
	EXPECT_EQ(config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 0, -1, 0, STRV_NULL, NULL), 1);
	config.strs.used = 0;
	config.lists.cnt = config.lists.cap;
	EXPECT_EQ(config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 0, -1, 0, STRV_NULL, NULL), 1);
	config.lists.cnt = 0;
	config.ops.cnt	 = config.ops.cap;
	EXPECT_EQ(config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 0, -1, 0, STRV_NULL, NULL), 1);
	config.ops.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 0, -1, 0, STRV_NULL, &deps), 0);
	EXPECT_EQ(deps, 0);

	config_free(&config);

	END;
}

TEST(config_str_list_add)
{
	START;

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	uint deps;
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 0, -1, 0, STRV_NULL, &deps);

	EXPECT_EQ(config_str_list_add(NULL, deps, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_str_list_add(&config, config.lists.cnt, STRV_NULL), 1);
	log_set_quiet(0, 0);

	mem_oom(1);
	size_t used	 = config.strs.used;
	config.strs.used = config.strs.size;
	EXPECT_EQ(config_str_list_add(&config, deps, STRV_NULL), 1);
	config.strs.used = used;

	uint cnt	 = config.lists.cnt;
	config.lists.cnt = config.lists.cap;
	EXPECT_EQ(config_str_list_add(&config, deps, STRV_NULL), 1);
	config.lists.cnt = cnt;
	mem_oom(0);

	config_op_t *op	 = arr_get(&config.ops, deps);
	list_node_t list = op->args.l;
	op->args.l	 = config.lists.cnt;
	log_set_quiet(0, 1);
	EXPECT_EQ(config_str_list_add(&config, deps, STRV_NULL), 1);
	log_set_quiet(0, 0);
	op	   = arr_get(&config.ops, deps);
	op->args.l = list;

	EXPECT_EQ(config_str_list_add(&config, deps, STRV_NULL), 0);
	EXPECT_EQ(deps, 0);

	config_free(&config);

	END;
}

TEST(config_tgt)
{
	START;

	config_t config = {0};
	log_set_quiet(0, 1);
	config_init(&config, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(config_tgt(NULL, 0, 0, 0), 1);
	mem_oom(1);
	EXPECT_EQ(config_tgt(&config, 0, 0, 0), 1);
	mem_oom(0);
	EXPECT_EQ(config_tgt(&config, 0, 0, 0), 0);

	config_free(&config);

	END;
}

TEST(config_tgt_type)
{
	START;

	config_t config = {0};
	log_set_quiet(0, 1);
	config_init(&config, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	EXPECT_EQ(config_tgt_type(NULL, 0, 0, 0, 0), 1);
	mem_oom(1);
	EXPECT_EQ(config_tgt_type(&config, 0, 0, 0, 0), 1);
	mem_oom(0);
	EXPECT_EQ(config_tgt_type(&config, 0, 0, 0, 0), 0);

	config_free(&config);

	END;
}

TEST(config_merge)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);

	EXPECT_EQ(config_merge(NULL, NULL, CONFIG_STATE_NULL, NULL), 1);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_same_unknown)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_op_t *op = arr_add(&config.ops, NULL);
	op->type	= CONFIG_OP_TYPE_UNKNOWN;
	op->mode	= __CONFIG_MODE_CNT;

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	op	 = arr_add(&other.ops, NULL);
	op->type = CONFIG_OP_TYPE_UNKNOWN;
	op->mode = __CONFIG_MODE_CNT;

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "");
	log_set_quiet(0, 0);

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_same_pkg)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_pkg(&config, pkg, CONFIG_MODE_APP);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_pkg(&other, pkg, CONFIG_MODE_APP);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkgs += pkg\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_same_str)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_SET, STRV("path"));

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_str(&other, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_SET, STRV("path"));

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg::path = path\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_same_str_list)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	uint deps;
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, pkg, -1, CONFIG_MODE_SET, STRV("dep1"), &deps);
	config_str_list_add(&config, deps, STRV("dep2"));

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_str_list(&other, CONFIG_OP_TYPE_PKG_DEPS, pkg, -1, CONFIG_MODE_SET, STRV("dep1"), &deps);
	config_str_list_add(&other, deps, STRV("dep2"));

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg::deps = dep1, dep2\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_same_tgt)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint pkg, tgt;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, STRV("tgt"), &tgt);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_tgt(&config, pkg, tgt, CONFIG_MODE_APP);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_tgt(&other, pkg, tgt, CONFIG_MODE_APP);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg:tgts += tgt\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_same_tgt_type)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint pkg, tgt;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, STRV("tgt"), &tgt);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_tgt_type(&config, pkg, tgt, CONFIG_MODE_SET, 0);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_tgt_type(&other, pkg, tgt, CONFIG_MODE_SET, 0);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg:tgt:type = 0\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_diff_mode)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_pkg(&config, pkg, CONFIG_MODE_EN);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_pkg(&other, pkg, CONFIG_MODE_APP);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkgs += pkg\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_diff_pkg)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint pkg1, pkg2;
	registry_add_pkg(&registry, STRV("pkg1"), &pkg1);
	registry_add_pkg(&registry, STRV("pkg2"), &pkg2);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_pkg(&config, pkg1, CONFIG_MODE_APP);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_pkg(&other, pkg2, CONFIG_MODE_APP);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf,
		   "pkgs += pkg1\n"
		   "\n"
		   "pkgs += pkg2\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_diff_str)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_SET, STRV("path1"));

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_str(&other, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_SET, STRV("path2"));

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 1);
	log_set_quiet(0, 0);

	other.prio = 1;
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg::path = path2\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_diff_str_list)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 10, ALLOC_STD);
	uint deps;
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, pkg, -1, CONFIG_MODE_SET, STRV("dep1"), &deps);
	config_str_list_add(&config, deps, STRV("dep2"));

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_str_list(&other, CONFIG_OP_TYPE_PKG_DEPS, pkg, -1, CONFIG_MODE_SET, STRV("dep1"), &deps);
	config_str_list_add(&other, deps, STRV("dep3"));

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 1);
	log_set_quiet(0, 0);

	other.prio = 1;
	mem_oom(1);
	size_t cnt	 = config.strs.used;
	config.strs.used = config.strs.size;
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 1);
	config.strs.used = cnt;
	cnt		 = config.lists.cnt;
	config.lists.cnt = config.lists.cap;
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 1);
	config.lists.cnt = cnt;
	mem_oom(0);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg::deps = dep1, dep3\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_diff_str_list_add)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	uint deps;
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, pkg, -1, CONFIG_MODE_APP, STRV("dep1"), &deps);
	config_str_list_add(&config, deps, STRV("dep2"));

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_str_list(&other, CONFIG_OP_TYPE_PKG_DEPS, pkg, -1, CONFIG_MODE_APP, STRV("dep1"), &deps);
	config_str_list_add(&other, deps, STRV("dep3"));

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);
	log_set_quiet(0, 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg::deps += dep1, dep2, dep3\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_diff_tgt)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 3, ALLOC_STD);
	uint pkg, tgt1, tgt2;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, STRV("tgt1"), &tgt1);
	registry_add_tgt(&registry, STRV("tgt2"), &tgt2);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_tgt(&config, pkg, tgt1, CONFIG_MODE_APP);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_tgt(&other, pkg, tgt2, CONFIG_MODE_APP);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf,
		   "pkg:tgts += tgt1\n"
		   "\n"
		   "pkg:tgts += tgt2\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_diff_tgt_type)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint pkg, tgt;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, STRV("tgt"), &tgt);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_tgt_type(&config, pkg, tgt, CONFIG_MODE_SET, 0);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_tgt_type(&other, pkg, tgt, CONFIG_MODE_SET, 1);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 1);
	log_set_quiet(0, 0);

	other.prio = 1;
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg:tgt:type = 1\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_unknown)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_op_t *op = arr_add(&other.ops, NULL);
	op->type	= 0;

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);
	log_set_quiet(0, 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_pkg)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_pkg(&other, pkg, CONFIG_MODE_APP);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkgs += pkg\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_str)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_str(&other, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_SET, STRV("path"));

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg::path = path\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_str_list)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	uint deps;
	config_str_list(&other, CONFIG_OP_TYPE_PKG_DEPS, pkg, -1, CONFIG_MODE_SET, STRV("dep1"), &deps);
	config_str_list_add(&other, deps, STRV("dep2"));

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg::deps = dep1, dep2\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_tgt)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint pkg, tgt;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, STRV("tgt"), &tgt);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_tgt(&other, pkg, tgt, CONFIG_MODE_APP);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg:tgts += tgt\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_merge_tgt_type)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint pkg, tgt;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, STRV("tgt"), &tgt);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_tgt_type(&other, pkg, tgt, CONFIG_MODE_SET, 0);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &registry), 0);

	char buf[256] = {0};
	config_print(&config, &registry, DST_BUF(buf));
	EXPECT_STR(buf, "pkg:tgt:type = 0\n");

	config_free(&config);
	config_free(&other);
	registry_free(&registry);

	END;
}

TEST(config_print)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	EXPECT_EQ(config_print(NULL, NULL, DST_NONE()), 0);

	char buf[256] = {0};
	EXPECT_EQ(config_print(&config, &registry, DST_BUF(buf)), 0);
	EXPECT_STR(buf, "");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_unknown)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_op_t *op = arr_add(&config.ops, NULL);
	op->type	= 0;

	char buf[256] = {0};
	log_set_quiet(0, 1);
	EXPECT_EQ(config_print(&config, &registry, DST_BUF(buf)), 0);
	log_set_quiet(0, 0);
	EXPECT_STR(buf, "");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_pkg)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	config_pkg(&config, pkg, CONFIG_MODE_APP);

	char buf[256] = {0};
	EXPECT_NE(config_print(&config, &registry, DST_BUF(buf)), 0);
	EXPECT_STR(buf, "pkgs += pkg\n");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_pkg_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	config_pkg(&config, pkg, __CONFIG_MODE_CNT);

	char buf[256] = {0};
	log_set_quiet(0, 1);
	EXPECT_NE(config_print(&config, &registry, DST_BUF(buf)), 0);
	log_set_quiet(0, 0);
	EXPECT_STR(buf, "pkgs = pkg\n");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_str)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	config_str(&config, CONFIG_OP_TYPE_PKG_PATH, pkg, -1, CONFIG_MODE_EN, STRV("path"));

	char buf[256] = {0};
	EXPECT_NE(config_print(&config, &registry, DST_BUF(buf)), 0);
	EXPECT_STR(buf, "pkg::path ?= path\n");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_str_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_str(&config, CONFIG_OP_TYPE_PKG_PATH, 0, -1, __CONFIG_MODE_CNT, STRV("path"));

	char buf[256] = {0};
	log_set_quiet(0, 1);
	EXPECT_NE(config_print(&config, &registry, DST_BUF(buf)), 0);
	log_set_quiet(0, 0);
	EXPECT_STR(buf, "::path = path\n");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_str_list)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);

	uint deps;
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, pkg, -1, CONFIG_MODE_EN, STRV("dep1"), &deps);
	config_str_list_add(&config, deps, STRV("dep2"));

	char buf[256] = {0};
	EXPECT_NE(config_print(&config, &registry, DST_BUF(buf)), 0);
	EXPECT_STR(buf, "pkg::deps ?= dep1, dep2\n");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_tgt)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	uint pkg, tgt;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, STRV("tgt"), &tgt);
	config_tgt(&config, pkg, tgt, CONFIG_MODE_APP);

	char buf[256] = {0};
	EXPECT_NE(config_print(&config, &registry, DST_BUF(buf)), 0);
	EXPECT_STR(buf, "pkg:tgts += tgt\n");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_tgt_type)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	uint pkg, tgt;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, STRV("tgt"), &tgt);

	config_tgt_type(&config, pkg, tgt, CONFIG_MODE_SET, 0);

	char buf[256] = {0};
	EXPECT_NE(config_print(&config, &registry, DST_BUF(buf)), 0);
	EXPECT_STR(buf, "pkg:tgt:type = 0\n");

	config_free(&config);
	registry_free(&registry);

	END;
}

STEST(config)
{
	SSTART;

	RUN(config_init_free);
	RUN(config_state);
	RUN(config_pkg);
	RUN(config_str);
	RUN(config_str_list);
	RUN(config_str_list_add);
	RUN(config_tgt);
	RUN(config_tgt_type);
	RUN(config_merge);
	RUN(config_merge_same_unknown);
	RUN(config_merge_same_pkg);
	RUN(config_merge_same_str);
	RUN(config_merge_same_str_list);
	RUN(config_merge_same_tgt);
	RUN(config_merge_same_tgt_type);
	RUN(config_merge_diff_mode);
	RUN(config_merge_diff_pkg);
	RUN(config_merge_diff_str);
	RUN(config_merge_diff_str_list);
	RUN(config_merge_diff_str_list_add);
	RUN(config_merge_diff_tgt);
	RUN(config_merge_diff_tgt_type);
	RUN(config_merge_unknown);
	RUN(config_merge_pkg);
	RUN(config_merge_str);
	RUN(config_merge_str_list);
	RUN(config_merge_tgt);
	RUN(config_merge_tgt_type);
	RUN(config_print);
	RUN(config_print_unknown);
	RUN(config_print_pkg);
	RUN(config_print_pkg_invalid);
	RUN(config_print_str);
	RUN(config_print_str_invalid);
	RUN(config_print_str_list);
	RUN(config_print_tgt);
	RUN(config_print_tgt_type);

	SEND;
}
