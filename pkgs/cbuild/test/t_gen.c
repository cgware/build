#include "gen.h"

#include "test.h"

TEST(gen_find_param)
{
	START;

	EXPECT_EQ(gen_find_param(STRV_NULL), NULL);
	EXPECT_NE(gen_find_param(STRV("N")), NULL);

	END;
}

STEST(gen)
{
	SSTART;

	RUN(gen_find_param);

	SEND;
}
