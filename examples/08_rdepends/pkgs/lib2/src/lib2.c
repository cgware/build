#include "base.h"

#include <stdio.h>

int lib2()
{
	int ret = base();
	printf("lib2");
	return ret;
}
