#include "driver.h"
#include "drv.h"

#include <stdio.h>

int main()
{
	int ret = 0;
	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type != 1) {
			continue;
		}
		drv_t *fn = i->data;
		ret += (*fn)();
	}
	printf("test\n");
	return ret;
}
