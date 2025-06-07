#ifndef PROJ_LOADER
#define PROJ_LOADER

#include "file/cfg.h"
#include "fs.h"
#include "proc.h"
#include "proj.h"

int proj_load(fs_t *fs, proc_t *proc, strv_t dir, strv_t name, proj_t *proj, alloc_t alloc, str_t *buf);
int proj_set_cfg(proj_t *proj, const cfg_t *cfg, cfg_var_t root, fs_t *fs, proc_t *proc, alloc_t alloc, str_t *buf);

#endif
