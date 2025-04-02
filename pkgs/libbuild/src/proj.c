#include "proj.h"

#include "file.h"
#include "log.h"
#include "pkg_loader.h"

proj_t *proj_init(proj_t *proj, uint pkgs_cap, alloc_t alloc)
{
	if (proj == NULL) {
		return NULL;
	}

	if (pkgs_init(&proj->pkgs, pkgs_cap, alloc) == NULL) {
		return NULL;
	}

	return proj;
}

void proj_free(proj_t *proj)
{
	if (proj == NULL) {
		return;
	}

	pkgs_free(&proj->pkgs);
}

static int on_pkg(path_t *path, const char *folder, void *priv)
{
	(void)folder;
	pkgs_t *pkgs = priv;

	strv_t dir = STRVN(path->data, path->len);

	strv_t name;
	pathv_get_dir(dir, &name);

	uint id;
	if (pkgs_add_pkg(pkgs, name, &id) == NULL) {
		return 1; // LCOV_EXCL_LINE
	}

	return pkg_load(id, dir, pkgs, pkgs->alloc);
}

int proj_set_dir(proj_t *proj, strv_t dir)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;

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
		strv_t name;
		pathv_get_dir(dir, &name);

		uint id;
		if (proj_set_pkg(proj, name, &id) == NULL) {
			ret = 1;
		} else {
			ret |= pkg_load(id, dir, &proj->pkgs, proj->pkgs.alloc);
		}
	}

	if (is_pkgs) {
		ret |= files_foreach(&tmp, on_pkg, NULL, &proj->pkgs);
	}

	return ret;
}

pkg_t *proj_set_pkg(proj_t *proj, strv_t name, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	proj->is_pkg = 1;

	return pkgs_add_pkg(&proj->pkgs, name, id);
}

pkg_t *proj_add_pkg(proj_t *proj, strv_t name, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	return pkgs_add_pkg(&proj->pkgs, name, id);
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

	dst.off += pkgs_print(&proj->pkgs, dst);

	return dst.off - off;
}
