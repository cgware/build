#include "drv.h"
#include "driver.h"
#include "priv/drv_priv.h"

#include <stdio.h>

static int fn()
{
	int ret = drv_priv();
	printf("drv\n");
	return ret;
}

static drv_t driver = fn;

DRIVER(driver, 1, &driver);
