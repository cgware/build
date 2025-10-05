#include "vars.h"

#include "log.h"
#include "mem.h"
#include "str.h"
#include "test.h"

TEST(vars_init)
{
	START;

	vars_t vars = {0};

	EXPECT_EQ(vars_init(NULL), NULL);
	EXPECT_EQ(vars_init(&vars), &vars);

	EXPECT_EQ(vars.vars[PKG_DIR].deps, (1ULL << PN));
	EXPECT_EQ(vars.vars[TGT_CMD].deps, (1 << PN) | (1 << TN));

	END;
}

TEST(vars_replace_null)
{
	START;

	EXPECT_EQ(vars_replace(NULL, NULL, NULL), 1);

	END;
}

TEST(vars_replace_none)
{
	START;

	vars_t vars = {0};
	vars_init(&vars);

	strv_t values[__VARS_CNT] = {0};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("bin"));

	EXPECT_EQ(vars_replace(&vars, &str, values), 0);
	EXPECT_STRN(str.data, "bin", str.len);

	END;
}

TEST(vars_replace_no_end)
{
	START;

	vars_t vars = {0};
	vars_init(&vars);

	strv_t values[__VARS_CNT] = {0};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${aaaaa"));

	EXPECT_EQ(vars_replace(&vars, &str, values), 0);
	EXPECT_STRN(str.data, "${aaaaa", str.len);

	END;
}

TEST(vars_replace_not_set)
{
	START;

	vars_t vars = {0};
	vars_init(&vars);

	strv_t values[__VARS_CNT] = {0};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${ARCH}"));

	EXPECT_EQ(vars_replace(&vars, &str, values), 0);
	EXPECT_STRN(str.data, "${ARCH}", str.len);

	END;
}

TEST(vars_replace_not_exists)
{
	START;

	vars_t vars = {0};
	vars_init(&vars);

	strv_t values[__VARS_CNT] = {0};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${NOT_EXISTS}"));

	EXPECT_EQ(vars_replace(&vars, &str, values), 0);
	EXPECT_STRN(str.data, "${NOT_EXISTS}", str.len);

	END;
}

TEST(vars_replace_oom)
{
	START;

	vars_t vars = {0};
	vars_init(&vars);

	strv_t values[__VARS_CNT] = {
		[ARCH] = STRVT("123456789"),
	};

	char buf[8] = {0};
	str_t str   = STRB(buf, 0);
	str_cat(&str, STRV("${ARCH}"));

	mem_oom(1);
	EXPECT_EQ(vars_replace(&vars, &str, values), 1);
	mem_oom(0);
	EXPECT_STRN(str.data, "${ARCH}", str.len);

	END;
}

TEST(vars_replace_empty)
{
	START;

	vars_t vars = {0};
	vars_init(&vars);

	strv_t values[__VARS_CNT] = {
		[ARCH] = STRVT(""),
	};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${ARCH}"));
	EXPECT_EQ(vars_replace(&vars, &str, values), 0);
	EXPECT_STRN(str.data, "", str.len);

	END;
}

TEST(vars_replace_same)
{
	START;

	vars_t vars = {0};
	vars_init(&vars);

	strv_t values[__VARS_CNT] = {
		[ARCH] = STRVT("${ARCH}"),
	};

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${ARCH}"));
	EXPECT_EQ(vars_replace(&vars, &str, values), 0);
	EXPECT_STRN(str.data, "${ARCH}", str.len);

	END;
}

TEST(var_convert_null)
{
	START;

	EXPECT_EQ(var_convert(NULL, '\0', '\0', '\0', '\0'), 1);

	END;
}

TEST(var_convert_short)
{
	START;

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${}"));

	EXPECT_EQ(var_convert(&str, '{', '}', '(', ')'), 0);

	EXPECT_STRN(str.data, "$()", str.len);

	END;
}

TEST(var_convert_short_no_end)
{
	START;

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${"));

	EXPECT_EQ(var_convert(&str, '{', '}', '(', ')'), 0);

	EXPECT_STRN(str.data, "${", str.len);

	END;
}

TEST(var_convert_one)
{
	START;

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${a}"));

	EXPECT_EQ(var_convert(&str, '{', '}', '(', ')'), 0);

	EXPECT_STRN(str.data, "$(a)", str.len);

	END;
}

TEST(var_convert_no_end)
{
	START;

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${a"));

	EXPECT_EQ(var_convert(&str, '{', '}', '(', ')'), 0);

	EXPECT_STRN(str.data, "${a", str.len);

	END;
}

TEST(var_convert_recursive)
{
	START;

	char buf[16] = {0};
	str_t str    = STRB(buf, 0);
	str_cat(&str, STRV("${${b}}"));

	EXPECT_EQ(var_convert(&str, '{', '}', '(', ')'), 0);

	EXPECT_STRN(str.data, "$($(b))", str.len);

	END;
}

STEST(vars)
{
	SSTART;

	RUN(vars_init);
	RUN(vars_replace_null);
	RUN(vars_replace_none);
	RUN(vars_replace_no_end);
	RUN(vars_replace_not_set);
	RUN(vars_replace_not_exists);
	RUN(vars_replace_oom);
	RUN(vars_replace_empty);
	RUN(vars_replace_same);
	RUN(var_convert_null);
	RUN(var_convert_short);
	RUN(var_convert_short_no_end);
	RUN(var_convert_one);
	RUN(var_convert_no_end);
	RUN(var_convert_recursive);

	SEND;
}
