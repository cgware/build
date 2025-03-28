#include "var.h"

#include "log.h"
#include "str.h"
#include "test.h"

TEST(var_replace)
{
	START;

	strv_t values[__VAR_CNT] = {0};

	EXPECT_EQ(var_replace(NULL, NULL), 1);

	{
		char buf[16] = "bin";
		str_t str    = strb(buf, sizeof(buf), 3);

		EXPECT_EQ(var_replace(&str, values), 0);
		EXPECT_STRN(str.data, "bin", str.len);
	}

	{
		char buf[16] = "${aaaaa";
		str_t str    = strb(buf, sizeof(buf), 7);

		EXPECT_EQ(var_replace(&str, values), 0);
		EXPECT_STRN(str.data, "${aaaaa", str.len);
	}

	{
		char buf[16] = "${ARCH}";
		str_t str    = strb(buf, sizeof(buf), 7);

		EXPECT_EQ(var_replace(&str, values), 0);
		EXPECT_STRN(str.data, "", str.len);
	}

	{
		char buf[16] = "${NOT_EXISTS}";
		str_t str    = strb(buf, sizeof(buf), 13);

		EXPECT_EQ(var_replace(&str, values), 0);
		EXPECT_STRN(str.data, "${NOT_EXISTS}", str.len);
	}

	{
		strv_t values_c[__VAR_CNT] = {
			[VAR_ARCH] = STRVS("123456789"),
		};
		char buf[8] = "${ARCH}";
		str_t str   = strb(buf, sizeof(buf), 7);

		log_set_quiet(0, 1);
		EXPECT_EQ(var_replace(&str, values_c), 1);
		log_set_quiet(0, 0);
		EXPECT_STRN(str.data, "${ARCH}", str.len);
	}

	END;
}

STEST(var)
{
	SSTART;

	RUN(var_replace);

	SEND;
}
