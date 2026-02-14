#include "lib.h"

#include "priv/lib_priv.h"

#include <stdio.h>

int lib()
{
	int ret = lib_priv();
	printf("lib\n");
	return ret;
}
