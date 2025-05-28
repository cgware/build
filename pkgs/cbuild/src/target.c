#include "target.h"

#include "log.h"

target_t *target_init(target_t *target)
{
	if (target == NULL) {
		return NULL;
	}

	target->type	 = TARGET_TYPE_UNKNOWN;
	target->has_deps = 0;

	return target;
}

void target_free(target_t *target)
{
	if (target == NULL) {
		return;
	}
}

static const char *target_type_str[] = {
	[TARGET_TYPE_UNKNOWN] = "UNKNOWN",
	[TARGET_TYPE_EXE]     = "EXE",
	[TARGET_TYPE_LIB]     = "LIB",
};

size_t target_print(const target_t *target, dst_t dst)
{
	size_t off = dst.off;

	dst.off += dputf(dst,
			 "[target]\n"
			 "TYPE: %s\n",
			 target_type_str[target->type]);

	return dst.off - off;
}
