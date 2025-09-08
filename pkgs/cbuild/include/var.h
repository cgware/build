#ifndef VAR_H
#define VAR_H

#include "str.h"

typedef enum vars_e {
	ARCH,
	CONFIG,
	// PROJ
	DIR_PROJ,
	DIR_BUILD,
	// PKG
	PKG_DIR,
	PKG_URI,
	PKG_DLFILE,
	PKG_DLROOT,
	// TGT
	TGT_CMD,
	TGT_OUT,
	// DIR_TMP
	DIR_TMP,
	DIR_TMP_EXT,
	DIR_TMP_EXT_PKG,
	DIR_TMP_EXT_PKG_ROOT,
	DIR_TMP_EXT_PKG_ROOT_OUT,
	DIR_TMP_REP,
	DIR_TMP_COV,
	DIR_TMP_DL,
	DIR_TMP_DL_PKG,
	// DIR_PKG
	DIR_PKG,
	DIR_PKG_SRC,
	DIR_PKG_INC,
	DIR_PKG_DRV,
	DIR_PKG_TST,
	// DIR_OUT
	DIR_OUT,
	DIR_OUT_INT,
	DIR_OUT_INT_SRC,
	DIR_OUT_INT_DRV,
	DIR_OUT_INT_TST,
	DIR_OUT_LIB,
	DIR_OUT_LIB_FILE,
	DIR_OUT_BIN,
	DIR_OUT_BIN_FILE,
	DIR_OUT_EXT,
	DIR_OUT_EXT_PKG,
	DIR_OUT_EXT_FILE,
	DIR_OUT_TST,
	DIR_OUT_TST_FILE,
	__VARS_CNT,
} vars_t;

typedef struct var_s {
	strv_t name;
	strv_t val;
	int pkg;
	int tgt;
} var_t;

extern const var_t g_vars[__VARS_CNT];

int var_replace(str_t *str, const strv_t *values);
int var_convert(str_t *str, char froms, char frome, char tos, char toe);

#endif
