#include "proj_gen.h"

static int create_bin(fs_t *fs, strv_t dir)
{
	path_t path = {0};
	path_init(&path, dir);

	path_child(&path, STRV("bin"));
	if (!fs_isdir(fs, STRVS(path))) {
		fs_mkdir(fs, STRVS(path));
	}

	path_child(&path, STRV(".gitignore"));
	if (!fs_isfile(fs, STRVS(path))) {
		void *f;
		fs_open(fs, STRVS(path), "w", &f);
		fs_write(fs, f, STRV("*"));
		fs_close(fs, f);
	}

	return 0;
}

int proj_gen(const proj_t *proj, const gen_driver_t *drv)
{
	create_bin(drv->fs, STRVS(proj->dir));
	return drv->gen(drv, proj);
}
