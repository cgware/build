#include "gen.h"

#include "test.h"

TEST(gen_none_empty)
{
	START;

	gen_driver_t drv = *gen_find_param(STRV("N"));

	EXPECT_EQ(drv.gen(&drv, NULL, STRV_NULL, STRV_NULL), 0);

	END;
}

STEST(gen_none)
{
	SSTART;

	RUN(gen_none_empty);

	SEND;
}
