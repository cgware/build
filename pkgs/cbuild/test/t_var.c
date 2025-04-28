#include "var.h"

#include "log.h"
#include "mem.h"
#include "str.h"
#include "test.h"

TEST(var_replace)
{
	START;

	strv_t values[__VAR_CNT] = {0};

	str_t str = strz(16);

	EXPECT_EQ(var_replace(NULL, NULL), 1);

	{
		str.len = 0;
		str_cat(&str, STRV("bin"));

		EXPECT_EQ(var_replace(&str, values), 0);
		EXPECT_STRN(str.data, "bin", str.len);
	}

	{
		str.len = 0;
		str_cat(&str, STRV("${aaaaa"));

		EXPECT_EQ(var_replace(&str, values), 0);
		EXPECT_STRN(str.data, "${aaaaa", str.len);
	}

	{
		str.len = 0;
		str_cat(&str, STRV("${ARCH}"));

		EXPECT_EQ(var_replace(&str, values), 0);
		EXPECT_STRN(str.data, "", str.len);
	}

	{
		str.len = 0;
		str_cat(&str, STRV("${NOT_EXISTS}"));

		EXPECT_EQ(var_replace(&str, values), 0);
		EXPECT_STRN(str.data, "${NOT_EXISTS}", str.len);
	}

	{
		strv_t values_c[__VAR_CNT] = {
			[VAR_ARCH] = STRVT("123456789"),
		};
		size_t size = str.size;
		str.size    = 8;
		str.len	    = 0;
		str_cat(&str, STRV("${ARCH}"));
		mem_oom(1);
		EXPECT_EQ(var_replace(&str, values_c), 1);
		mem_oom(0);
		EXPECT_STRN(str.data, "${ARCH}", str.len);
		str.size = size;
	}

	str_free(&str);

	END;
}

STEST(var)
{
	SSTART;

	RUN(var_replace);

	SEND;
}
