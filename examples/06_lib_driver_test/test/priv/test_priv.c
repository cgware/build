#include "priv/test_priv.h"

#include "drv/drv.h"
#include "lib/lib.h"
#include "priv/drv_priv.h"
#include "priv/lib_priv.h"

#include <stdio.h>

int test_priv()
{
	int ret = lib_priv() + lib() + drv_priv() + drv();
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != 1) {
			continue;
		}
		drv_t *drv = i->data;
		ret += (*drv)();
	}
	printf("test_priv\n");
	return ret;
}
