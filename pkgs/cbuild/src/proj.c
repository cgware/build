#include "proj.h"

#include "fs.h"
#include "log.h"
#include "pkg_loader.h"

proj_t *proj_init(proj_t *proj, uint pkgs_cap, alloc_t alloc)
{
	if (proj == NULL) {
		return NULL;
	}

	if (pkgs_init(&proj->pkgs, pkgs_cap, alloc) == NULL || strvbuf_init(&proj->strs, __EXT_STR_CNT, 8 + 36, alloc) == NULL) {
		return NULL;
	}

	return proj;
}

void proj_free(proj_t *proj)
{
	if (proj == NULL) {
		return;
	}

	strvbuf_free(&proj->strs);
	pkgs_free(&proj->pkgs);
}

pkg_t *proj_set_pkg(proj_t *proj, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	return pkgs_add(&proj->pkgs, id);
}

pkg_t *proj_add_pkg(proj_t *proj, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	return pkgs_add(&proj->pkgs, id);
}

size_t proj_print(const proj_t *proj, dst_t dst)
{
	if (proj == NULL) {
		return 0;
	}

	size_t off = dst.off;

	dst.off += dputf(dst,
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
