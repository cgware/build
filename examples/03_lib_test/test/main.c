#include "lib.h"
#include "priv/lib_priv.h"

#include <stdio.h>

int main()
{
	int ret = lib() + lib_priv();
	printf("test\n");
	return ret;
}
