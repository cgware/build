#include "args.h"
#include "gen.h"
#include "log.h"
#include "mem.h"
#include "path.h"
#include "proj.h"

int main(int argc, const char **argv)
{
	mem_stats_t mem_stats = {0};
	mem_stats_set(&mem_stats);

	c_print_init();

	log_t log = {0};
	log_set(&log);
	log_add_callback(log_std_cb, PRINT_DST_STD(), LOG_INFO, 1, 1);

	strv_t source = STRV(".");

	int gen = 0;

	int gen_drivers_cnt = 0;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			gen_drivers_cnt++;
		}
	}

	opt_enum_val_t *gens = mem_alloc(gen_drivers_cnt * sizeof(opt_enum_val_t));
	gen_drivers_cnt	     = 0;

	for (driver_t *i = DRIVER_START; i < DRIVER_END; i++) {
		if (i->type == GEN_DRIVER_TYPE) {
			gen_driver_t *drv     = i->data;
			gens[gen_drivers_cnt] = (opt_enum_val_t){
				.param = drv->param,
				.desc  = drv->desc,
				.priv  = drv,
			};
			gen_drivers_cnt++;
		}
	}

	const opt_enum_t gens_desc = {
		.name	   = "Generators",
		.vals	   = gens,
		.vals_size = gen_drivers_cnt * sizeof(opt_enum_val_t),
	};

	opt_t opts[] = {
		OPT('s', "source", OPT_STR, "<path>", "Specify source directory", &source, {0}, OPT_OPT),
		OPT('g', "generator", OPT_ENUM, "<generator>", "Specify build system generator", &gen, gens_desc, OPT_OPT),
	};

	if (args_parse(argc, argv, opts, sizeof(opts), PRINT_DST_STD())) {
		return 1;
	}

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	path_t path = {0};

	str_t dir = STRB(path.data, 0);
	if (fs_getcwd(&fs, &dir)) {
		return 1;
	}
	path.len = dir.len;

	path_merge(&path, source);

	if (proj_set_dir(&proj, &fs, STRVS(path))) {
		return 1;
	}

	proj_print(&proj, PRINT_DST_STD());

	gen_driver_t gen_driver = *(gen_driver_t *)gens[gen].priv;

	gen_driver.fs = &fs;
	gen_driver.gen(&gen_driver, &proj);

	proj_free(&proj);
	fs_free(&fs);

	mem_free(gens, gen_drivers_cnt * sizeof(opt_enum_val_t));

	mem_print(PRINT_DST_STD());
	return 0;
}
