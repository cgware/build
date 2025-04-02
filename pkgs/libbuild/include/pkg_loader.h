#ifndef PKG_LOADER
#define PKG_LOADER

#include "file/cfg.h"
#include "pkgs.h"

int pkg_load(uint id, strv_t dir, pkgs_t *pkgs, alloc_t alloc);
int pkg_set_cfg(uint id, const cfg_t *cfg, cfg_var_t root, pkgs_t *pkgs);

#endif
