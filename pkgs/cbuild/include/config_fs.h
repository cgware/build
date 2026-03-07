#ifndef CONFIG_FS
#define CONFIG_FS

#include "config.h"
#include "fs.h"
#include "proc.h"

int config_fs(config_t *config, config_t *tmp, const config_schema_t *schema, registry_t *registry, fs_t *fs, proc_t *proc,
	      strv_t proj_path, strv_t cur_path, strv_t name, str_t *buf, alloc_t alloc, dst_t dst);

#endif
