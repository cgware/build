#include "proj_cfg.h"

#include "log.h"
#include "proj_utils.h"

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

int proj_cfg(proj_t *proj, cfg_t *cfg, cfg_var_t root, fs_t *fs, proc_t *proc, strv_t proj_dir, strv_t pkg_dir, str_t *buf)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;

	uint pkg_id, target_id;
	pkg_t *pkg	 = NULL;
	target_t *target = NULL;
	cfg_var_t var;

	void *data;
	cfg_var_t tbl;
	cfg_foreach(cfg, root, data, &tbl)
	{
		strv_t key = cfg_get_key(cfg, tbl);
		if (strv_eq(key, STRV("pkg"))) {
			strv_t name = {0};
			if (cfg_has_var(cfg, tbl, STRV("name"), &var)) {
				cfg_get_lit(cfg, var, &name);
			} else if (pkg_dir.len > 0) {
				strv_t l;
				pathv_rsplit(pkg_dir, &l, &name);
				if (name.len == 0) {
					pathv_rsplit(l, NULL, &name);
				}
			} else {
				name = proj->name;
			}

			pkg = proj_find_pkg(proj, name, &pkg_id);
			if (pkg == NULL) {
				pkg = proj_add_pkg_target(proj, name, &pkg_id, &target_id);
				if (pkg == NULL) {
					log_error("cbuild", "proj_cfg", NULL, "failed to add package: '%.*s'", name.len, name.data);
					ret = 1;
					continue;
				}
			} else if (pkg->inited) {
				log_error("cbuild", "proj_cfg", NULL, "package already exists: '%.*s'", name.len, name.data);
				ret = 1;
			}

			target = proj_get_target(proj, target_id);

			proj_set_str(proj, pkg->strs + PKG_DIR, pkg_dir);

			path_t path = {0};
			path_init(&path, proj_dir);
			path_push(&path, pkg_dir);
			size_t path_len = path.len;

			strv_t src = {0};
			if (cfg_has_var(cfg, tbl, STRV("src"), &var)) {
				cfg_get_str(cfg, var, &src);
				path_push(&path, src);
				if (!fs_isdir(fs, STRVS(path))) {
					src.len = 0;
					log_error("cbuild", "proj_cfg", NULL, "src does not exist: '%.*s'", path.len, path.data);
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
				proj_set_str(proj, pkg->strs + PKG_SRC, src);
				target->type = TARGET_TYPE_EXE;
			}

			strv_t inc = {0};
			if (cfg_has_var(cfg, tbl, STRV("include"), &var)) {
				cfg_get_str(cfg, var, &inc);
				path_push(&path, inc);
				if (!fs_isdir(fs, STRVS(path))) {
					inc.len = 0;
					log_error("cbuild", "proj_cfg", NULL, "include does not exist: '%.*s'", path.len, path.data);
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
				proj_set_str(proj, pkg->strs + PKG_INC, inc);
				target->type = TARGET_TYPE_LIB;
			}

			if (cfg_has_var(cfg, tbl, STRV("deps"), &var)) {
				cfg_var_t dep;
				void *data;
				cfg_foreach(cfg, var, data, &dep)
				{
					strv_t uri;
					if (cfg_get_lit(cfg, dep, &uri)) {
						log_error("cbuild", "proj_cfg", NULL, "invalid dependency");
						ret = 1;
						continue;
					}

					if (proj_add_dep_uri(proj, target_id, uri)) {
						log_error("cbuild", "proj_cfg", NULL, "failed to dependency");
						ret = 1;
					}
				}
			}

			pkg->inited = 1;
		} else if (strv_eq(key, STRV("target"))) {
			if (pkg == NULL) {
				log_error("cbuild", "proj_cfg", NULL, "target must be defined in a package");
				ret = 1;
				continue;
			}

			strv_t name = {0};
			if (cfg_has_var(cfg, tbl, STRV("name"), &var)) {
				cfg_get_lit(cfg, var, &name);
				if (target == NULL || target->inited) {
					target = proj_find_target(proj, pkg_id, name, &target_id);
					if (target == NULL) {
						target = proj_add_target(proj, pkg_id, name, &target_id);
						if (target == NULL) {
							log_error("cbuild",
								  "proj_cfg",
								  NULL,
								  "failed to add target: '%.*s'",
								  name.len,
								  name.data);
							ret = 1;
							continue;
						}
					} else if (target->inited) {
						log_error("cbuild", "proj_cfg", NULL, "target already exists: '%.*s'", name.len, name.data);
						ret = 1;
					}
				} else {
					proj_set_str(proj, target->strs + TARGET_NAME, name);
				}
			} else {
				if (target == NULL || target->inited) {
					log_error("cbuild", "proj_cfg", NULL, "target name is required");
					ret = 1;
				}
			}

			if (cfg_has_var(cfg, tbl, STRV("type"), &var)) {
				strv_t type = {0};
				cfg_get_lit(cfg, var, &type);
				if (strv_eq(type, STRV("EXE"))) {
					target->type = TARGET_TYPE_EXE;
				} else if (strv_eq(type, STRV("LIB"))) {
					target->type = TARGET_TYPE_LIB;
				} else {
					log_error("cbuild",
						  "proj_cfg",
						  NULL,
						  "unknown target type: '%.*s': '%.*s'",
						  name.len,
						  name.data,
						  type.len,
						  type.data);
					ret = 1;
				}
			}

			target->inited = 1;
		} else if (strv_eq(key, STRV("ext"))) {
			path_t ext_dir = {0};
			path_init(&ext_dir, STRV("tmp"));
			path_push(&ext_dir, STRV("ext"));
			path_t tmp = {0};
			path_init(&tmp, proj_dir);

			size_t ext_len = ext_dir.len;

			// TODO: not this method responsibility to create folders and execute commands, move it to somewhere else
			create_tmp(fs, proj_dir);

			tmp.len = proj_dir.len;
			path_push(&tmp, STRVS(ext_dir));
			if (!fs_isdir(fs, STRVS(tmp))) {
				fs_mkpath(fs, proj_dir, STRVS(tmp));
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

				pkg_t *ext_pkg = proj_add_pkg(proj, name, NULL);
				if (ext_pkg == NULL) {
					ret = 1;
					continue;
				}

				if (proj_set_uri(proj, ext_pkg, uri)) {
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
					str_cat(buf, proj_get_str(proj, ext_pkg->strs + PKG_URL));
					str_cat(buf, STRV(" "));
					str_cat(buf, STRVS(tmp));
					if (proc) {
						log_info("cbuild", "proj_loader", NULL, "build: %.*s", buf->len, buf->data);
						proc_cmd(proc, STRVS(*buf));
					}
				}

				path_push(&ext_dir, name);
				path_push(&ext_dir, STRV(""));
				// TODO: implement proj_fs();
				// ret |= pkg_load(fs, proc, proj_dir, name, STRVS(ext_dir), pkgs, alloc, buf) == NULL;
			}
		}
	}

	return ret;
}
