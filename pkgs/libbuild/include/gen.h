#ifndef GEN_H
#define GEN_H

#include "driver.h"
#include "file.h"
#include "proj.h"

typedef struct gen_driver_s gen_driver_t;

struct gen_driver_s {
	const char *param;
	const char *desc;
	int (*gen)(const gen_driver_t *drv, const proj_t *proj);
	print_dst_ex_t dst;
};

#define GEN_DRIVER_TYPE 1

#define GEN_DRIVER(_name, _data) DRIVER(_name, GEN_DRIVER_TYPE, _data)

#endif
