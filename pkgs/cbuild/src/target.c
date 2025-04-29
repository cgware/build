#include "target.h"

#include "log.h"

target_t *target_init(target_t *target)
{
	if (target == NULL) {
		return NULL;
	}

	target->type = TARGET_TYPE_UNKNOWN;
	target->deps = LIST_END;

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

int target_print(const target_t *target, print_dst_t dst)
{
	int off = dst.off;

	dst.off += c_dprintf(dst,
			     "[target]\n"
			     "TYPE: %s\n",
			     target_type_str[target->type]);

	return dst.off - off;
}
