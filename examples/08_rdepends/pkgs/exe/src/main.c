#include "lib1.h"
#include "lib2.h"

#include <stdio.h>

int main()
{
	int ret = lib1() + lib2();
	printf("exe\n");
	return ret;
}
