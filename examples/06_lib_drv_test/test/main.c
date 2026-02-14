#include "driver.h"
#include "drv/drv.h"
#include "lib/lib.h"

#include <stdio.h>

int main()
{
	int ret = lib();
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != 1) {
			continue;
		}
		drv_t *drv = i->data;
		ret += (*drv)();
	}
	printf("test\n");
	return ret;
}
