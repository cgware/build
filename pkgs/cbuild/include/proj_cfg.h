#ifndef PROJ_CFG
#define PROJ_CFG

#include "file/cfg.h"
#include "fs.h"
#include "proc.h"
#include "proj.h"

int proj_cfg(proj_t *proj, cfg_t *cfg, cfg_var_t root, fs_t *fs, proc_t *proc, strv_t proj_dir, strv_t pkg_dir, strv_t pkg_name, str_t *buf,
	     alloc_t alloc);

#endif
