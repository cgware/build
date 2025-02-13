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

	pkg_t *pkg;
	arr_foreach(&proj->pkgs, pkg)
	{
		pkg_free(pkg);
	}

	arr_free(&proj->pkgs);
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
			     "BUILDDIR: %.*s\n"
			     "OUTDIR: %.*s\n"
			     "\n",
			     proj->builddir.len,
			     proj->builddir.data,
			     proj->outdir.len,
			     proj->outdir.data);

	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, pkg)
	{
		dst.off += pkg_print(pkg, dst);
	}

	return dst.off - off;
}
