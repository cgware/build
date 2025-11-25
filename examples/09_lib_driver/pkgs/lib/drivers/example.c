#include "example.h"
#include "driver.h"

#include <stdio.h>

static void example_f()
{
	printf("Example\n");
}

static example_t example = {
	.f = example_f,
};

DRIVER(example, 1, &example);
