#ifndef MOD_H
#define MOD_H

#include "config.h"
#include "driver.h"
#include "fs.h"
#include "proc.h"
#include "proj.h"

typedef struct mod_s mod_t;

struct mod_s {
	strv_t name;
	int (*init)(mod_t *mod, uint cap, config_schema_t *config_schema, alloc_t alloc);
	int (*free)(mod_t *mod);
	int (*config_fs)(mod_t *mod, config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, fs_t *fs,
			 proc_t *proc, strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst);
	int (*apply_val)(mod_t *mod, const config_schema_t *schema, const config_t *config, const config_val_t *v, proj_t *proj);
	int (*proj_cfg)(mod_t *mod, proj_t *proj);
	uint ops;
	void *priv;
};

#define DRIVER_TYPE_MOD 2

#define MOD(_name, _data) DRIVER(_name, DRIVER_TYPE_MOD, _data)

#endif
