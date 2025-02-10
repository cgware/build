#include "gen.h"

#include <stdio.h>

static int gen_make(const proj_t *proj)
{
	(void)proj;
	printf("Gen make\n");
	return 0;
}

static gen_driver_t make = {
	.param = "M",
	.desc  = "Make",
	.gen   = gen_make,
};

GEN_DRIVER(make, &make);
