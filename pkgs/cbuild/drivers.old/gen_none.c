#include "gen.h"

static int gen_none(const gen_driver_t *drv, const proj_t *proj, strv_t proj_dir, strv_t build_dir)
{
	(void)drv;
	(void)proj;
	(void)proj_dir;
	(void)build_dir;

	return 0;
}

static gen_driver_t none = {
	.param = STRVT("N"),
	.desc  = "None",
	.gen   = gen_none,
};

GEN_DRIVER(none, &none);
