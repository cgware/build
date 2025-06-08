#include "proj_gen.h"

static int create_dir(fs_t *fs, strv_t proj_dir, strv_t dir)
{
	path_t path = {0};
	path_init(&path, proj_dir);

	path_push(&path, dir);
	if (!fs_isdir(fs, STRVS(path))) {
		fs_mkdir(fs, STRVS(path));
	}

	path_push(&path, STRV(".gitignore"));
	if (!fs_isfile(fs, STRVS(path))) {
		void *f;
		fs_open(fs, STRVS(path), "w", &f);
		fs_write(fs, f, STRV("*"));
		fs_close(fs, f);
	}

	return 0;
}

int proj_gen(const proj_t *proj, const gen_driver_t *drv, strv_t proj_dir, strv_t build_dir)
{
	create_dir(drv->fs, proj_dir, STRV("bin"));
	create_dir(drv->fs, proj_dir, STRV("tmp"));

	path_t tmp = {0};
	path_init(&tmp, STRVS(proj_dir));
	path_push(&tmp, STRV("tmp"));
	path_push(&tmp, STRV("build"));

	if (!fs_isdir(drv->fs, STRVS(tmp))) {
		fs_mkdir(drv->fs, STRVS(tmp));
	}

	return drv->gen(drv, proj, proj_dir, build_dir);
}
