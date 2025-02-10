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

int proj_gen(const proj_t *proj, gen_t gen)
{
	(void)proj;
	(void)gen;
	return 0;
}
