#ifndef GEN_H
#define GEN_H

#include "driver.h"
#include "proj.h"

typedef struct gen_driver_s {
	const char *param;
	const char *desc;
	int (*gen)(const proj_t *proj);
} gen_driver_t;

#define GEN_DRIVER_TYPE 1

#define GEN_DRIVER(_name, _data) DRIVER(_name, GEN_DRIVER_TYPE, _data)

#endif
