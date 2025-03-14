#include "gen.h"

#include "log.h"
#include "mem.h"
#include "test.h"

#include <string.h>

static int file;

void *test_open(print_dst_ex_t dst, const char *path, const char *mode)
{
	int _passed = 1;

	(void)mode;

	switch (file) {
	case 0:
		EXPECT_STR(path, "./tests/package/pkg.mk");
		break;
	case 1:
		EXPECT_STR(path, "./tests/package/Makefile");
		break;
	default:
		EXPECT_FAIL("Unexpected file: %s\n", path);
		break;
	}

	file++;

	return dst.dst.dst;
}

int test_close(print_dst_ex_t dst)
{
	printf("%.*s\n", (int)dst.dst.off, (char *)dst.dst.dst);
	printf("----------------------\n");

	return 0;
}

TEST(pkg)
{
	START;

	strv_t dir = STRV("./tests/package/");

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);
	proj_set_dir(&proj, dir);

	gen_driver_t *drvi = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			drvi = i->data;
			if (strv_eq(STRV(drvi->param), STRV("M"))) {
				break;
			}
		}
	}

	char buf[4096] = {0};

	gen_driver_t drv = *drvi;
	drv.dst		 = ((print_dst_ex_t){.dst = PRINT_DST_BUF(buf, sizeof(buf), 0), .open = test_open, .close = test_close});

	file = 0;

	drv.gen(&drv, &proj);

	proj_free(&proj);

	END;
}

STEST(gen_make)
{
	SSTART;

	RUN(pkg);

	SEND;
}
