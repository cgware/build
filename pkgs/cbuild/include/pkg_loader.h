#ifndef PKG_LOADER
#define PKG_LOADER

#include "file/cfg.h"
#include "fs.h"
#include "pkgs.h"
#include "targets.h"

int pkg_load(uint id, fs_t *fs, strv_t dir, pkgs_t *pkgs, targets_t *targets, alloc_t alloc);
int pkg_set_cfg(uint id, const cfg_t *cfg, cfg_var_t root, pkgs_t *pkgs, targets_t *targets);

#endif
