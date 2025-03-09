#include "file.h"
#include "log.h"
#include "mem.h"
#include "test.h"

STEST(gen_make);
STEST(pkg);
STEST(proj);
STEST(var);

TEST(libbuild)
{
	SSTART;
	RUN(gen_make);
	RUN(pkg);
	RUN(proj);
	RUN(var);
	SEND;
}

int main()
{
	c_print_init();

	log_t log = {0};
	log_set(&log);
	log_add_callback(log_std_cb, PRINT_DST_FILE(stderr), LOG_WARN, 1, 1);

	t_init();

	t_run(test_libbuild, 1);

	int ret = t_finish();

	mem_print(PRINT_DST_STD());

	if (mem_check()) {
		ret = 1;
	}

	return ret;
}
