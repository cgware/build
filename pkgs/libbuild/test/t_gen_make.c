#include "gen.h"

#include "file.h"
#include "log.h"
#include "mem.h"
#include "test.h"

#include <string.h>

TEST(pkg)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	pkg_t *pkg = proj_set_pkg(&proj);
	pkg_set_source(pkg, STRV("src"));

	gen_driver_t *drv = NULL;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			drv = i->data;
			if (str_eq(strc(drv->param, strlen(drv->param)), STR("M"))) {
				break;
			}
		}
	}

	PRINT_DST_FILE();

	drv->gen(&proj);

	proj_free(&proj);

	END;
}

STEST(gen_make)
{
	SSTART;

	RUN(pkg);

	SEND;
}
