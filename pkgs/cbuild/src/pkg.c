#include "pkg.h"

#include "file/cfg_prs.h"
#include "log.h"

pkg_t *pkg_init(pkg_t *pkg, uint id)
{
	if (pkg == NULL) {
		return NULL;
	}

	pkg->id		 = id;
	pkg->has_targets = 0;
	pkg->loaded	 = 0;

	return pkg;
}

void pkg_free(pkg_t *pkg)
{
	if (pkg == NULL) {
		return;
	}
}

target_t *pkg_add_target(pkg_t *pkg, targets_t *targets, strv_t name, list_node_t *id)
{
	if (pkg == NULL || targets == NULL) {
		return NULL;
	}

	list_node_t tmp;
	target_t *target = targets_target(targets, name, &tmp);
	if (target == NULL) {
		log_error("cbuild", "pkg", NULL, "failed to create target");
		return NULL;
	}

	if (pkg->has_targets) {
		if (targets_app(targets, pkg->targets, tmp)) {
			log_error("cbuild", "pkg", NULL, "failed to add target");
			return NULL;
		}
	} else {
		pkg->targets	 = tmp;
		pkg->has_targets = 1;
	}

	if (id) {
		*id = tmp;
	}

	target->pkg = pkg->id;
	return target;
}

size_t pkg_print(const pkg_t *pkg, const targets_t *targets, dst_t dst)
{
	size_t off = dst.off;

	if (pkg->has_targets) {
		dst.off += targets_print(targets, pkg->targets, dst);
	}

	return dst.off - off;
}
