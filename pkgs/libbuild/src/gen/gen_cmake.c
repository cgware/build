#include "gen.h"

#include <stdio.h>

static int gen_cmake(const proj_t *proj)
{
	(void)proj;
	printf("Gen cmake\n");
	return 0;
}

static gen_driver_t cmake = {
	.param = "C",
	.desc  = "CMake",
	.gen   = gen_cmake,
};

GEN_DRIVER(cmake, &cmake);
