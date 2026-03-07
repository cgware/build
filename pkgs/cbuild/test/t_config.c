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

	config_int(&config, 0, 0, 0, 0, 0);

	config_set_state(NULL, state);
	config_set_state(&config, state);
	EXPECT_EQ(config.vals.cnt, 0);
	EXPECT_EQ(config.strs.used, 0);
	EXPECT_EQ(config.lists.cnt, 0);

	config_free(&config);

	END;
}

TEST(config_int)
{
	START;

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	EXPECT_EQ(config_int(NULL, 0, 0, 0, CONFIG_ACT_UNKNOWN, 0), 1);
	mem_oom(1);
	config.vals.cnt = config.vals.cap;
	EXPECT_EQ(config_int(&config, 0, 0, 0, CONFIG_ACT_UNKNOWN, 0), 1);
	config.vals.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(config_int(&config, 0, 0, 0, CONFIG_ACT_UNKNOWN, 0), 0);

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
	EXPECT_EQ(config_str(&config, 0, 0, 0, 0, STRV_NULL), 1);
	config.strs.used = 0;
	config.vals.cnt	 = config.vals.cap;
	EXPECT_EQ(config_str(&config, 0, 0, 0, 0, STRV_NULL), 1);
	config.vals.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(config_str(&config, 0, 0, 0, 0, STRV_NULL), 0);

	config_free(&config);

	END;
}

TEST(config_str_list)
{
	START;

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);

	EXPECT_EQ(config_str_list(NULL, 0, 0, 0, 0, STRV_NULL, NULL), 1);
	mem_oom(1);
	config.strs.used = config.strs.size;
	EXPECT_EQ(config_str_list(&config, 0, 0, 0, 0, STRV_NULL, NULL), 1);
	config.strs.used = 0;
	config.lists.cnt = config.lists.cap;
	EXPECT_EQ(config_str_list(&config, 0, 0, 0, 0, STRV_NULL, NULL), 1);
	config.lists.cnt = 0;
	config.vals.cnt	 = config.vals.cap;
	EXPECT_EQ(config_str_list(&config, 0, 0, 0, 0, STRV_NULL, NULL), 1);
	config.vals.cnt = 0;
	mem_oom(0);

	log_set_quiet(0, 1);
	uint deps = config.vals.cnt;
	EXPECT_EQ(config_str_list(&config, 0, 0, 0, 0, STRV_NULL, &deps), 1);
	deps = -1;
	log_set_quiet(0, 0);

	EXPECT_EQ(config_str_list(&config, 0, 0, 0, 0, STRV_NULL, &deps), 0);
	EXPECT_EQ(deps, 0);

	mem_oom(1);
	config_val_t *v	 = arr_get(&config.vals, deps);
	list_node_t list = v->args.l;
	v->args.l	 = config.lists.cnt;
	EXPECT_EQ(config_str_list(&config, 0, 0, 0, 0, STRV_NULL, &deps), 1);
	v	  = arr_get(&config.vals, deps);
	v->args.l = list;
	mem_oom(0);

	config_free(&config);

	END;
}

TEST(config_merge)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc[] = {
		{CONFIG_TYPE_UNKNOWN, CONFIG_SCOPE_GLOBAL, STRV(""), NULL},
	};
	config_schema_add_ops(&schema, desc, sizeof(desc));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);

	EXPECT_EQ(config_merge(NULL, NULL, CONFIG_STATE_NULL, NULL, NULL), 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	log_set_quiet(0, 1);
	config_int(&other, 0, 0, 0, CONFIG_ACT_SET, 0);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 1);
	config_int(&config, 0, 0, 0, CONFIG_ACT_SET, 0);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_same_unknown)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_val_t *v = arr_add(&config.vals, NULL);
	v->op		= 0;
	v->act		= __CONFIG_ACT_CNT;

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	v      = arr_add(&other.vals, NULL);
	v->op  = 0;
	v->act = __CONFIG_ACT_CNT;

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, "");
	log_set_quiet(0, 0);

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_same_int)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_INT, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(desc));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_int(&config, 0, 0, 0, CONFIG_ACT_SET, 0);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_int(&other, 0, 0, 0, CONFIG_ACT_SET, 0);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = 0\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_same_str)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, 0, 0, 0, CONFIG_ACT_SET, STRV("path"));

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_str(&other, 0, 0, 0, CONFIG_ACT_SET, STRV("path"));

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = path\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_same_str_list)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR_LIST, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	uint deps = -1;
	config_str_list(&config, 0, 0, 0, CONFIG_ACT_SET, STRV("dep1"), &deps);
	config_str_list(&config, 0, 0, 0, CONFIG_ACT_SET, STRV("dep2"), &deps);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	deps = -1;
	config_str_list(&other, 0, 0, 0, CONFIG_ACT_SET, STRV("dep1"), &deps);
	config_str_list(&other, 0, 0, 0, CONFIG_ACT_SET, STRV("dep2"), &deps);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = dep1, dep2\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_diff_mode)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_INT, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_int(&config, 0, 0, 0, CONFIG_ACT_EN, 0);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_int(&other, 0, 0, 0, CONFIG_ACT_APP, 0);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " += 0\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_diff_int)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_INT, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_int(&config, 0, 0, 0, CONFIG_ACT_SET, 0);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_int(&other, 0, 0, 0, CONFIG_ACT_SET, 1);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 1);
	log_set_quiet(0, 0);

	other.prio = 1;
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = 1\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_diff_str)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, 0, 0, 0, CONFIG_ACT_SET, STRV("path1"));

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_str(&other, 0, 0, 0, CONFIG_ACT_SET, STRV("path2"));

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 1);
	log_set_quiet(0, 0);

	other.prio = 1;
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = path2\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_diff_str_list_set)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR_LIST, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 10, ALLOC_STD);
	uint deps = -1;
	config_str_list(&config, 0, 0, 0, CONFIG_ACT_SET, STRV("dep1"), &deps);
	config_str_list(&config, 0, 0, 0, CONFIG_ACT_SET, STRV("dep2"), &deps);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	deps = -1;
	config_str_list(&other, 0, 0, 0, CONFIG_ACT_SET, STRV("dep1"), &deps);
	config_str_list(&other, 0, 0, 0, CONFIG_ACT_SET, STRV("dep3"), &deps);

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 1);
	log_set_quiet(0, 0);

	other.prio = 1;
	mem_oom(1);
	size_t cnt	 = config.strs.used;
	config.strs.used = config.strs.size;
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 1);
	config.strs.used = cnt;
	cnt		 = config.lists.cnt;
	config.lists.cnt = config.lists.cap;
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 1);
	config.lists.cnt = cnt;
	mem_oom(0);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = dep1, dep3\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_diff_str_list_app)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR_LIST, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 10, ALLOC_STD);
	uint deps = -1;
	config_str_list(&config, 0, 0, 0, CONFIG_ACT_SET, STRV("dep1"), &deps);
	config_str_list(&config, 0, 0, 0, CONFIG_ACT_SET, STRV("dep2"), &deps);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	deps = -1;
	config_str_list(&other, 0, 0, 0, CONFIG_ACT_APP, STRV("dep1"), &deps);
	config_str_list(&other, 0, 0, 0, CONFIG_ACT_APP, STRV("dep3"), &deps);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = dep1, dep2, dep3\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_unknown)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_val_t *v = arr_add(&other.vals, NULL);
	v->op		= 0;

	log_set_quiet(0, 1);
	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);
	log_set_quiet(0, 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, "");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_int)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_INT, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_int(&other, 0, 0, 0, CONFIG_ACT_SET, 0);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = 0\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_str)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	config_str(&other, 0, 0, 0, CONFIG_ACT_SET, STRV("path"));

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = path\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_merge_str_list)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR_LIST, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t other = {0};
	config_init(&other, 1, ALLOC_STD);
	uint deps = -1;
	config_str_list(&other, 0, 0, 0, CONFIG_ACT_SET, STRV("dep1"), &deps);
	config_str_list(&other, 0, 0, 0, CONFIG_ACT_SET, STRV("dep2"), &deps);

	EXPECT_EQ(config_merge(&config, &other, CONFIG_STATE_NULL, &schema, NULL), 0);

	char buf[256] = {0};
	config_print(&config, &schema, NULL, DST_BUF(buf));
	EXPECT_STR(buf, " = dep1, dep2\n");

	config_free(&config);
	config_free(&other);
	config_schema_free(&schema);

	END;
}

TEST(config_print)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	EXPECT_EQ(config_print(NULL, NULL, NULL, DST_NONE()), 0);

	char buf[256] = {0};
	EXPECT_EQ(config_print(&config, NULL, &registry, DST_BUF(buf)), 0);
	EXPECT_STR(buf, "");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_invalid_op)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_val_t *v = arr_add(&config.vals, NULL);
	v->op		= 0;

	char buf[256] = {0};
	log_set_quiet(0, 1);
	EXPECT_EQ(config_print(&config, NULL, &registry, DST_BUF(buf)), 0);
	log_set_quiet(0, 0);
	EXPECT_STR(buf, "");

	config_free(&config);
	registry_free(&registry);

	END;
}

TEST(config_print_invalid_type)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_UNKNOWN, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_val_t *v = arr_add(&config.vals, NULL);
	v->op		= 0;

	char buf[256] = {0};
	log_set_quiet(0, 1);
	EXPECT_EQ(config_print(&config, &schema, &registry, DST_BUF(buf)), 3);
	log_set_quiet(0, 0);
	EXPECT_STR(buf, " = ");

	config_free(&config);
	config_schema_free(&schema);
	registry_free(&registry);

	END;
}

TEST(config_print_invalid_mode)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_str(&config, 0, 0, 0, __CONFIG_ACT_CNT, STRV("path"));

	char buf[256] = {0};
	log_set_quiet(0, 1);
	EXPECT_NE(config_print(&config, &schema, NULL, DST_BUF(buf)), 0);
	log_set_quiet(0, 0);
	EXPECT_STR(buf, " = path\n");

	config_free(&config);
	config_schema_free(&schema);

	END;
}

TEST(config_print_invalid_scope)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR, CONFIG_SCOPE_UNKNOWN, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_str(&config, 0, 0, 0, __CONFIG_ACT_CNT, STRV("path"));

	char buf[256] = {0};
	log_set_quiet(0, 1);
	EXPECT_NE(config_print(&config, &schema, NULL, DST_BUF(buf)), 0);
	log_set_quiet(0, 0);
	EXPECT_STR(buf, "path\n");

	config_free(&config);
	config_schema_free(&schema);

	END;
}

TEST(config_print_scope)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 3, ALLOC_STD);
	uint pkg, tgt;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), &tgt);

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc[] = {
		{CONFIG_TYPE_INT, CONFIG_SCOPE_GLOBAL, STRV("g"), NULL},
		{CONFIG_TYPE_INT, CONFIG_SCOPE_PKG, STRV("p"), NULL},
		{CONFIG_TYPE_INT, CONFIG_SCOPE_TGT, STRV("t"), NULL},
	};
	config_schema_add_ops(&schema, desc, sizeof(desc));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_int(&config, 0, pkg, tgt, CONFIG_ACT_SET, 0);
	config_int(&config, 1, pkg, tgt, CONFIG_ACT_SET, 0);
	config_int(&config, 2, pkg, tgt, CONFIG_ACT_SET, 0);

	char buf[256] = {0};
	EXPECT_NE(config_print(&config, &schema, &registry, DST_BUF(buf)), 24);
	EXPECT_STR(buf,
		   "g = 0\n"
		   "pkg:p = 0\n"
		   "pkg:tgt:t = 0\n");

	config_free(&config);
	config_schema_free(&schema);
	registry_free(&registry);

	END;
}

TEST(config_print_int)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_INT, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_int(&config, 0, 0, 0, CONFIG_ACT_SET, 0);

	char buf[256] = {0};
	EXPECT_NE(config_print(&config, &schema, NULL, DST_BUF(buf)), 0);
	EXPECT_STR(buf, " = 0\n");

	config_free(&config);
	config_schema_free(&schema);

	END;
}

TEST(config_print_str)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_str(&config, 0, 0, 0, CONFIG_ACT_EN, STRV("path"));

	char buf[256] = {0};
	EXPECT_NE(config_print(&config, &schema, NULL, DST_BUF(buf)), 0);
	EXPECT_STR(buf, " ?= path\n");

	config_free(&config);
	config_schema_free(&schema);

	END;
}

TEST(config_print_str_list)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {CONFIG_TYPE_STR_LIST, CONFIG_SCOPE_GLOBAL, STRV(""), NULL};
	config_schema_add_ops(&schema, &desc, sizeof(config_schema_op_desc_t));

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	uint deps = -1;
	config_str_list(&config, 0, 0, 0, CONFIG_ACT_EN, STRV("dep1"), &deps);
	config_str_list(&config, 0, 0, 0, CONFIG_ACT_EN, STRV("dep2"), &deps);

	char buf[256] = {0};
	EXPECT_NE(config_print(&config, &schema, NULL, DST_BUF(buf)), 0);
	EXPECT_STR(buf, " ?= dep1, dep2\n");

	config_free(&config);
	config_schema_free(&schema);

	END;
}

STEST(config)
{
	SSTART;

	RUN(config_init_free);
	RUN(config_state);
	RUN(config_int);
	RUN(config_str);
	RUN(config_str_list);
	RUN(config_merge);
	RUN(config_merge_same_unknown);
	RUN(config_merge_same_int);
	RUN(config_merge_same_str);
	RUN(config_merge_same_str_list);
	RUN(config_merge_diff_mode);
	RUN(config_merge_diff_int);
	RUN(config_merge_diff_str);
	RUN(config_merge_diff_str_list_set);
	RUN(config_merge_diff_str_list_app);
	RUN(config_merge_unknown);
	RUN(config_merge_int);
	RUN(config_merge_str);
	RUN(config_merge_str_list);
	RUN(config_print);
	RUN(config_print_invalid_op);
	RUN(config_print_invalid_mode);
	RUN(config_print_invalid_type);
	RUN(config_print_invalid_scope);
	RUN(config_print_scope);
	RUN(config_print_int);
	RUN(config_print_str);
	RUN(config_print_str_list);

	SEND;
}
