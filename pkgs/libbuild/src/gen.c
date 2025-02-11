#include "gen.h"

#include <stdio.h>

int gen_proj(const proj_t *proj)
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			gen_driver_t *drv = i->data;
			drv->gen(proj);
		}
	}

	return 0;
}
