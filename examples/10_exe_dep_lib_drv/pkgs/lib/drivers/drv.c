#include "drv.h"
#include "driver.h"

#include <stdio.h>

static int fn()
{
	printf("drv\n");
	return 0;
}

static drv_t driver = fn;

DRIVER(driver, 1, &driver);
