#include "config.h"

#include "log.h"
#include "mem.h"
#include "test.h"

TEST(config_init_free)
{
	START;

	config_t config = {0};

	log_set_quiet(0, 1);
	EXPECT_EQ(config_init(NULL, 0, 0, 0, ALLOC_STD), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(config_init(&config, 1, 0, 0, ALLOC_STD), NULL);
	mem_oom(0);
	EXPECT_EQ(config_init(&config, 1, 1, 1, ALLOC_STD), &config);

	config_free(&config);
	config_free(NULL);

	END;
}

TEST(config_add_dir)
{
	START;

	config_t config = {0};
	log_set_quiet(0, 1);
	config_init(&config, 0, 0, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	uint id;

	EXPECT_EQ(config_add_dir(NULL, NULL), NULL);
	mem_oom(1);
	EXPECT_EQ(config_add_dir(&config, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(config_add_dir(&config, NULL), NULL);
	EXPECT_NE(config_add_dir(&config, &id), NULL);
	EXPECT_EQ(id, 1);

	config_free(&config);

	END;
}

TEST(config_get_dir)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint id;
	config_dir_t *dir = config_add_dir(&config, &id);

	EXPECT_EQ(config_get_dir(NULL, id), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_get_dir(&config, config.dirs.cnt), NULL);
	log_set_quiet(0, 0);
	EXPECT_EQ(config_get_dir(&config, id), dir);

	config_free(&config);

	END;
}

TEST(config_add_pkg)
{
	START;

	config_t config = {0};
	log_set_quiet(0, 1);
	config_init(&config, 0, 0, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	uint dir = 0;
	list_node_t id;

	EXPECT_EQ(config_add_pkg(NULL, config.dirs.cnt, NULL), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_add_pkg(&config, config.dirs.cnt, NULL), NULL);
	log_set_quiet(0, 0);
	config_add_dir(&config, &dir);
	mem_oom(1);
	EXPECT_EQ(config_add_pkg(&config, dir, NULL), NULL);
	mem_oom(0);
	EXPECT_NE(config_add_pkg(&config, dir, NULL), NULL);
	EXPECT_NE(config_add_pkg(&config, dir, &id), NULL);
	EXPECT_EQ(id, 1);

	config_free(&config);

	END;
}

TEST(config_get_pkg)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);
	list_node_t id;
	config_pkg_t *pkg = config_add_pkg(&config, dir, &id);

	EXPECT_EQ(config_get_pkg(NULL, id), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_get_pkg(&config, config.pkgs.cnt), NULL);
	log_set_quiet(0, 0);
	EXPECT_EQ(config_get_pkg(&config, id), pkg);

	config_free(&config);

	END;
}

TEST(config_add_target)
{
	START;

	config_t config = {0};
	log_set_quiet(0, 1);
	config_init(&config, 0, 0, 1, ALLOC_STD);
	log_set_quiet(0, 0);

	uint dir	= 0;
	list_node_t pkg = 0;
	list_node_t id;

	config_add_dir(&config, &dir);
	config_add_pkg(&config, dir, &pkg);

	EXPECT_EQ(config_add_target(NULL, config.pkgs.cnt, NULL), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_add_target(&config, config.pkgs.cnt, NULL), NULL);
	log_set_quiet(0, 0);
	mem_oom(1);
	config.targets.cnt = config.targets.cap;
	EXPECT_EQ(config_add_target(&config, pkg, NULL), NULL);
	config.targets.cnt   = 0;
	config.strs.buf.used = config.strs.buf.size;
	EXPECT_EQ(config_add_target(&config, pkg, NULL), NULL);
	config.strs.buf.used = 0;
	mem_oom(0);
	EXPECT_NE(config_add_target(&config, pkg, NULL), NULL);
	EXPECT_NE(config_add_target(&config, pkg, &id), NULL);
	EXPECT_EQ(id, 1);

	config_free(&config);

	END;
}

TEST(config_get_target)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);
	list_node_t pkg;
	config_add_pkg(&config, dir, &pkg);
	list_node_t id;
	config_target_t *target = config_add_target(&config, pkg, &id);

	EXPECT_EQ(config_get_target(NULL, id), NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_get_target(&config, config.targets.cnt), NULL);
	log_set_quiet(0, 0);
	EXPECT_EQ(config_get_target(&config, id), target);

	config_free(&config);

	END;
}

TEST(config_add_dep)
{
	START;

	config_t config = {0};
	log_set_quiet(0, 1);
	config_init(&config, 0, 0, 0, ALLOC_STD);
	log_set_quiet(0, 0);

	uint dir	= 0;
	list_node_t pkg = 0;

	config_add_dir(&config, &dir);
	config_add_pkg(&config, dir, &pkg);

	EXPECT_EQ(config_add_dep(NULL, config.pkgs.cnt, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_add_dep(&config, config.pkgs.cnt, STRV_NULL), 1);
	log_set_quiet(0, 0);
	mem_oom(1);
	EXPECT_EQ(config_add_dep(&config, pkg, STRV_NULL), 1);
	mem_oom(0);
	EXPECT_EQ(config_add_dep(&config, pkg, STRV_NULL), 0);
	EXPECT_EQ(config_add_dep(&config, pkg, STRV_NULL), 0);

	config_free(&config);

	END;
}

TEST(config_set_str)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);
	config_pkg_t *pkg = config_add_pkg(&config, dir, NULL);

	EXPECT_EQ(config_set_str(NULL, config.strs.off.cnt, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_set_str(&config, config.strs.off.cnt, STRV_NULL), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(config_set_str(&config, pkg->strs + CONFIG_PKG_NAME, STRV_NULL), 0);

	config_free(&config);

	END;
}

TEST(config_get_str)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	config_add_dir(&config, &dir);
	config_pkg_t *pkg = config_add_pkg(&config, dir, NULL);

	EXPECT_EQ(config_get_str(NULL, config.strs.off.cnt).data, NULL);
	log_set_quiet(0, 1);
	EXPECT_EQ(config_get_str(&config, config.strs.off.cnt).data, NULL);
	log_set_quiet(0, 0);
	strv_t name = config_get_str(&config, pkg->strs + CONFIG_PKG_NAME);
	EXPECT_STRN(name.data, "", name.len);

	config_free(&config);

	END;
}

TEST(config_print)
{
	START;

	config_t config = {0};
	config_init(&config, 1, 1, 1, ALLOC_STD);

	uint dir;
	list_node_t pkg;
	config_add_dir(&config, &dir);
	config_add_pkg(&config, dir, &pkg);
	config_add_target(&config, pkg, NULL);
	config_add_dep(&config, pkg, STRV("dep"));

	EXPECT_EQ(config_print(NULL, DST_NONE()), 0);

	char buf[256] = {0};
	EXPECT_EQ(config_print(&config, DST_BUF(buf)), 125);
	EXPECT_STR(buf,
		   "[dir]\n"
		   "NAME: \n"
		   "PATH: \n"
		   "SRC: \n"
		   "MAIN: 0\n"
		   "INC: \n"
		   "DRV: \n"
		   "TEST: \n"
		   "\n"
		   "[pkg]\n"
		   "NAME: \n"
		   "URI: \n"
		   "INC: \n"
		   "DEPS: dep\n"
		   "\n"
		   "[target]\n"
		   "NAME: \n"
		   "CMD: \n"
		   "OUT: \n"
		   "DST: \n"
		   "\n")

	config_free(&config);

	END;
}

STEST(config)
{
	SSTART;

	RUN(config_init_free);
	RUN(config_add_dir);
	RUN(config_get_dir);
	RUN(config_add_pkg);
	RUN(config_get_pkg);
	RUN(config_add_target);
	RUN(config_get_target);
	RUN(config_add_dep);
	RUN(config_set_str);
	RUN(config_get_str);
	RUN(config_print);

	SEND;
}
