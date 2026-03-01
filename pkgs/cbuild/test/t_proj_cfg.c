#include "proj_cfg.h"

#include "log.h"
#include "mem.h"
#include "path.h"
#include "test.h"

TEST(proj_cfg_empty)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);

	EXPECT_EQ(proj_cfg(NULL, NULL, NULL), 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_crt)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	registry_add_pkg(&registry, STRV("name"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	EXPECT_EQ(proj.pkgs.cnt, 1);

	const pkg_t *pkg = proj_get_pkg(&proj, 0);

	strv_t val = proj_get_str(&proj, pkg->strs + PKG_STR_NAME);
	EXPECT_STRN(val.data, "name", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_oom)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	registry_add_pkg(&registry, STRV("name"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);

	mem_oom(1);
	proj.pkgs.cnt = proj.pkgs.cap;
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	proj.pkgs.cnt = 0;
	mem_oom(0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_crt_exists)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	registry_add_pkg(&registry, STRV("name"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_pkg(&config, 0, CONFIG_MODE_APP);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_en)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	registry_add_pkg(&registry, STRV("name"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_pkg(&config, 0, CONFIG_MODE_EN);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	EXPECT_EQ(proj.pkgs.cnt, 1);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_crt)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("name"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	EXPECT_EQ(proj.targets.cnt, 1);

	const target_t *target = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, target->strs + PKG_STR_NAME);
	EXPECT_STRN(val.data, "name", val.len);
	EXPECT_EQ(target->type, TARGET_TYPE_UNKNOWN);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_oom)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("name"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);

	mem_oom(1);
	proj.targets.cnt = proj.targets.cap;
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	proj.targets.cnt = 0;
	mem_oom(0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_crt_exists)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("name"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_en)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("name"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_EN);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	EXPECT_EQ(proj.targets.cnt, 1);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_path)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	registry_add_pkg(&registry, STRV("pkg"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_PKG_PATH, 0, -1, CONFIG_MODE_SET, STRV("path"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const pkg_t *pkg = proj_get_pkg(&proj, 0);

	strv_t val = proj_get_str(&proj, pkg->strs + PKG_STR_PATH);
	EXPECT_STRN(val.data, "path", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_path_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_PKG_PATH, 0, -1, CONFIG_MODE_SET, STRV("path"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_uri)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	registry_add_pkg(&registry, STRV("pkg"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_PKG_URI, 0, -1, CONFIG_MODE_SET, STRV("https://host.com/file.zip"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const pkg_t *pkg = proj_get_pkg(&proj, 0);

	strv_t val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://host.com/file.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "file.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "file", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_DIR);
	EXPECT_STRN(val.data, "file" SEP, val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_uri_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_PKG_URI, 0, -1, CONFIG_MODE_SET, STRV("https://host.com/file.zip"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_inc)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_PKG_INC, 0, -1, CONFIG_MODE_SET, STRV("include"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_INC);
	EXPECT_STRN(val.data, "include", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_inc_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_PKG_INC, 0, -1, CONFIG_MODE_SET, STRV("https://host.com/file.zip"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_dep_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 0, -1, CONFIG_MODE_SET, STRV("dep"), NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_dep_pkg)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint dep, pkg;
	registry_add_pkg(&registry, STRV("dep"), &dep);
	registry_add_tgt(&registry, dep, STRV("dep_tgt"), NULL);
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 4, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_pkg(&config, 1, CONFIG_MODE_APP);
	config_tgt(&config, 1, 1, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 1, -1, CONFIG_MODE_SET, STRV("dep"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_dep_tgt)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint dep, pkg;
	registry_add_pkg(&registry, STRV("dep"), &dep);
	registry_add_tgt(&registry, dep, STRV("dep_tgt"), NULL);
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 4, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_pkg(&config, 1, CONFIG_MODE_APP);
	config_tgt(&config, 1, 1, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 1, -1, CONFIG_MODE_SET, STRV("dep:dep_tgt"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_dep_not_test)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 4, ALLOC_STD);
	uint dep, pkg;
	registry_add_pkg(&registry, STRV("dep"), &dep);
	registry_add_tgt(&registry, dep, STRV("test"), NULL);
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 6, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_tgt_type(&config, 0, 0, CONFIG_MODE_SET, TARGET_TYPE_TST);
	config_pkg(&config, 1, CONFIG_MODE_APP);
	config_tgt(&config, 1, 1, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 1, -1, CONFIG_MODE_SET, STRV("dep"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_dep_pkg_not_found)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	registry_add_pkg(&registry, STRV("pkg"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 0, -1, CONFIG_MODE_SET, STRV("dep"), NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_pkg_dep_tgt_not_found)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	registry_add_pkg(&registry, STRV("dep"), NULL);
	registry_add_pkg(&registry, STRV("pkg"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_pkg(&config, 1, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_PKG_DEPS, 1, -1, CONFIG_MODE_SET, STRV("dep:dep_tgt"), NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_type_exe)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_tgt_type(&config, 0, 0, CONFIG_MODE_SET, TARGET_TYPE_EXE);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	EXPECT_EQ(tgt->type, TARGET_TYPE_EXE);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_type_exe_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_tgt_type(&config, 0, 0, CONFIG_MODE_SET, TARGET_TYPE_EXE);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_type_drv)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_tgt_type(&config, 0, 0, CONFIG_MODE_SET, TARGET_TYPE_DRV);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);
	EXPECT_EQ(tgt->type, TARGET_TYPE_DRV);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_type_tst)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 2, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_tgt_type(&config, 0, 0, CONFIG_MODE_SET, TARGET_TYPE_TST);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	EXPECT_EQ(tgt->type, TARGET_TYPE_TST);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_src)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_TGT_SRC, 0, 0, CONFIG_MODE_SET, STRV("src"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_SRC);
	EXPECT_STRN(val.data, "src", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_src_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_TGT_SRC, 0, 0, CONFIG_MODE_SET, STRV("src"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_inc)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_TGT_INC, 0, 0, CONFIG_MODE_SET, STRV("include"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_INC);
	EXPECT_STRN(val.data, "include", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_inc_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_TGT_INC, 0, 0, CONFIG_MODE_SET, STRV("include"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_inc_priv)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_TGT_INCS_PRIV, 0, 0, CONFIG_MODE_SET, STRV("src"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	target_t *tgt = proj_get_target(&proj, 0);
	uint *id      = list_get_at(&proj.lists, tgt->incs_priv, 0, NULL);
	strv_t val    = proj_get_str(&proj, *id);
	EXPECT_STRN(val.data, "src", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_inc_priv_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str_list(&config, CONFIG_OP_TYPE_TGT_INCS_PRIV, 0, 0, CONFIG_MODE_SET, STRV("src"), NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_dep_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str_list(&config, CONFIG_OP_TYPE_TGT_DEPS, 0, 0, CONFIG_MODE_SET, STRV("dep"), NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_dep_pkg)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint dep, pkg;
	registry_add_pkg(&registry, STRV("dep"), &dep);
	registry_add_tgt(&registry, dep, STRV("dep_tgt"), NULL);
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 5, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_pkg(&config, 1, CONFIG_MODE_APP);
	config_tgt(&config, 1, 1, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_TGT_DEPS, 1, 1, CONFIG_MODE_SET, STRV("dep"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_dep_tgt)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	uint dep, pkg;
	registry_add_pkg(&registry, STRV("dep"), &dep);
	registry_add_tgt(&registry, dep, STRV("dep_tgt"), NULL);
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 5, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_pkg(&config, 1, CONFIG_MODE_APP);
	config_tgt(&config, 1, 1, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_TGT_DEPS, 1, 1, CONFIG_MODE_SET, STRV("dep:dep_tgt"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_dep_pkg_not_test)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 4, ALLOC_STD);
	uint dep, pkg;
	registry_add_pkg(&registry, STRV("dep"), &dep);
	registry_add_tgt(&registry, dep, STRV("test"), NULL);
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 6, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_tgt_type(&config, 0, 0, CONFIG_MODE_SET, TARGET_TYPE_TST);
	config_pkg(&config, 1, CONFIG_MODE_APP);
	config_tgt(&config, 1, 1, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_TGT_DEPS, 1, 1, CONFIG_MODE_SET, STRV("dep"), NULL);

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_dep_pkg_not_found)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_TGT_DEPS, 0, 0, CONFIG_MODE_SET, STRV("dep"), NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_dep_tgt_not_found)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 2, ALLOC_STD);
	registry_add_pkg(&registry, STRV("dep"), NULL);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_pkg(&config, 1, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str_list(&config, CONFIG_OP_TYPE_TGT_DEPS, 1, 0, CONFIG_MODE_SET, STRV("dep:dep_tgt"), NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_prep)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_TGT_PREP, 0, 0, CONFIG_MODE_SET, STRV("prep"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_PREP);
	EXPECT_STRN(val.data, "prep", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_prep_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_TGT_PREP, 0, 0, CONFIG_MODE_SET, STRV("prep"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_conf)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_TGT_CONF, 0, 0, CONFIG_MODE_SET, STRV("conf"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_CONF);
	EXPECT_STRN(val.data, "conf", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_conf_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_TGT_CONF, 0, 0, CONFIG_MODE_SET, STRV("conf"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_comp)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_TGT_COMP, 0, 0, CONFIG_MODE_SET, STRV("comp"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_COMP);
	EXPECT_STRN(val.data, "comp", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_comp_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_TGT_COMP, 0, 0, CONFIG_MODE_SET, STRV("comp"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_inst)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_TGT_INST, 0, 0, CONFIG_MODE_SET, STRV("inst"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_INST);
	EXPECT_STRN(val.data, "inst", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_inst_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_TGT_INST, 0, 0, CONFIG_MODE_SET, STRV("inst"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_out)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);
	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_TGT_OUT, 0, 0, CONFIG_MODE_SET, STRV("out"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_OUT);
	EXPECT_STRN(val.data, "out", val.len);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_out_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_TGT_OUT, 0, 0, CONFIG_MODE_SET, STRV("out"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_lib)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_TGT_LIB, 0, 0, CONFIG_MODE_SET, STRV("lib"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_TGT);
	EXPECT_STRN(val.data, "lib", val.len);
	EXPECT_EQ(tgt->out_type, TARGET_TGT_TYPE_LIB);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_lib_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_TGT_LIB, 0, 0, CONFIG_MODE_SET, STRV("lib"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_exe)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);
	uint pkg;
	registry_add_pkg(&registry, STRV("pkg"), &pkg);
	registry_add_tgt(&registry, pkg, STRV("tgt"), NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 3, ALLOC_STD);
	config_pkg(&config, 0, CONFIG_MODE_APP);
	config_tgt(&config, 0, 0, CONFIG_MODE_APP);
	config_str(&config, CONFIG_OP_TYPE_TGT_EXE, 0, 0, CONFIG_MODE_SET, STRV("exe"));

	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 0);

	const target_t *tgt = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, tgt->strs + TGT_STR_TGT);
	EXPECT_STRN(val.data, "exe", val.len);
	EXPECT_EQ(tgt->out_type, TARGET_TGT_TYPE_EXE);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

TEST(proj_cfg_tgt_exe_invalid)
{
	START;

	registry_t registry = {0};
	registry_init(&registry, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, ALLOC_STD);
	config_str(&config, CONFIG_OP_TYPE_TGT_EXE, 0, 0, CONFIG_MODE_SET, STRV("lib"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config, &registry), 1);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);
	registry_free(&registry);

	END;
}

STEST(proj_cfg)
{
	SSTART;

	RUN(proj_cfg_empty);
	RUN(proj_cfg_pkg_crt);
	RUN(proj_cfg_pkg_oom);
	RUN(proj_cfg_pkg_crt_exists);
	RUN(proj_cfg_pkg_en);
	RUN(proj_cfg_tgt_crt);
	RUN(proj_cfg_tgt_oom);
	RUN(proj_cfg_tgt_crt_exists);
	RUN(proj_cfg_tgt_en);
	RUN(proj_cfg_pkg_path);
	RUN(proj_cfg_pkg_path_invalid);
	RUN(proj_cfg_pkg_uri);
	RUN(proj_cfg_pkg_uri_invalid);
	RUN(proj_cfg_pkg_inc);
	RUN(proj_cfg_pkg_inc_invalid);
	RUN(proj_cfg_pkg_dep_invalid);
	RUN(proj_cfg_pkg_dep_pkg);
	RUN(proj_cfg_pkg_dep_tgt);
	RUN(proj_cfg_pkg_dep_not_test);
	RUN(proj_cfg_pkg_dep_pkg_not_found);
	RUN(proj_cfg_pkg_dep_tgt_not_found);
	RUN(proj_cfg_tgt_type_exe);
	RUN(proj_cfg_tgt_type_exe_invalid);
	RUN(proj_cfg_tgt_type_drv);
	RUN(proj_cfg_tgt_type_tst);
	RUN(proj_cfg_tgt_src);
	RUN(proj_cfg_tgt_src_invalid);
	RUN(proj_cfg_tgt_inc);
	RUN(proj_cfg_tgt_inc_invalid);
	RUN(proj_cfg_tgt_inc_priv);
	RUN(proj_cfg_tgt_inc_priv_invalid);
	RUN(proj_cfg_tgt_dep_invalid);
	RUN(proj_cfg_tgt_dep_pkg);
	RUN(proj_cfg_tgt_dep_tgt);
	RUN(proj_cfg_tgt_dep_pkg_not_test);
	RUN(proj_cfg_tgt_dep_pkg_not_found);
	RUN(proj_cfg_tgt_dep_tgt_not_found);
	RUN(proj_cfg_tgt_prep);
	RUN(proj_cfg_tgt_prep_invalid);
	RUN(proj_cfg_tgt_conf);
	RUN(proj_cfg_tgt_conf_invalid);
	RUN(proj_cfg_tgt_comp);
	RUN(proj_cfg_tgt_comp_invalid);
	RUN(proj_cfg_tgt_inst);
	RUN(proj_cfg_tgt_inst_invalid);
	RUN(proj_cfg_tgt_out);
	RUN(proj_cfg_tgt_out_invalid);
	RUN(proj_cfg_tgt_lib);
	RUN(proj_cfg_tgt_lib_invalid);
	RUN(proj_cfg_tgt_exe);
	RUN(proj_cfg_tgt_exe_invalid);

	SEND;
}
