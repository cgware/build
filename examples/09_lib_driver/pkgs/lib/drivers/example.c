#include "example.h"
#include "driver.h"

#include <stdio.h>

void example_f()
{
	printf("Example\n");
}

static example_t example = {
	.f = example_f,
};

DRIVER(example, 0, &example);
