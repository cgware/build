#include "config_schema.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(config_schema_init_free)
{
	START;

	config_schema_t schema = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(config_schema_init(NULL, 0, ALLOC_STD), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(config_schema_init(&schema, 1, ALLOC_STD), NULL);
	mem_oom(0);
	EXPECT_EQ(config_schema_init(&schema, 1, ALLOC_STD), &schema);

	config_schema_free(&schema);
	config_schema_free(NULL);

	END;
}

TEST(config_schema_add_ops)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);

	config_schema_op_desc_t desc = {0};

	EXPECT_EQ(config_schema_add_ops(NULL, NULL, 0), 1);
	EXPECT_EQ(config_schema_add_ops(&schema, NULL, 0), 0);
	mem_oom(1);
	schema.strs.used = schema.strs.size;
	EXPECT_EQ(config_schema_add_ops(&schema, &desc, sizeof(desc)), 1);
	schema.strs.used = 0;
	schema.ops.cnt	 = schema.ops.cap;
	EXPECT_EQ(config_schema_add_ops(&schema, &desc, sizeof(desc)), 1);
	schema.ops.cnt = 0;
	mem_oom(0);
	EXPECT_EQ(config_schema_add_ops(&schema, &desc, sizeof(desc)), 0);

	config_schema_free(&schema);

	END;
}

TEST(config_schema_get_op)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);
	config_schema_op_desc_t desc = {0};
	config_schema_add_ops(&schema, &desc, sizeof(desc));

	EXPECT_EQ(config_schema_get_op(NULL, 0), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_schema_get_op(&schema, schema.ops.cnt), NULL);
	log_set_quiet(0, 0);
	EXPECT_NE(config_schema_get_op(&schema, 0), NULL);

	config_schema_free(&schema);

	END;
}

STEST(config_schema)
{
	SSTART;

	RUN(config_schema_init_free);
	RUN(config_schema_add_ops);
	RUN(config_schema_get_op);

	SEND;
}
