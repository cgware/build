#ifndef PKG_LOADER
#define PKG_LOADER

#include "file/cfg.h"
#include "fs.h"
#include "pkgs.h"

pkg_t *pkg_load(fs_t *fs, strv_t proj_dir, strv_t dir, pkgs_t *pkgs, alloc_t alloc, str_t *buf);
int pkg_set_cfg(pkg_t *pkg, const cfg_t *cfg, cfg_var_t root, pkgs_t *pkgs, strv_t proj_dir, fs_t *fs);

#endif
