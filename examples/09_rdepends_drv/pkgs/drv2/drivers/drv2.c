#include "base.h"

#include <stdio.h>

static int fn()
{
	int ret = base();
	printf("drv2");
	return ret;
}

static drv_t driver2 = fn;

DRIVER(driver2, 1, &driver2);
