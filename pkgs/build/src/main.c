#include "args.h"
#include "print.h"

#include <stdio.h>

typedef enum gen_e {
	GEN_MAKE,
} gen_t;

int main(int argc, const char **argv)
{
	const char *source = ".";
	const char *build  = "./bin";
	gen_t gen	   = GEN_MAKE;

	const opt_enum_val_t gens[] = {
		[GEN_MAKE]  = {"M", "Make"},
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

	return 0;
}
