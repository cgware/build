#include "pkg_loader.h"

#include "file/cfg_prs.h"
#include "log.h"

pkg_t *pkg_load(fs_t *fs, strv_t proj_dir, strv_t def_name, strv_t dir, pkgs_t *pkgs, alloc_t alloc, str_t *buf)
{
	int ret = 0;

	path_t path = {0};
	path_init(&path, proj_dir);
	path_child(&path, dir);

	size_t path_len = path.len;

	cfg_t cfg = {0};
	cfg_var_t root;
	cfg_init(&cfg, 4, 4, alloc);

	path_child(&path, STRV("pkg.cfg"));
	if (fs_isfile(fs, STRVS(path))) {
		if (buf) {
			cfg_prs_t prs = {0};
			cfg_prs_init(&prs, alloc);
			fs_read(fs, STRVS(path), 0, buf);
			cfg_prs_parse(&prs, STRVS(*buf), &cfg, alloc, &root, DST_STD());
			cfg_prs_free(&prs);
		}
	} else {
		cfg_root(&cfg, &root);
	}
	path.len = path_len;

	strv_t name = {0};
	cfg_var_t var;
	if (cfg_has_var(&cfg, root, STRV("name"), &var)) {
		cfg_get_lit(&cfg, var, &name);
	} else if (dir.len > 0) {
		strv_t l;
		pathv_rsplit(dir, &l, &name);
		if (name.len == 0) {
			pathv_rsplit(l, NULL, &name);
		}
	} else {
		name = def_name;
	}

	pkg_t *pkg = pkgs_find(pkgs, name, NULL);
	if (pkg == NULL) {
		pkg = pkgs_add(pkgs, NULL);
	} else if (pkg->loaded) {
		log_error("cbuild", "pkg_loader", NULL, "package already exists: '%.*s'", name.len, name.data);
		pkg = NULL;
	}

	if (pkg) {
		pkgs_set_str(pkgs, pkg->strs[PKG_NAME], name);
		pkgs_set_str(pkgs, pkg->strs[PKG_DIR], dir);
		ret |= pkg_set_cfg(pkg, &cfg, root, pkgs, proj_dir, fs);
		pkg->loaded = 1;
	}

	cfg_free(&cfg);

	return pkg;
}

int pkg_set_cfg(pkg_t *pkg, const cfg_t *cfg, cfg_var_t root, pkgs_t *pkgs, strv_t proj_dir, fs_t *fs)
{
	if (pkg == NULL || cfg == NULL || pkgs == NULL) {
		return 1;
	}

	int ret = 0;

	path_t path = {0};
	path_init(&path, proj_dir);
	path_child(&path, strvbuf_get(&pkgs->strs, pkg->strs[PKG_DIR]));
	size_t path_len = path.len;

	cfg_var_t var;

	strv_t src = {0};
	if (cfg_has_var(cfg, root, STRV("src"), &var)) {
		cfg_get_str(cfg, var, &src);
		path_child(&path, src);
		if (!fs_isdir(fs, STRVS(path))) {
			src.len = 0;
			log_error("cbuild", "pkg_loader", NULL, "src does not exist: '%.*s'", src.len, src.data);
			ret = 1;
		}
	} else {
		src = STRV("src");
		path_child(&path, src);
		if (!fs_isdir(fs, STRVS(path))) {
			src.len = 0;
		}
	}
	path.len = path_len;

	if (src.len > 0) {
		pkgs_set_str(pkgs, pkg->strs[PKG_SRC], src);
	}

	strv_t inc = {0};
	if (cfg_has_var(cfg, root, STRV("include"), &var)) {
		cfg_get_str(cfg, var, &inc);
		path_child(&path, inc);
		if (!fs_isdir(fs, STRVS(path))) {
			inc.len = 0;
			log_error("cbuild", "pkg_loader", NULL, "include does not exist: '%.*s'", inc.len, inc.data);
			ret = 1;
		}
	} else {
		inc = STRV("include");
		path_child(&path, inc);
		if (!fs_isdir(fs, STRVS(path))) {
			inc.len = 0;
		}
	}
	path.len = path_len;

	if (inc.len > 0) {
		pkgs_set_str(pkgs, pkg->strs[PKG_INC], inc);
	}

	void *data;
	cfg_var_t cfg_target;
	cfg_foreach(cfg, root, data, &cfg_target)
	{
		strv_t key = cfg_get_key(cfg, cfg_target);
		if (strv_eq(key, STRV("target"))) {
			target_t *target = pkg_add_target(pkg, &pkgs->targets, strvbuf_get(&pkgs->strs, pkg->strs[PKG_NAME]), NULL);
			if (target == NULL) {
				log_error("cbuild", "pkg_loader", NULL, "failed to add target");
				ret = 1;
			} else {
				cfg_var_t cfg_type;
				if (cfg_has_var(cfg, cfg_target, STRV("type"), &cfg_type)) {
					strv_t type;
					cfg_get_lit(cfg, cfg_type, &type);
					if (strv_eq(type, STRV("EXE"))) {
						target->type = TARGET_TYPE_EXE;
					} else if (strv_eq(type, STRV("LIB"))) {
						target->type = TARGET_TYPE_LIB;
					} else {
						log_error("cbuild", "pkg_loader", NULL, "unknown target type: '%.*s'", type.len, type.data);
						ret = 1;
					}
				} else {
					if (src.len > 0) {
						target->type = TARGET_TYPE_EXE;
					}
					if (inc.len > 0) {
						target->type = TARGET_TYPE_LIB;
					}
				}
			}
		}
	}

	if (!pkg->has_targets) {
		target_t *target = pkg_add_target(pkg, &pkgs->targets, strvbuf_get(&pkgs->strs, pkg->strs[PKG_NAME]), NULL);
		if (target == NULL) {
			log_error("cbuild", "pkg_loader", NULL, "failed to add target");
			ret = 1;
		} else {
			if (src.len > 0) {
				target->type = TARGET_TYPE_EXE;
			}
			if (inc.len > 0) {
				target->type = TARGET_TYPE_LIB;
			}
		}
	}

	if (cfg_has_var(cfg, root, STRV("deps"), &var)) {
		cfg_var_t dep;
		void *data;
		cfg_foreach(cfg, var, data, &dep)
		{
			strv_t dep_str;
			if (cfg_get_lit(cfg, dep, &dep_str)) {
				log_error("cbuild", "pkg_loader", NULL, "invalid dependency");
				ret = 1;
				continue;
			}

			const target_t *target;
			list_node_t i = pkg->targets;
			list_foreach(&pkgs->targets.targets, i, target)
			{
				if (targets_add_dep(&pkgs->targets, i, dep_str) == NULL) {
					log_error("cbuild", "pkg_loader", NULL, "failed to dependency");
					ret = 1;
				}
			}
		}
	}

	return ret;
}
