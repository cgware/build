#include "gen.h"

gen_driver_t *gen_find_param(strv_t param)
{
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			gen_driver_t *drv = i->data;
			if (strv_eq(drv->param, param)) {
				return i->data;
			}
		}
	}

	return NULL;
}
