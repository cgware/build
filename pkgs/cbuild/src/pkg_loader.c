#include "pkg_loader.h"

#include "file/cfg_parse.h"

int pkg_load(uint id, fs_t *fs, strv_t dir, pkgs_t *pkgs, targets_t *targets, alloc_t alloc)
{
	pkg_t *pkg = pkgs_get_pkg(pkgs, id);
	if (pkg == NULL) {
		return 1;
	}

	int ret = 0;

	path_init(&pkg->dir, dir);

	path_init(&pkg->src, dir);
	path_child(&pkg->src, STRV("src"));

	if (!fs_isdir(fs, STRVS(pkg->src))) {
		pkg->src.len = 0;
	}

	path_init(&pkg->inc, dir);
	path_child(&pkg->inc, STRV("include"));

	if (!fs_isdir(fs, STRVS(pkg->inc))) {
		pkg->inc.len = 0;
	}

	path_t conf_path = {0};
	path_init(&conf_path, dir);
	path_child(&conf_path, STRV("pkg.cfg"));

	if (fs_isfile(fs, STRVS(conf_path))) {
		cfg_prs_t prs = {0};
		cfg_prs_init(&prs, alloc);

		str_t buf = strz(1024);

		fs_read(fs, STRVS(conf_path), 0, &buf);

		cfg_t cfg = {0};
		cfg_init(&cfg, 4, 4, alloc);

		cfg_var_t root = cfg_prs_parse(&prs, STRVS(buf), &cfg, alloc, PRINT_DST_STD());
		cfg_prs_free(&prs);

		ret |= pkg_set_cfg(id, &cfg, root, pkgs, targets);

		cfg_free(&cfg);
		str_free(&buf);
	} else if (pkg->src.len > 0) {
		if (pkg_add_target(pkg, targets, pkgs_get_pkg_name(pkgs, id), NULL) == NULL) {
			ret = 1;
		}
	}

	target_t *target = targets_get(targets, pkg->targets);
	if (target != NULL) {
		if (target->type == TARGET_TYPE_UNKNOWN) {
			if (pkg->src.len > 0) {
				target->type = TARGET_TYPE_EXE;
			}
			if (pkg->inc.len > 0) {
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

	uint target_id	 = ARR_END;
	target_t *target = pkg_add_target(pkg, targets, pkgs_get_pkg_name(pkgs, id), &target_id);
	if (target == NULL) {
		ret = 1;
	}

	cfg_var_t deps;
	if (cfg_get_var(cfg, root, STRV("deps"), &deps) == 0) {
		cfg_var_t dep = CFG_VAR_END;
		while (!cfg_get_arr(cfg, deps, &dep)) {
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
