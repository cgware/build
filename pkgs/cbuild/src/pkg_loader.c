#include "pkg_loader.h"

#include "file.h"
#include "file/cfg_parse.h"

int pkg_load(uint id, strv_t dir, pkgs_t *pkgs, alloc_t alloc)
{
	pkg_t *pkg = pkgs_get_pkg(pkgs, id);
	if (pkg == NULL) {
		return 1;
	}

	int ret = 0;

	path_init(&pkg->dir, dir);

	path_init(&pkg->src, dir);
	path_child(&pkg->src, STRV("src"));

	if (!path_is_dir(&pkg->src)) {
		pkg->src.len = 0;
	} else {
		pkg->type = PKG_TYPE_EXE;
	}

	path_init(&pkg->inc, dir);
	path_child(&pkg->inc, STRV("include"));

	if (!path_is_dir(&pkg->inc)) {
		pkg->inc.len = 0;
	} else {
		pkg->type = PKG_TYPE_LIB;
	}

	path_t conf_path = {0};
	path_init(&conf_path, dir);
	path_child(&conf_path, STRV("pkg.cfg"));

	if (file_exists(conf_path.data)) {
		cfg_prs_t prs = {0};
		cfg_prs_init(&prs, alloc);

		char buf[1024] = {0};

		size_t len = file_read_t(conf_path.data, buf, sizeof(buf));

		cfg_t cfg = {0};
		cfg_init(&cfg, 4, 4, alloc);

		cfg_var_t root = cfg_prs_parse(&prs, STRVN(buf, len), &cfg, alloc, PRINT_DST_STD());
		cfg_prs_free(&prs);

		ret |= pkg_set_cfg(id, &cfg, root, pkgs);

		cfg_free(&cfg);
	}

	return ret;
}

int pkg_set_cfg(uint pkg, const cfg_t *cfg, cfg_var_t root, pkgs_t *pkgs)
{
	if (cfg == NULL || pkgs == NULL) {
		return 1;
	}

	int ret = 0;

	cfg_var_t deps;
	if (cfg_get_var(cfg, root, STRV("deps"), &deps) == 0) {
		cfg_var_t dep = CFG_VAR_END;
		while (!cfg_get_arr(cfg, deps, &dep)) {
			strv_t dep_str;
			if (cfg_get_lit(cfg, dep, &dep_str)) {
				ret = 1;
				continue;
			}

			ret |= pkgs_add_dep(pkgs, pkg, dep_str);
		}
	}

	return ret;
}
