#include "proj.h"

proj_t *proj_init(proj_t *proj, uint pkgs_cap, uint targets_cap, alloc_t alloc)
{
	if (proj == NULL) {
		return NULL;
	}

	if (strvbuf_init(&proj->strs, 4 * pkgs_cap + 1 * targets_cap, 16, alloc) == NULL ||
	    arr_init(&proj->pkgs, pkgs_cap, sizeof(pkg_t), alloc) == NULL ||
	    list_init(&proj->targets, targets_cap, sizeof(target_t), alloc) == NULL) {
		return NULL;
	}

	return proj;
}

void proj_free(proj_t *proj)
{
	if (proj == NULL) {
		return;
	}

	list_free(&proj->targets);
	arr_free(&proj->pkgs);
	strvbuf_free(&proj->strs);
}

int proj_config(proj_t *proj, const config_t *config)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;
	uint i;

	i = 0;
	const config_dir_t *dir;
	arr_foreach(&config->dirs, i, dir)
	{
		strv_t name = strvbuf_get(&config->strs, dir->name);
		strv_t path = strvbuf_get(&config->strs, dir->path);
		strv_t src  = strvbuf_get(&config->strs, dir->src);
		strv_t inc  = strvbuf_get(&config->strs, dir->inc);

		pkg_t *pkg	 = NULL;
		target_t *target = NULL;

		if (src.len > 0 || inc.len > 0) {
			pkg = arr_add(&proj->pkgs, NULL);

			pkg->has_targets = 0;

			strvbuf_add(&proj->strs, name, &pkg->name);
			strvbuf_add(&proj->strs, path, &pkg->path);
			strvbuf_add(&proj->strs, src, &pkg->src);
			strvbuf_add(&proj->strs, inc, &pkg->inc);

			list_node_t target_id;
			target = list_node(&proj->targets, &target_id);
			strvbuf_add(&proj->strs, name, &target->name);

			if (pkg->has_targets) {
				list_app(&proj->targets, pkg->targets, target_id);
			} else {
				pkg->targets	 = target_id;
				pkg->has_targets = 1;
			}
		}

		if (src.len > 0) {
			target->type = TARGET_TYPE_EXE;
		}

		if (inc.len > 0) {
			target->type = TARGET_TYPE_LIB;
		}

		if (dir->has_pkgs) {
			const config_pkg_t *cfg_pkg;
			list_node_t pkgs = dir->pkgs;
			list_foreach(&config->pkgs, pkgs, cfg_pkg)
			{
				if (pkg == NULL) {
					pkg = arr_add(&proj->pkgs, NULL);

					pkg->has_targets = 0;

					strvbuf_add(&proj->strs, name, &pkg->name);
					strvbuf_add(&proj->strs, path, &pkg->path);
					strvbuf_add(&proj->strs, src, &pkg->src);
					strvbuf_add(&proj->strs, inc, &pkg->inc);
				}

				/*const size_t *cfg_dep;
				list_node_t deps = pkg->deps;
				list_foreach(&config->pkgs, pkgs, cpkg)
				{

				}*/

				pkg = NULL;
			}
		}
	}

	return ret;
}

static const char *target_type_str[] = {
	[TARGET_TYPE_UNKNOWN] = "UNKNOWN",
	[TARGET_TYPE_EXE]     = "EXE",
	[TARGET_TYPE_LIB]     = "LIB",
	[TARGET_TYPE_EXT]     = "EXT",
};

size_t proj_print(const proj_t *proj, dst_t dst)
{
	if (proj == NULL) {
		return 0;
	}
	(void)target_type_str;
	int off = dst.off;

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		dst.off += dputf(dst, "[pkg]\n");

		strv_t name = strvbuf_get(&proj->strs, pkg->name);
		strv_t path = strvbuf_get(&proj->strs, pkg->path);
		strv_t src  = strvbuf_get(&proj->strs, pkg->src);
		strv_t inc  = strvbuf_get(&proj->strs, pkg->inc);

		dst.off += dputf(dst,
				 "NAME: %.*s\n"
				 "PATH: %.*s\n"
				 "SRC: %.*s\n"
				 "INC: %.*s\n\n",
				 name.len,
				 name.data,
				 path.len,
				 path.data,
				 src.len,
				 src.data,
				 inc.len,
				 inc.data);
	}

	return off - dst.off;
}
