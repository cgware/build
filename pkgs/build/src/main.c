#include "args.h"
#include "config_fs.h"
#include "log.h"
#include "mem.h"
#include "path.h"
#include "platform.h"
#include "proj_cfg.h"
#include "proj_gen.h"

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

static int build(proc_t *proc, strv_t proj_dir, gen_driver_t *gen_driver, strv_t build_rel, str_t *buf)
{
	int ret = 0;

	fs_t fs = {0};
	fs_init(&fs, 0, 0, ALLOC_STD);

	if (fs_getcwd(&fs, buf)) {
		return 1;
	}

	if (buf->data[buf->len] != '/' && buf->data[buf->len] != '\\') {
		str_cat(buf, STRV(SEP));
	}

	path_t proj_abs = {0};
	path_init(&proj_abs, STRVS(*buf));
	path_merge(&proj_abs, proj_dir);

	path_t proj_rel = {0};
	path_init(&proj_rel, proj_dir);
	path_push(&proj_rel, STRV(""));

	strv_t l, name;
	pathv_rsplit(STRVS(proj_abs), &l, &name);
	if (name.len == 0) {
		pathv_rsplit(l, NULL, &name);
	}

	config_t config = {0};
	config_init(&config, 4, 8, 16, ALLOC_STD);
	config_fs(&config, &fs, proc, STRVS(proj_rel), STRV_NULL, name, buf, ALLOC_STD, DST_STD());
	config_print(&config, DST_STD());
	proj_t proj = {0};
	proj_init(&proj, 8, 16, ALLOC_STD);
	proj_set_str(&proj, proj.name, name);
	proj_cfg(&proj, &config);

	config_free(&config);

	proj_print(&proj, DST_STD());

	gen_driver->fs = &fs;
	if (proj_gen(&proj, gen_driver, STRVS(proj_rel), build_rel)) {
		ret = 1;
	}

	proj_free(&proj);
	fs_free(&fs);

	return ret;
}

static int compile(proc_t *proc, gen_driver_t *gen_driver, strv_t build_rel, strv_t genbuild_rel, strv_t gen_gen, strv_t arch, strv_t conf,
		   strv_t target, int open, int compile, str_t *buf)
{
	if (strv_eq(gen_driver->param, STRV("M"))) {
		buf->len = 0;
		str_cat(buf, STRV("make -C \""));
		str_cat(buf, build_rel);
		str_cat(buf, STRV("\""));
		if (target.len > 0) {
			str_cat(buf, STRV(" "));
			str_cat(buf, target);
		}
		str_cat(buf, STRV(" ARCH=\""));
		str_cat(buf, arch);
		str_cat(buf, STRV("\" CONFIGS=\""));
		str_cat(buf, conf);
		str_cat(buf, open ? STRV("\" OPEN=1") : STRV("\" OPEN=0"));
		log_info("build", "main", NULL, "building project");
		int ret = proc_cmd(proc, STRVS(*buf));
		if (ret) {
			return ret;
		}
	} else if (strv_eq(gen_driver->param, STRV("C"))) {
		if (compile == 0) {
			buf->len = 0;
			str_cat(buf, STRV("cmake -S \""));
			str_cat(buf, build_rel);
			str_cat(buf, STRV("\" -B \""));
			str_cat(buf, genbuild_rel);
			str_cat(buf, STRV("\" -G \""));
			str_cat(buf, gen_gen);
			str_cat(buf, STRV("\" -DARCH=\""));
			str_cat(buf, arch);
			str_cat(buf, STRV("\" -DCONFIGS=\""));
			str_cat(buf, conf);
			str_cat(buf, open ? STRV("\" -DOPEN=1") : STRV("\" -DOPEN=0"));
			log_info("build", "main", NULL, "creating generator");
			int ret = proc_cmd(proc, STRVS(*buf));
			if (ret) {
				return ret;
			}
		}

		buf->len = 0;
		str_cat(buf, STRV("cmake --build \""));
		str_cat(buf, genbuild_rel);
		str_cat(buf, STRV("\""));
		if (target.len > 0) {
			str_cat(buf, STRV(" --target "));
			str_cat(buf, target);
		}
		log_info("build", "main", NULL, "building project");
		int ret = proc_cmd(proc, STRVS(*buf));
		if (ret) {
			return ret;
		}
	}

	return 0;
}

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
	strv_t target	 = STRV("");
	strv_t arch	 = STRV("x64");
	strv_t conf	 = STRV("Debug");
	strv_t gen_build = STRV("build");
#ifdef C_WIN
	strv_t gen_gen = STRV("Visual Studio 17 2022");
#else
	strv_t gen_gen = STRV("Unix Makefiles");
#endif
	int open     = 1;
	int comp     = 0;
	int generate = 0;

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
#ifdef C_WIN
			strv_t param = STRV("C");
#else
			strv_t param = STRV("M");
#endif
			if (strv_eq(drv->param, param)) {
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
		OPT('t', "target", OPT_STR, "<target>", "Specify target", &target, {0}, OPT_OPT),
		OPT('a', "arch", OPT_STR, "<arch>", "Specify architecture", &arch, {0}, OPT_OPT),
		OPT('c', "config", OPT_STR, "<config>", "Specify configuration", &conf, {0}, OPT_OPT),
		OPT('B', "gen-build", OPT_STR, "<path>", "Specify generator build directory", &gen_build, {0}, OPT_OPT),
		OPT('G', "gen-gen", OPT_STR, "<generator-generator>", "Specify generator generator", &gen_gen, {0}, OPT_OPT),
		OPT('O', "open", OPT_BOOL, "<0/1>", "Open", &open, {0}, OPT_OPT),
		OPT('S', "generate", OPT_NONE, NULL, "Only generate", &generate, {0}, OPT_OPT),
		OPT('C', "compile", OPT_NONE, NULL, "Only compile", &comp, {0}, OPT_OPT),
	};

	if (args_parse(argc, argv, opts, sizeof(opts), DST_STD())) {
		return 1;
	}

	path_t build_rel = {0};
	path_init(&build_rel, STRVS(proj_dir));
	path_merge(&build_rel, build_dir);
	path_push(&build_rel, STRV(""));

	path_t genbuild_rel = {0};
	path_init(&genbuild_rel, STRVS(proj_dir));
	path_merge(&genbuild_rel, gen_build);
	path_push(&genbuild_rel, STRV(""));

	gen_driver_t *gen_driver = (gen_driver_t *)gens[gen].priv;

	str_t buf = strz(1024);

	proc_t proc = {0};
	proc_init(&proc, 0, 0);

	if (comp == 0 && build(&proc, STRVS(proj_dir), gen_driver, STRVS(build_rel), &buf)) {
		return 1;
	}

	if (generate == 0 && compile(&proc,
				     gen_driver,
				     STRVS(build_rel),
				     STRVS(genbuild_rel),
				     STRVS(gen_gen),
				     STRVS(arch),
				     STRVS(conf),
				     STRVS(target),
				     open,
				     comp,
				     &buf)) {
		return 1;
	}

	str_free(&buf);
	proc_free(&proc);

	mem_free(gens, gen_drivers_cnt * sizeof(opt_enum_val_t));

	mem_print(DST_STD());
	return 0;
}
