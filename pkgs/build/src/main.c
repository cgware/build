#include "args.h"
#include "file.h"
#include "gen.h"
#include "log.h"
#include "mem.h"
#include "path.h"
#include "proj.h"

#include <string.h>

int main(int argc, const char **argv)
{
	mem_stats_t mem_stats = {0};
	mem_stats_set(&mem_stats);

	c_print_init();

	log_t log = {0};
	log_set(&log);
	log_add_callback(log_std_cb, PRINT_DST_FILE(stderr), LOG_INFO, 1, 1);

	const char *source = ".";

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
			gens[gen_drivers_cnt] = (opt_enum_val_t){drv->param, drv->desc, drv};
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

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);
	path_init(&proj.dir, STRVN(source, strlen(source)));
	path_init(&proj.outdir, STRV("bin" SEP "${ARCH}-${CONFIG}/"));

	path_t tmp = {0};
	path_init(&tmp, STRVN(source, strlen(source)));
	path_child(&tmp, STRV("src"));
	int is_src = path_is_dir(&tmp);

	path_init(&tmp, STRVN(source, strlen(source)));
	path_child(&tmp, STRV("pkgs"));
	int is_pkgs = path_is_dir(&tmp);

	if (!is_src && !is_pkgs) {
		log_error("build", "main", NULL, "No 'src' or 'pkgs' folder found: %s\n", source);
		return 1;
	} else if (is_src && is_pkgs) {
		log_error("build", "main", NULL, "Only one of 'src' or 'pkgs' folder expected: %s\n", source);
		return 1;
	}

	if (is_src) {
		pkg_t *pkg = proj_set_pkg(&proj);

		if (pkg_set_source(pkg, STRVN(source, strlen(source)))) {
			return 1;
		}
	}

	if (is_pkgs) {
		// TODO
	}

	proj_print(&proj, PRINT_DST_STD());

	gen_driver_t *gen_driver = gens[gen].priv;
	gen_driver->gen(&proj);

	proj_free(&proj);

	mem_free(gens, gen_drivers_cnt * sizeof(opt_enum_val_t));

	mem_print(PRINT_DST_STD());

	return 0;
}
