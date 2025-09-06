#include "var.h"

#include "log.h"
#include "mem.h"
#include "str.h"
#include "test.h"

TEST(var_replace_null)
{
	START;

	EXPECT_EQ(var_replace(NULL, NULL), 1);

	END;
}

TEST(var_replace_none)
{
	START;

	strv_t values[__VAR_CNT] = {0};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("bin"));

	EXPECT_EQ(var_replace(&str, values), 0);
	EXPECT_STRN(str.data, "bin", str.len);

	END;
}

TEST(var_replace_no_end)
{
	START;

	strv_t values[__VAR_CNT] = {0};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${aaaaa"));

	EXPECT_EQ(var_replace(&str, values), 0);
	EXPECT_STRN(str.data, "${aaaaa", str.len);

	END;
}

TEST(var_replace_not_set)
{
	START;

	strv_t values[__VAR_CNT] = {0};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${ARCH}"));

	EXPECT_EQ(var_replace(&str, values), 0);
	EXPECT_STRN(str.data, "${ARCH}", str.len);

	END;
}

TEST(var_replace_not_exists)
{
	START;

	strv_t values[__VAR_CNT] = {0};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${NOT_EXISTS}"));

	EXPECT_EQ(var_replace(&str, values), 0);
	EXPECT_STRN(str.data, "${NOT_EXISTS}", str.len);

	END;
}

TEST(var_replace_oom)
{
	START;

	strv_t values[__VAR_CNT] = {
		[VAR_ARCH] = STRVT("123456789"),
	};

	char buf[8] = {0};
	str_t str   = STRB(buf, 0);
	str_cat(&str, STRV("${ARCH}"));

	mem_oom(1);
	EXPECT_EQ(var_replace(&str, values), 1);
	mem_oom(0);
	EXPECT_STRN(str.data, "${ARCH}", str.len);

	END;
}

TEST(var_replace_empty)
{
	START;

	strv_t values[__VAR_CNT] = {
		[VAR_ARCH] = STRVT(""),
	};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${ARCH}"));
	EXPECT_EQ(var_replace(&str, values), 0);
	EXPECT_STRN(str.data, "", str.len);

	END;
}

TEST(var_replace_same)
{
	START;

	strv_t values[__VAR_CNT] = {
		[VAR_ARCH] = STRVT("${ARCH}"),
	};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${ARCH}"));
	EXPECT_EQ(var_replace(&str, values), 0);
	EXPECT_STRN(str.data, "${ARCH}", str.len);

	END;
}

STEST(var)
{
	SSTART;

	RUN(var_replace_null);
	RUN(var_replace_none);
	RUN(var_replace_no_end);
	RUN(var_replace_not_set);
	RUN(var_replace_not_exists);
	RUN(var_replace_oom);
	RUN(var_replace_empty);
	RUN(var_replace_same);

	SEND;
}
