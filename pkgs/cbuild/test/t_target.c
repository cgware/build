#include "target.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(target_init_free)
{
	START;

	target_t target = {0};

	EXPECT_EQ(target_init(NULL), NULL);
	EXPECT_EQ(target_init(&target), &target);

	target_free(&target);
	target_free(NULL);

	END;
}

TEST(target_print)
{
	START;

	target_t target = {0};
	target_init(&target);

	char buf[256] = {0};
	EXPECT_EQ(target_print(&target, PRINT_DST_BUF(buf, sizeof(buf), 0)), 23);
	EXPECT_STR(buf,
		   "[target]\n"
		   "TYPE: UNKNOWN\n");

	target_free(&target);

	END;
}

STEST(target)
{
	SSTART;

	RUN(target_init_free);
	RUN(target_print);

	SEND;
}
