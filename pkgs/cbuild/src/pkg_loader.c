#include "pkg_loader.h"

#include "file/cfg_prs.h"
#include "log.h"

int pkg_load(uint id, fs_t *fs, strv_t dir, pkgs_t *pkgs, targets_t *targets, alloc_t alloc)
{
	pkg_t *pkg = pkgs_get_pkg(pkgs, id);
	if (pkg == NULL) {
		return 1;
	}

	int ret = 0;

	pkgs_set_str(pkgs, pkg->dir, dir);

	path_t path = {0};
	path_init(&path, dir);
	path_child(&path, STRV("src"));

	if (fs_isdir(fs, STRVS(path))) {
		pkgs_set_str(pkgs, pkg->src, STRVS(path));
	}

	path_init(&path, dir);
	path_child(&path, STRV("include"));

	if (fs_isdir(fs, STRVS(path))) {
		pkgs_set_str(pkgs, pkg->inc, STRVS(path));
	}

	path_init(&path, dir);
	path_child(&path, STRV("pkg.cfg"));

	if (fs_isfile(fs, STRVS(path))) {
		cfg_prs_t prs = {0};
		cfg_prs_init(&prs, alloc);

		str_t buf = strz(1024);

		fs_read(fs, STRVS(path), 0, &buf);

		cfg_t cfg = {0};
		cfg_init(&cfg, 4, 4, alloc);

		cfg_var_t root;
		cfg_prs_parse(&prs, STRVS(buf), &cfg, alloc, &root, DST_STD());
		cfg_prs_free(&prs);

		ret |= pkg_set_cfg(id, &cfg, root, pkgs, targets);

		cfg_free(&cfg);
		str_free(&buf);
	} else {
		strv_t src = strvbuf_get(&pkgs->strs, pkg->src);
		if (src.len > 0) {
			if (pkg_add_target(pkg, targets, strvbuf_get(&pkgs->strs, pkg->name), NULL) == NULL) {
				log_error("cbuild", "pkg_loader", NULL, "failed to add target");
				ret = 1;
			}
		}
	}

	if (pkg->has_targets) {
		target_t *target = targets_get(targets, pkg->targets);
		if (target->type == TARGET_TYPE_UNKNOWN) {
			strv_t src = strvbuf_get(&pkgs->strs, pkg->src);
			if (src.len > 0) {
				target->type = TARGET_TYPE_EXE;
			}
			strv_t inc = strvbuf_get(&pkgs->strs, pkg->inc);
			if (inc.len > 0) {
				target->type = TARGET_TYPE_LIB;
			}
		}
	}

	return ret;
}

int pkg_set_cfg(uint id, const cfg_t *cfg, cfg_var_t root, pkgs_t *pkgs, targets_t *targets)
{
	if (cfg == NULL || pkgs == NULL) {
		return 1;
	}

	pkg_t *pkg = pkgs_get_pkg(pkgs, id);
	if (pkg == NULL) {
		return 1;
	}

	int ret = 0;

	uint target_id;
	target_t *target = pkg_add_target(pkg, targets, strvbuf_get(&pkgs->strs, pkg->name), &target_id);
	if (target == NULL) {
		ret = 1;
	}

	cfg_var_t deps;
	if (cfg_get_var(cfg, root, STRV("deps"), &deps) == 0) {
		cfg_var_t dep;
		void *data;
		cfg_foreach(cfg, deps, data, &dep)
		{
			strv_t dep_str;
			if (cfg_get_lit(cfg, dep, &dep_str)) {
				ret = 1;
				continue;
			}

			if (targets_add_dep(targets, target_id, dep_str) == NULL) {
				ret = 1;
				continue;
			}
		}
	}

	return ret;
}
