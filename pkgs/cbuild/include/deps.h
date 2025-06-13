#ifndef DEPS_H
#define DEPS_H

#include "list.h"

typedef struct deps_s {
	list_t nodes;
} deps_t;

deps_t *deps_init(deps_t *deps, uint cnt);
void deps_free(deps_t *deps);



#endif
