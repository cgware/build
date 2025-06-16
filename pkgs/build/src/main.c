#include "args.h"
#include "log.h"
#include "mem.h"
#include "path.h"
#include "proj_fs.h"
#include "proj_gen.h"

int main(int argc, const char **argv)
{
	mem_stats_t mem_stats = {0};
	mem_stats_set(&mem_stats);

	c_print_init();

	log_t log = {0};
	log_set(&log);
	log_add_callback(log_std_cb, DST_STD(), LOG_INFO, 1, 1);

	strv_t proj_dir	 = STRV(".");
	strv_t build_dir = STRV("tmp/build");

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
			if (strv_eq(drv->param, STRV("M"))) {
				gen = gen_drivers_cnt;
			}
			gen_drivers_cnt++;
		}
	}

	const opt_enum_t gens_desc = {
		.name	   = "Generators",
		.vals	   = gens,
		.vals_size = gen_drivers_cnt * sizeof(opt_enum_val_t),
	};

	opt_t opts[] = {
		OPT('p', "project", OPT_STR, "<path>", "Specify project directory", &proj_dir, {0}, OPT_OPT),
		OPT('b', "build", OPT_STR, "<path>", "Specify build directory", &build_dir, {0}, OPT_OPT),
		OPT('g', "generator", OPT_ENUM, "<generator>", "Specify build system generator", &gen, gens_desc, OPT_OPT),
	};

	if (args_parse(argc, argv, opts, sizeof(opts), DST_STD())) {
		return 1;
	}

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	proc_t proc = {0};
	proc_init(&proc, 0, 0);

	proj_t proj = {0};
	proj_init(&proj, 4, 4, ALLOC_STD);

	str_t cwd = strz(64);

	if (fs_getcwd(&fs, &cwd)) {
		return 1;
	}

	if (cwd.data[cwd.len] != '/' && cwd.data[cwd.len] != '\\') {
		str_cat(&cwd, STRV(SEP));
	}

	path_t proj_abs = {0};
	path_init(&proj_abs, STRVS(cwd));
	path_merge(&proj_abs, proj_dir);

	path_t proj_rel = {0};
	path_init(&proj_rel, proj_dir);
	path_push(&proj_rel, STRV(""));

	path_t build_rel = {0};
	path_init(&build_rel, STRVS(proj_dir));
	path_merge(&build_rel, build_dir);
	path_push(&build_rel, STRV(""));

	strv_t l, name;
	pathv_rsplit(STRVS(proj_abs), &l, &name);
	if (name.len == 0) {
		pathv_rsplit(l, NULL, &name);
	}
	str_t buf = strz(1024);
	if (proj_fs(&proj, &fs, &proc, STRVS(proj_rel), STRV_NULL, name, &buf, ALLOC_STD)) {
		return 1;
	}

	proj_print(&proj, DST_STD());

	gen_driver_t gen_driver = *(gen_driver_t *)gens[gen].priv;

	gen_driver.fs = &fs;
	if (proj_gen(&proj, &gen_driver, STRVS(proj_rel), STRVS(build_rel))) {
		return 1;
	}

	proj_free(&proj);
	proc_free(&proc);
	fs_free(&fs);
	str_free(&cwd);

	mem_free(gens, gen_drivers_cnt * sizeof(opt_enum_val_t));

	mem_print(DST_STD());
	return 0;
}
