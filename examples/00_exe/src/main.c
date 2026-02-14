#include "priv/priv.h"

#include <stdio.h>

int main()
{
	int ret = priv();
	printf("exe\n");
	return ret;
}
