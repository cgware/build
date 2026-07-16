#include "base.h"

#include <stdio.h>

static int fn()
{
	int ret = base();
	printf("drv1");
	return ret;
}

static drv_t driver1 = fn;

DRIVER(driver1, 1, &driver1);
