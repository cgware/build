#include "gen.h"

#include <stdio.h>

int gen_init()
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			gen_driver_t *drv = i->data;
			printf("%s = %s\n", drv->param, drv->desc);
			drv->gen(NULL);
		}
	}

	return 0;
}
