#include "drv.h"
#include "driver.h"

#include <stdio.h>

static int drv()
{
	printf("drv\n");
	return 0;
}

static drv_t driver = drv;

DRIVER(driver, 1, &driver);
