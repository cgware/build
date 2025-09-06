#include "log.h"
#include "mem.h"
#include "test.h"

STEST(config);
STEST(config_cfg);
STEST(config_fs);
STEST(gen);
STEST(gen_cmake);
STEST(gen_make);
STEST(gen_none);
STEST(proj);
STEST(proj_cfg);
STEST(proj_gen);
STEST(proj_utils);
STEST(var);

TEST(cbuild)
{
	SSTART;
	RUN(config);
	RUN(config_cfg);
	RUN(config_fs);
	RUN(gen);
	RUN(gen_make);
	RUN(gen_none);
	RUN(gen_cmake);
	RUN(proj);
	RUN(proj_cfg);
	RUN(proj_gen);
	RUN(proj_utils);
	RUN(var);
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
