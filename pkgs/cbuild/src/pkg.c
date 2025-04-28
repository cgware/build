#include "pkg.h"

#include "file/cfg_parse.h"
#include "log.h"

pkg_t *pkg_init(pkg_t *pkg, uint id)
{
	if (pkg == NULL) {
		return NULL;
	}

	pkg->id	       = id;
	pkg->dir       = (path_t){0};
	pkg->src       = (path_t){0};
	pkg->inc       = (path_t){0};
	pkg->targets   = LIST_END;

	return pkg;
}

void pkg_free(pkg_t *pkg)
{
	if (pkg == NULL) {
		return;
	}
}

target_t *pkg_add_target(pkg_t *pkg, targets_t *targets, strv_t name, lnode_t *id)
{
	if (pkg == NULL || targets == NULL) {
		return NULL;
	}

	target_t *target = targets_add(targets, &pkg->targets, name, id);
	if (target == NULL) {
		return NULL;
	}

	target->pkg = pkg->id;
	return target;
}

int pkg_print(const pkg_t *pkg, const targets_t *targets, print_dst_t dst)
{
	int off = dst.off;

	dst.off += c_dprintf(dst,
			     "[package]\n"
			     "ID: %d\n"
			     "DIR: %.*s\n"
			     "SRC: %.*s\n"
			     "INC: %.*s\n",
			     pkg->id,
			     pkg->dir.len,
			     pkg->dir.data,
			     pkg->src.len,
			     pkg->src.data,
			     pkg->inc.len,
			     pkg->inc.data);

	dst.off += targets_print(targets, pkg->targets, dst);

	return dst.off - off;
}
