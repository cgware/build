#include "log.h"
#include "mem.h"
#include "test.h"

STEST(config);
STEST(config_fs);
STEST(gen);
STEST(gen_cmake);
STEST(gen_make);
STEST(gen_none);
STEST(mod_cfg);
STEST(mod_drv);
STEST(mod_exe);
STEST(mod_lib);
STEST(mod_pkgs);
STEST(mod_test);
STEST(proj);
STEST(proj_cfg);
STEST(proj_gen);
STEST(proj_utils);
STEST(registry);
STEST(vars);

TEST(cbuild)
{
	SSTART;
	RUN(config);
	RUN(config_fs);
	RUN(gen);
	RUN(gen_make);
	RUN(gen_none);
	RUN(gen_cmake);
	RUN(mod_cfg);
	RUN(mod_drv);
	RUN(mod_exe);
	RUN(mod_lib);
	RUN(mod_pkgs);
	RUN(mod_test);
	RUN(proj);
	RUN(proj_cfg);
	RUN(proj_gen);
	RUN(proj_utils);
	RUN(registry);
	RUN(vars);
	SEND;
}

int main()
{
	c_print_init();

	log_t log = {0};
	log_set(&log);
	log_add_callback(log_std_cb, DST_STD(), LOG_WARN, 1, 1);

	t_init();

	t_run(test_cbuild, 1);

	int ret = t_finish();

	mem_print(DST_STD());

	if (mem_check()) {
		ret = 1;
	}

	return ret;
}
