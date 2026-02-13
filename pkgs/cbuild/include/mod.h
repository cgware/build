#ifndef MOD_H
#define MOD_H

#include "config.h"
#include "driver.h"
#include "fs.h"

typedef struct mod_driver_s mod_driver_t;

struct mod_driver_s {
	int (*config_fs)(const mod_driver_t *drv, fs_t *fs, config_t *config, config_dir_t *dir, strv_t path);
	int (*proj_cfg)(const mod_driver_t *drv, fs_t *fs, config_t *config, config_dir_t *dir, strv_t path);
	fs_t *fs;
};

#define MOD_DRIVER_TYPE 2

#define MOD_DRIVER(_name, _data) DRIVER(_name, MOD_DRIVER_TYPE, _data)

mod_driver_t *mod_find_param(strv_t param);

#endif
