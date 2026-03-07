#include "proj_cfg.h"

#include "log.h"
#include "mem.h"
#include "mod_base.h"
#include "path.h"
#include "test.h"

TEST(proj_cfg_empty)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	EXPECT_EQ(proj_cfg(NULL, NULL, NULL, NULL), 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 0);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_invalid_op)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_int(&config, 0, 0, 0, CONFIG_ACT_SET, 0);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_pkg_crt)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 0);

	EXPECT_EQ(proj.pkgs.cnt, 1);

	const pkg_t *pkg = proj_get_pkg(&proj, 0);

	strv_t val = proj_get_str(&proj, pkg->strs + PKG_STR_NAME);
	EXPECT_STRN(val.data, "name", val.len);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_pkg_oom)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);

	mem_oom(1);
	proj.pkgs.cnt = proj.pkgs.cap;
	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 1);
	proj.pkgs.cnt = 0;
	mem_oom(0);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_pkg_crt_exists)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_pkg_en)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_EN, STRV("name"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 0);

	EXPECT_EQ(proj.pkgs.cnt, 1);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_tgt_crt)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);
	config_str_list(&config, CONFIG_TGTS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 0);

	EXPECT_EQ(proj.targets.cnt, 1);

	const target_t *target = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, target->strs + PKG_STR_NAME);
	EXPECT_STRN(val.data, "name", val.len);
	EXPECT_EQ(target->type, TARGET_TYPE_UNKNOWN);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_tgt_oom)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("pkg"), NULL);
	config_str_list(&config, CONFIG_TGTS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);

	mem_oom(1);
	proj.targets.cnt = proj.targets.cap;
	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 1);
	proj.targets.cnt = 0;
	mem_oom(0);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_tgt_crt_exists)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);
	config_str_list(&config, CONFIG_TGTS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);
	config_str_list(&config, CONFIG_TGTS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_tgt_en)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);
	config_str_list(&config, CONFIG_TGTS, 0, 0, CONFIG_ACT_APP, STRV("name"), NULL);
	config_str_list(&config, CONFIG_TGTS, 0, 0, CONFIG_ACT_EN, STRV("name"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 0);

	EXPECT_EQ(proj.targets.cnt, 1);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_tgt_type_invalid)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_int(&config, CONFIG_TGT_TYPE, 0, 0, CONFIG_ACT_SET, TARGET_TYPE_EXE);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_tgt_type_exe)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("pkg"), NULL);
	config_str_list(&config, CONFIG_TGTS, 0, 0, CONFIG_ACT_APP, STRV("tgt"), NULL);
	config_int(&config, CONFIG_TGT_TYPE, 0, 0, CONFIG_ACT_SET, TARGET_TYPE_EXE);

	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	EXPECT_EQ(tgt->type, TARGET_TYPE_EXE);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_tgt_type_drv)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("pkg"), NULL);
	config_str_list(&config, CONFIG_TGTS, 0, 0, CONFIG_ACT_APP, STRV("tgt"), NULL);
	config_int(&config, CONFIG_TGT_TYPE, 0, 0, CONFIG_ACT_SET, TARGET_TYPE_DRV);

	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 0);

	const target_t *tgt = proj_get_target(&proj, 0);
	EXPECT_EQ(tgt->type, TARGET_TYPE_DRV);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_tgt_type_tst)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str_list(&config, CONFIG_PKGS, 0, 0, CONFIG_ACT_APP, STRV("pkg"), NULL);
	config_str_list(&config, CONFIG_TGTS, 0, 0, CONFIG_ACT_APP, STRV("tgt"), NULL);
	config_int(&config, CONFIG_TGT_TYPE, 0, 0, CONFIG_ACT_SET, TARGET_TYPE_TST);

	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	EXPECT_EQ(tgt->type, TARGET_TYPE_TST);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

TEST(proj_cfg_apply_op)
{
	START;

	config_schema_t schema = {0};
	config_schema_init(&schema, 9, ALLOC_STD);
	mod_base_init(0, &schema, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str(&config, CONFIG_PKG_PATH, 0, 0, CONFIG_ACT_SET, STRV("path"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &schema, NULL), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	config_schema_free(&schema);

	END;
}

STEST(proj_cfg)
{
	SSTART;

	RUN(proj_cfg_empty);
	RUN(proj_cfg_invalid_op);
	RUN(proj_cfg_pkg_crt);
	RUN(proj_cfg_pkg_oom);
	RUN(proj_cfg_pkg_crt_exists);
	RUN(proj_cfg_pkg_en);
	RUN(proj_cfg_tgt_crt);
	RUN(proj_cfg_tgt_oom);
	RUN(proj_cfg_tgt_crt_exists);
	RUN(proj_cfg_tgt_en);
	RUN(proj_cfg_tgt_type_invalid);
	RUN(proj_cfg_tgt_type_exe);
	RUN(proj_cfg_tgt_type_drv);
	RUN(proj_cfg_tgt_type_tst);
	RUN(proj_cfg_apply_op);

	SEND;
}
