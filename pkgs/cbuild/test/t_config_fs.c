#include "config_fs.h"

#include "test.h"

TEST(config_fs_empty)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 1, ALLOC_STD);

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	config_t tmp = {0};
	config_init(&tmp, 1, ALLOC_STD);

	EXPECT_EQ(config_fs(NULL, NULL, NULL, NULL, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD, DST_NONE()), 0);
	EXPECT_EQ(config_fs(&config, &tmp, &schema, &registry, NULL, NULL, STRV_NULL, STRV_NULL, STRV_NULL, NULL, ALLOC_STD, DST_NONE()),
		  0);

	config_free(&config);
	config_free(&tmp);
	registry_free(&registry);
	config_schema_free(&schema);

	END;
}

STEST(config_fs)
{
	SSTART;

	RUN(config_fs_empty);

	SEND;
}
