#include "pkg_loader.h"

#include "file/cfg_prs.h"
#include "log.h"

static int create_tmp(fs_t *fs, strv_t dir)
{
	path_t path = {0};
	path_init(&path, dir);

	path_push(&path, STRV("tmp"));
	if (!fs_isdir(fs, STRVS(path))) {
		fs_mkdir(fs, STRVS(path));
	}

	path_push(&path, STRV(".gitignore"));
	if (!fs_isfile(fs, STRVS(path))) {
		void *f;
		fs_open(fs, STRVS(path), "w", &f);
		fs_write(fs, f, STRV("*"));
		fs_close(fs, f);
	}

	return 0;
}

pkg_t *pkg_load(fs_t *fs, strv_t proj_dir, strv_t def_name, strv_t dir, pkgs_t *pkgs, alloc_t alloc, str_t *buf)
{
	int ret = 0;

	path_t path = {0};
	path_init(&path, proj_dir);
	path_push(&path, dir);

	size_t path_len = path.len;

	cfg_t cfg = {0};
	cfg_var_t root;
	cfg_init(&cfg, 4, 4, alloc);

	path_push(&path, STRV("pkg.cfg"));
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

	ret |= pkg_set_cfg(pkg, &cfg, root, pkgs, proj_dir, fs);

	cfg_free(&cfg);

	return pkg;
}

int pkg_set_cfg(const cfg_t *cfg, cfg_var_t root, pkgs_t *pkgs, strv_t proj_dir, strv_t proj_name, strv_t dir, fs_t *fs, proc_t *proc,
		alloc_t alloc, str_t *buf)
{
	if (cfg == NULL || pkgs == NULL) {
		return 1;
	}

	int ret = 0;

	pkg_t *pkg = NULL;

	void *data;
	cfg_var_t tbl;
	cfg_foreach(cfg, root, data, &tbl)
	{
		strv_t key = cfg_get_key(cfg, tbl);
		if (strv_eq(key, STRV("pkg"))) {
			strv_t name = {0};
			cfg_var_t var;
			if (cfg_has_var(&cfg, tbl, STRV("name"), &var)) {
				cfg_get_lit(&cfg, var, &name);
			} else if (dir.len > 0) {
				strv_t l;
				pathv_rsplit(dir, &l, &name);
				if (name.len == 0) {
					pathv_rsplit(l, NULL, &name);
				}
			} else {
				name = proj_name;
			}

			list_node_t pkg_id;
			pkg = pkgs_find(pkgs, name, &pkg_id);
			if (pkg == NULL) {
				pkg = pkgs_add(pkgs, &pkg_id);
			} else if (pkg->loaded) {
				log_error("cbuild", "pkg_loader", NULL, "package already exists: '%.*s'", name.len, name.data);
				pkg = NULL;
			}

			if (pkg) {
				pkgs_set_str(pkgs, pkg->strs[PKG_NAME], name);
				pkgs_set_str(pkgs, pkg->strs[PKG_DIR], dir);

				path_t path = {0};
				path_init(&path, proj_dir);
				path_push(&path, strvbuf_get(&pkgs->strs, pkg->strs[PKG_DIR]));
				size_t path_len = path.len;

				cfg_var_t var;

				strv_t src = {0};
				if (cfg_has_var(cfg, tbl, STRV("src"), &var)) {
					cfg_get_str(cfg, var, &src);
					path_push(&path, src);
					if (!fs_isdir(fs, STRVS(path))) {
						src.len = 0;
						log_error("cbuild", "pkg_loader", NULL, "src does not exist: '%.*s'", src.len, src.data);
						ret = 1;
					}
				} else {
					src = STRV("src");
					path_push(&path, src);
					if (!fs_isdir(fs, STRVS(path))) {
						src.len = 0;
					}
				}
				path.len = path_len;

				if (src.len > 0) {
					pkgs_set_str(pkgs, pkg->strs[PKG_SRC], src);
				}

				strv_t inc = {0};
				if (cfg_has_var(cfg, tbl, STRV("include"), &var)) {
					cfg_get_str(cfg, var, &inc);
					path_push(&path, inc);
					if (!fs_isdir(fs, STRVS(path))) {
						inc.len = 0;
						log_error(
							"cbuild", "pkg_loader", NULL, "include does not exist: '%.*s'", inc.len, inc.data);
						ret = 1;
					}
				} else {
					inc = STRV("include");
					path_push(&path, inc);
					if (!fs_isdir(fs, STRVS(path))) {
						inc.len = 0;
					}
				}
				path.len = path_len;

				if (inc.len > 0) {
					pkgs_set_str(pkgs, pkg->strs[PKG_INC], inc);
				}

				if (cfg_has_var(cfg, tbl, STRV("deps"), &var)) {
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

						if (pkgs_add_dep(pkgs, pkg_id, dep_str) == NULL) {
							log_error("cbuild", "pkg_loader", NULL, "failed to dependency");
							ret = 1;
						}
					}
				}

				pkg->loaded = 1;
			}
		} else if (strv_eq(key, STRV("target"))) {
			if (pkg == NULL) {
				log_error("cbuild", "pkg_loader", NULL, "target must be defined in a package");
				ret = 1;
				continue;
			}

			target_t *target = pkg_add_target(pkg, &pkgs->targets, strvbuf_get(&pkgs->strs, pkg->strs[PKG_NAME]), NULL);
			if (target == NULL) {
				log_error("cbuild", "pkg_loader", NULL, "failed to add target");
				ret = 1;
			} else {
				cfg_var_t cfg_type;
				if (cfg_has_var(cfg, tbl, STRV("type"), &cfg_type)) {
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
					if (strvbuf_get(&pkgs->strs, pkg->strs[PKG_SRC]).len > 0) {
						target->type = TARGET_TYPE_EXE;
					}
					if (strvbuf_get(&pkgs->strs, pkg->strs[PKG_INC]).len > 0) {
						target->type = TARGET_TYPE_LIB;
					}
				}
			}
		} else if (strv_eq(key, STRV("ext"))) {
			path_t ext_dir = {0};
			path_init(&ext_dir, STRV("tmp"));
			path_push(&ext_dir, STRV("ext"));
			path_t tmp = {0};
			path_init(&tmp, proj_dir);

			size_t ext_len = ext_dir.len;

			create_tmp(fs, proj_dir);

			tmp.len = proj_dir.len;
			path_push(&tmp, STRVS(ext_dir));
			if (!fs_isdir(fs, STRVS(tmp))) {
				fs_mkdir(fs, STRVS(tmp));
			}
			size_t proj_ext_len = tmp.len;

			void *data;
			cfg_var_t dep;
			cfg_foreach(cfg, tbl, data, &dep)
			{
				tmp.len	    = proj_ext_len;
				ext_dir.len = ext_len;

				strv_t name = cfg_get_key(cfg, dep);
				strv_t uri;
				if (cfg_get_str(cfg, dep, &uri)) {
					log_error("cbuild", "pkg_loader", NULL, "invalid extern format");
					ret = 1;
					continue;
				}

				pkg_t *pkg = pkgs_add(pkgs, NULL);
				if (pkg == NULL) {
					ret = 1;
					continue;
				}

				pkgs_set_str(pkgs, pkg->strs[PKG_NAME], name);

				if (pkgs_set_uri(pkgs, pkg, uri)) {
					ret = 1;
					continue;
				}

				path_push(&tmp, name);

				size_t git_len = tmp.len;
				path_push(&tmp, STRV(".git"));
				if (!fs_isdir(fs, STRVS(tmp))) {
					tmp.len	 = git_len;
					buf->len = 0;
					str_cat(buf, STRV("git clone "));
					str_cat(buf, strvbuf_get(&pkgs->strs, pkg->strs[PKG_URL]));
					str_cat(buf, STRV(" "));
					str_cat(buf, STRVS(tmp));
					if (proc) {
						log_info("cbuild", "proj_loader", NULL, "build: %.*s", buf->len, buf->data);
						proc_cmd(proc, STRVS(*buf));
					}
				}

				path_push(&ext_dir, name);
				path_push(&ext_dir, STRV(""));
				ret |= pkg_load(fs, proj_dir, name, STRVS(ext_dir), pkgs, alloc, buf) == NULL;
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

	return ret;
}
