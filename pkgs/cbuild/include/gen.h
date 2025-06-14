#ifndef GEN_H
#define GEN_H

#include "driver.h"
#include "fs.h"
#include "proj.h"

typedef struct gen_driver_s gen_driver_t;

struct gen_driver_s {
	strv_t param;
	const char *desc;
	int (*gen)(const gen_driver_t *drv, const proj_t *proj, strv_t proj_dir, strv_t build_dir);
	fs_t *fs;
};

#define GEN_DRIVER_TYPE 1

#define GEN_DRIVER(_name, _data) DRIVER(_name, GEN_DRIVER_TYPE, _data)

gen_driver_t *gen_find_param(strv_t param);

#endif
