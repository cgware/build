#ifndef T_GEN_COMMON_H
#define T_GEN_COMMON_H

#include "gen.h"

typedef struct t_gen_common_s {
	fs_t fs;
	proj_t proj;
} t_gen_common_t;

int t_gen_proj_build_dir(t_gen_common_t *com, strv_t p);
int t_gen_proj_empty(t_gen_common_t *com, strv_t p);
int t_gen_proj_name(t_gen_common_t *com, strv_t p);
int t_gen_proj_unknown(t_gen_common_t *com, strv_t p);
int t_gen_proj_exe(t_gen_common_t *com, strv_t p);
int t_gen_proj_lib(t_gen_common_t *com, strv_t p);
int t_gen_proj_ext(t_gen_common_t *com, strv_t p);
int t_gen_proj_test(t_gen_common_t *com, strv_t p);
int t_gen_pkg_exe(t_gen_common_t *com, strv_t p);
int t_gen_pkg_lib(t_gen_common_t *com, strv_t p);
int t_gen_pkg_lib_test(t_gen_common_t *com, strv_t p);
int t_gen_pkg_multi(t_gen_common_t *com, strv_t p);
int t_gen_pkg_depends(t_gen_common_t *com, strv_t p);
int t_gen_pkg_rdepends(t_gen_common_t *com, strv_t p);
int t_gen_pkg_zip(t_gen_common_t *com, strv_t p);

void t_gen_free(t_gen_common_t *com);

#endif
