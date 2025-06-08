#include "log.h"
#include "mem.h"
#include "test.h"

STEST(gen);
STEST(gen_make);
STEST(gen_none);
STEST(pkg);
STEST(pkg_loader);
STEST(pkgs);
STEST(proj);
STEST(proj_gen);
STEST(proj_loader);
STEST(target);
STEST(targets);
STEST(var);

TEST(cbuild)
{
	SSTART;
	RUN(gen);
	RUN(gen_make);
	RUN(gen_none);
	RUN(pkg);
	RUN(pkg_loader);
	RUN(pkgs);
	RUN(proj);
	RUN(proj_gen);
	RUN(proj_loader);
	RUN(target);
	RUN(targets);
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
