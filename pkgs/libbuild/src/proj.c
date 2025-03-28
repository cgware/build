#include "proj.h"

#include "log.h"

proj_t *proj_init(proj_t *proj, uint pkgs_cap, alloc_t alloc)
{
	if (proj == NULL) {
		return NULL;
	}

	if (arr_init(&proj->pkgs, pkgs_cap, sizeof(pkg_t), alloc) == NULL) {
		log_error("build", "proj", NULL, "failed to initialize packages array");
		return NULL;
	}

	return proj;
}

void proj_free(proj_t *proj)
{
	if (proj == NULL) {
		return;
	}

	if (proj->is_pkg) {
		pkg_free(&proj->pkg);
	} else {
		pkg_t *pkg;
		arr_foreach(&proj->pkgs, pkg)
		{
			pkg_free(pkg);
		}
	}

	arr_free(&proj->pkgs);
}

int proj_set_dir(proj_t *proj, strv_t dir)
{
	if (proj == NULL) {
		return 1;
	}

	path_init(&proj->dir, dir);
	path_init(&proj->outdir, STRV("bin" SEP "${ARCH}-${CONFIG}/"));

	path_t tmp = {0};
	path_init(&tmp, dir);
	path_child(&tmp, STRV("src"));
	int is_src = path_is_dir(&tmp);

	path_init(&tmp, dir);
	path_child(&tmp, STRV("pkgs"));
	int is_pkgs = path_is_dir(&tmp);

	if (!is_src && !is_pkgs) {
		log_error("build", "proj", NULL, "No 'src' or 'pkgs' folder found: %.*s\n", dir.len, dir.data);
		return 1;
	} else if (is_src && is_pkgs) {
		log_error("build", "proj", NULL, "Only one of 'src' or 'pkgs' folder expected: %.*s\n", dir.len, dir.data);
		return 1;
	}

	if (is_src) {
		pkg_t *pkg = proj_set_pkg(proj);

		if (pkg_set_dir(pkg, dir)) {
			//return 1;
		}

		pathv_get_dir(STRVN(proj->dir.data, proj->dir.len), &pkg->name);
	}

	if (is_pkgs) {
		// TODO
	}

	return 0;
}

pkg_t *proj_set_pkg(proj_t *proj)
{
	if (proj == NULL) {
		return NULL;
	}

	proj->is_pkg = 1;

	return pkg_init(&proj->pkg);
}

pkg_t *proj_add_pkg(proj_t *proj)
{
	if (proj == NULL) {
		return NULL;
	}

	pkg_t *pkg = arr_add(&proj->pkgs);
	if (pkg == NULL) {
		return NULL;
	}

	return pkg_init(pkg);
}

int proj_print(const proj_t *proj, print_dst_t dst)
{
	if (proj == NULL) {
		return 0;
	}

	int off = dst.off;

	dst.off += c_dprintf(dst,
			     "[project]\n"
			     "DIR: %.*s\n"
			     "OUTDIR: %.*s\n"
			     "\n",
			     proj->dir.len,
			     proj->dir.data,
			     proj->outdir.len,
			     proj->outdir.data);

	if (proj->is_pkg) {
		dst.off += pkg_print(&proj->pkg, dst);
	} else {
		const pkg_t *pkg;
		arr_foreach(&proj->pkgs, pkg)
		{
			dst.off += pkg_print(pkg, dst);
		}
	}

	return dst.off - off;
}
