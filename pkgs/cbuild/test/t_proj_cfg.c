#include "proj_cfg.h"

#include "log.h"
#include "test.h"

TEST(proj_cfg_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	EXPECT_EQ(proj_cfg(NULL, NULL), 1);
	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_dir_empty)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_src)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);

	config_set_str(&config, d->strs + CONFIG_DIR_SRC, STRV("src"));

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	const pkg_t *pkg       = proj_get_pkg(&proj, 0);
	const target_t *target = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, pkg->strs + PKG_SRC);
	EXPECT_STRN(val.data, "src", val.len);
	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_include)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);

	config_set_str(&config, d->strs + CONFIG_DIR_INC, STRV("include"));

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	const pkg_t *pkg       = proj_get_pkg(&proj, 0);
	const target_t *target = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, pkg->strs + PKG_INC);
	EXPECT_STRN(val.data, "include", val.len);
	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_src_include)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);

	config_set_str(&config, d->strs + CONFIG_DIR_SRC, STRV("src"));
	config_set_str(&config, d->strs + CONFIG_DIR_INC, STRV("include"));

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	const target_t *target = proj_get_target(&proj, 0);

	EXPECT_EQ(target->type, TARGET_TYPE_LIB);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_src_include_main)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);

	config_set_str(&config, d->strs + CONFIG_DIR_SRC, STRV("src"));
	config_set_str(&config, d->strs + CONFIG_DIR_INC, STRV("include"));
	d->has_main = 1;

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	const target_t *target = proj_get_target(&proj, 0);

	EXPECT_EQ(target->type, TARGET_TYPE_EXE);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_test)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);

	config_set_str(&config, d->strs + CONFIG_DIR_TST, STRV("test"));

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	const pkg_t *pkg       = proj_get_pkg(&proj, 0);
	const target_t *target = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, pkg->strs + PKG_TST);
	EXPECT_STRN(val.data, "test", val.len);
	EXPECT_EQ(target->type, TARGET_TYPE_TST);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);
	config_set_str(&config, d->strs + CONFIG_DIR_NAME, STRV("name"));

	list_node_t cpkg;
	config_add_pkg(&config, dir, &cpkg);

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	const pkg_t *pkg = proj_get_pkg(&proj, 0);

	strv_t val = proj_get_str(&proj, pkg->strs + PKG_NAME);
	EXPECT_STRN(val.data, "name", val.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_uri)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	list_node_t cpkg;
	config_pkg_t *p = config_add_pkg(&config, dir, &cpkg);

	config_set_str(&config, p->strs + CONFIG_PKG_URI, STRV("https://host.com/file.zip"));

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	const pkg_t *pkg = proj_get_pkg(&proj, 0);

	strv_t val = proj_get_str(&proj, pkg->strs + PKG_URI_STR);
	EXPECT_STRN(val.data, "https://host.com/file.zip", val.len);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);
	config_set_str(&config, d->strs + CONFIG_DIR_NAME, STRV("name"));

	list_node_t cpkg;
	config_add_pkg(&config, dir, &cpkg);

	config_add_target(&config, cpkg, NULL);

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	const target_t *target = proj_get_target(&proj, 0);

	strv_t val = proj_get_str(&proj, target->strs + PKG_NAME);
	EXPECT_STRN(val.data, "name", val.len);
	EXPECT_EQ(target->type, TARGET_TYPE_UNKNOWN);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_mod)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);

	config_set_str(&config, d->strs + CONFIG_DIR_SRC, STRV("src"));

	list_node_t cpkg;
	config_add_pkg(&config, dir, &cpkg);
	config_add_target(&config, cpkg, NULL);

	EXPECT_EQ(proj_cfg(&proj, &config), 0);
	EXPECT_EQ(proj.targets.cnt, 1);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_target_ext)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	list_node_t cpkg;
	config_pkg_t *p = config_add_pkg(&config, dir, &cpkg);

	config_set_str(&config, p->strs + CONFIG_PKG_URI, STRV("https://host.com/file.zip"));

	list_node_t ctarget;
	config_add_target(&config, cpkg, &ctarget);

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	const target_t *target = proj_get_target(&proj, 0);

	EXPECT_EQ(target->type, TARGET_TYPE_EXT);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_dep)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);
	config_set_str(&config, d->strs + CONFIG_PKG_NAME, STRV("dep"));
	config_set_str(&config, d->strs + CONFIG_DIR_SRC, STRV("src"));

	list_node_t cpkg;
	config_add_pkg(&config, dir, &cpkg);

	d = config_add_dir(&config, &dir);
	config_set_str(&config, d->strs + CONFIG_DIR_SRC, STRV("src"));

	list_node_t cpkg2;
	config_add_pkg(&config, dir, &cpkg2);
	config_add_dep(&config, cpkg2, STRV("dep"));

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_dep_not_found)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);

	list_node_t cpkg;
	config_add_pkg(&config, dir, &cpkg);
	config_add_dep(&config, cpkg, STRV("dep"));

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_cfg(&proj, &config), 0);
	log_set_quiet(0, 0);

	config_free(&config);
	proj_free(&proj);

	END;
}

TEST(proj_cfg_pkg_dep_test)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_dir_t *d = config_add_dir(&config, &dir);
	config_set_str(&config, d->strs + CONFIG_DIR_SRC, STRV("src"));
	config_set_str(&config, d->strs + CONFIG_DIR_INC, STRV("include"));
	config_set_str(&config, d->strs + CONFIG_DIR_TST, STRV("test"));

	EXPECT_EQ(proj_cfg(&proj, &config), 0);

	arr_t deps = {0};
	arr_init(&deps, 1, sizeof(list_node_t), ALLOC_STD);
	proj_get_deps(&proj, 1, &deps);
	EXPECT_EQ(deps.cnt, 1);
	EXPECT_EQ(*(list_node_t *)arr_get(&deps, 0), 0);
	arr_free(&deps);

	config_free(&config);
	proj_free(&proj);

	END;
}

STEST(proj_cfg)
{
	SSTART;

	RUN(proj_cfg_empty);
	RUN(proj_cfg_dir_empty);
	RUN(proj_cfg_src);
	RUN(proj_cfg_include);
	RUN(proj_cfg_src_include);
	RUN(proj_cfg_src_include_main);
	RUN(proj_cfg_test);
	RUN(proj_cfg_pkg);
	RUN(proj_cfg_uri);
	RUN(proj_cfg_target);
	RUN(proj_cfg_target_mod);
	RUN(proj_cfg_target_ext);
	RUN(proj_cfg_pkg_dep);
	RUN(proj_cfg_pkg_dep_not_found);
	RUN(proj_cfg_pkg_dep_test);

	SEND;
}
