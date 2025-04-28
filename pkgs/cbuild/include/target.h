#ifndef TARGET_H
#define TARGET_H

#include "arr.h"
#include "file/cfg.h"
#include "list.h"

typedef enum target_type_s {
	TARGET_TYPE_UNKNOWN,
	TARGET_TYPE_EXE,
	TARGET_TYPE_LIB,
	__TARGET_TYPE_MAX,
} target_type_t;

typedef struct target_s {
	uint pkg;
	target_type_t type;
	lnode_t deps;
} target_t;

target_t *target_init(target_t *target);
void target_free(target_t *target);

int target_print(const target_t *target, print_dst_t dst);

#endif
