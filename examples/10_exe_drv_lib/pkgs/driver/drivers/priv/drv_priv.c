#include "priv/drv_priv.h"

#include "lib.h"

#include <stdio.h>

int drv_priv()
{
	int ret = lib();
	printf("drv_priv\n");
	return ret;
}
