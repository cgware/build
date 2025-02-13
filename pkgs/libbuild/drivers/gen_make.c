#include "gen.h"

#include "log.h"
#include "mem.h"
#include "var.h"

static int gen_make(const proj_t *proj)
{
	strv_t values[__VAR_CNT] = {
		[VAR_ARCH]   = STRVS("$(ARCH)"),
		[VAR_CONFIG] = STRVS("$(CONFIG)"),
	};

	path_t outdir = proj->outdir;

	str_t outdir_str = strb(outdir.data, sizeof(outdir.data) - 1, outdir.len);
	if (var_replace(&outdir_str, values)) {
		return 1;
	}

	outdir.len = outdir_str.len;

	return 0;
}

static gen_driver_t make = {
	.param = "M",
	.desc  = "Make",
	.gen   = gen_make,
};

GEN_DRIVER(make, &make);
