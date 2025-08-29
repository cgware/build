#ifndef VAR_H
#define VAR_H

#include "str.h"

typedef enum var_e {
	VAR_ARCH,
	VAR_CONFIG,
	__VAR_CNT,
} var_t;

int var_replace(str_t *str, const strv_t *values);

#endif
