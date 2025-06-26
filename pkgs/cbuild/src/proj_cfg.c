#include "proj_cfg.h"

#include "log.h"
#include "proj_fs.h"
#include "proj_utils.h"

int proj_cfg(proj_t *proj, cfg_t *cfg, cfg_var_t root, fs_t *fs, proc_t *proc, strv_t proj_dir, strv_t pkg_dir, strv_t pkg_name, str_t *buf,
	     alloc_t alloc)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;

	if (pkg_name.data) {
		uint pkg_id, target_id;

		pkg_t *pkg = pkg = proj_add_pkg_target(proj, pkg_name, &pkg_id, &target_id);
		if (pkg == NULL) {
			log_error("cbuild", "proj_cfg", NULL, "failed to add package: '%.*s'", pkg_name.len, pkg_name.data);
			return 1;
		}
		target_t *target = proj_get_target(proj, target_id);

		proj_set_str(proj, pkg->strs + PKG_DIR, pkg_dir);

		path_t path = {0};
		path_init(&path, proj_dir);
		path_push(&path, pkg_dir);
		size_t path_len = path.len;

		path_push(&path, STRV("src"));
		if (fs_isdir(fs, STRVS(path))) {
			proj_set_str(proj, pkg->strs + PKG_SRC, STRV("src"));
			target->type = TARGET_TYPE_EXE;
		}
		path.len = path_len;

		path_push(&path, STRV("include"));
		if (fs_isdir(fs, STRVS(path))) {
			proj_set_str(proj, pkg->strs + PKG_INC, STRV("include"));
			target->type = TARGET_TYPE_LIB;
		}
		path.len = path_len;

		path_push(&path, STRV("test"));
		if (fs_isdir(fs, STRVS(path))) {
			uint test_target;
			proj_set_str(proj, pkg->strs + PKG_TST, STRV("test"));
			target	     = proj_add_target(proj, pkg_id, STRV("test"), &test_target);
			target->type = TARGET_TYPE_TST;
			proj_add_dep(proj, test_target, target_id);
		}
		path.len = path_len;
	}

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
			} else {
				name = pkg_name;
			}

			pkg = proj_find_pkg(proj, pkg_name, &pkg_id);
			if (pkg) {
				target = proj_find_target(proj, pkg_id, pkg_name, &target_id);
			}

			if (cfg_has_var(cfg, tbl, STRV("name"), &var)) {
				cfg_get_lit(cfg, var, &name);
				if (pkg == NULL || pkg->inited) {
					pkg = proj_find_pkg(proj, name, &pkg_id);
					if (pkg == NULL) {
						pkg = proj_add_pkg_target(proj, name, &pkg_id, &target_id);
						if (pkg == NULL) {
							log_error("cbuild",
								  "proj_cfg",
								  NULL,
								  "failed to add package: '%.*s'",
								  name.len,
								  name.data);
							ret = 1;
							continue;
						}
						target = proj_get_target(proj, target_id);
					} else if (pkg->inited) {
						log_error(
							"cbuild", "proj_cfg", NULL, "package already exists: '%.*s'", name.len, name.data);
						ret    = 1;
						target = proj_find_target(proj, pkg_id, name, &target_id);
					} else {
						target = proj_find_target(proj, pkg_id, name, &target_id);
					}
				} else {
					proj_set_str(proj, pkg->strs + PKG_NAME, name);
					proj_set_str(proj, target->strs + TARGET_NAME, name);
				}
			} else {
				if (pkg == NULL || pkg->inited) {
					log_error("cbuild", "proj_cfg", NULL, "package name is required");
					ret = 1;
					continue;
				}
			}

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

			strv_t test = {0};
			if (cfg_has_var(cfg, tbl, STRV("test"), &var)) {
				cfg_get_str(cfg, var, &test);
				path_push(&path, test);
				if (!fs_isdir(fs, STRVS(path))) {
					test.len = 0;
					log_error("cbuild", "proj_cfg", NULL, "test does not exist: '%.*s'", path.len, path.data);
					ret = 1;
				}
			} else {
				test = STRV("test");
				path_push(&path, test);
				if (!fs_isdir(fs, STRVS(path))) {
					test.len = 0;
				}
			}
			path.len = path_len;

			if (test.len > 0) {
				proj_set_str(proj, pkg->strs + PKG_TST, test);
				target	     = proj_find_target(proj, pkg_id, STRV("test"), &target_id);
				target->type = TARGET_TYPE_TST;
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
					continue;
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
			void *data;
			cfg_var_t ext;
			cfg_foreach(cfg, tbl, data, &ext)
			{
				strv_t name = cfg_get_key(cfg, ext);
				strv_t uri;
				if (cfg_get_str(cfg, ext, &uri)) {
					log_error("cbuild", "pkg_loader", NULL, "invalid extern format");
					ret = 1;
					continue;
				}

				pkg_t *ext_pkg = proj_add_pkg_target(proj, name, NULL, NULL);
				if (ext_pkg == NULL) {
					ret = 1;
					continue;
				}

				if (proj_set_ext_uri(proj, ext_pkg, uri)) {
					ret = 1;
					continue;
				}

				path_t dir = {0};
				path_init(&dir, name);
				path_push(&dir, STRV(""));

				ret |= proj_fs_git(proj,
						   fs,
						   proc,
						   proj_dir,
						   STRVS(dir),
						   name,
						   proj_get_str(proj, ext_pkg->strs + PKG_URL),
						   buf,
						   alloc);
			}
		}
	}

	return ret;
}
