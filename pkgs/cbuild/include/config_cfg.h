#ifndef CONFIG_CFG_H
#define CONFIG_CFG_H

#include "config.h"
#include "file/cfg.h"
#include "fs.h"
#include "proc.h"

int config_cfg(config_t *config, cfg_t *cfg, cfg_var_t root, fs_t *fs, proc_t *proc, strv_t base_path, uint dir, str_t *buf, alloc_t alloc,
	       dst_t dst);

#endif
