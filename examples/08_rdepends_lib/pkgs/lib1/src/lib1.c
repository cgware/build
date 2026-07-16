#include "base.h"

#include <stdio.h>

int lib1()
{
	int ret = base();
	printf("lib1");
	return ret;
}
