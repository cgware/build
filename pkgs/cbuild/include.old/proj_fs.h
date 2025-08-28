#ifndef PROJ_FS_H
#define PROJ_FS_H

#include "fs.h"
#include "proc.h"
#include "proj.h"

int proj_fs(proj_t *proj, fs_t *fs, proc_t *proc, strv_t proj_dir, strv_t pkg_dir, strv_t pkg_name, str_t *buf, alloc_t alloc);
int proj_fs_child(proj_t *proj, fs_t *fs, proc_t *proc, strv_t proj_dir, strv_t pkg_dir, strv_t pkg_name, str_t *buf, alloc_t alloc);

int proj_fs_git(proj_t *proj, fs_t *fs, proc_t *proc, strv_t proj_dir, strv_t pkg_dir, strv_t pkg_name, strv_t url, str_t *buf,
		alloc_t alloc);

#endif
