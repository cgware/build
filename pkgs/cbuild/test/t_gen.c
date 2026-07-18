#include "gen.h"

#include "test.h"

TEST(gen_find_param)
{
	START;

	EXPECT_NULL(gen_find_param(STRV_NULL));
	EXPECT_NOT_NULL(gen_find_param(STRV("N")));

	END;
}

STEST(gen)
{
	SSTART;

	RUN(gen_find_param);

	SEND;
}
