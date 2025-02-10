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

	gen_init();

	const char *source = ".";
	const char *build  = "./bin";
	gen_t gen	   = GEN_MAKE;

	const opt_enum_val_t gens[] = {
		[GEN_MAKE] = {"M", "Make"},
	};

	const opt_enum_t gens_desc = {
		.name	   = "Generators",
		.vals	   = gens,
		.vals_size = sizeof(gens),
	};

	opt_t opts[] = {
		OPT('s', "source", OPT_STR, "<path>", "Specify source directory", &source, {0}, OPT_OPT),
		OPT('b', "build", OPT_STR, "<path>", "Specify build directory", &build, {0}, OPT_OPT),
		OPT('g', "generator", OPT_ENUM, "<generator>", "Specify build system generator", &gen, gens_desc, OPT_OPT),
	};

	if (args_parse(argc, argv, opts, sizeof(opts), PRINT_DST_STD())) {
		return 1;
	}

	proj_t proj = {0};
	proj_init(&proj, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj);

	if (pkg_set_source(pkg, STRVN(source, strlen(source)))) {
		return 1;
	}

	proj_gen(&proj, gen);

	proj_free(&proj);

	mem_print(PRINT_DST_STD());

	return 0;
}
