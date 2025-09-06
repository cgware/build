#ifndef CONFIG_FS_H
#define CONFIG_FS_H

#include "config.h"
#include "fs.h"
#include "proc.h"

config_dir_t *config_fs(config_t *config, fs_t *fs, proc_t *proc, strv_t base_path, strv_t dir_path, strv_t name, str_t *buf, alloc_t alloc,
			dst_t dst);

#endif
