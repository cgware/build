#include "base.h"

#include <stdio.h>

static int fn()
{
	int ret = base();
	printf("base_drv");
	return ret;
}

static drv_t base_driver = fn;

DRIVER(base_driver, 1, &base_driver);
