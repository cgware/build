#include "proj_fs.h"

#include "file/cfg_prs.h"
#include "proj_cfg.h"
#include "proj_utils.h"

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

int proj_fs_child_ng(proj_t *proj, fs_t *fs, proc_t *proc, strv_t proj_dir, uint *pkg_id, str_t *buf, alloc_t alloc);

int proj_fs(proj_t *proj, fs_t *fs, proc_t *proc, strv_t proj_dir, strv_t pkg_dir, strv_t pkg_name, str_t *buf, alloc_t alloc)
{
	if (proj == NULL) {
		return 1;
	}

	(void)pkg_dir;

	log_info("cbuild", "proj_fs", NULL, "loading project: '%.*s'", proj_dir.len, proj_dir.data);

	proj->name = pkg_name;
	path_init(&proj->outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	return proj_fs_child_ng(proj, fs, proc, proj_dir, NULL, buf, alloc);
}

int proj_fs_child_ng(proj_t *proj, fs_t *fs, proc_t *proc, strv_t proj_dir, uint *pkg_id, str_t *buf, alloc_t alloc)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;

	path_t path = {0};
	path_init(&path, proj_dir);
	pkg_t *pkg = pkg_id ? proj_get_pkg(proj, *pkg_id) : NULL;
	if (pkg) {
		path_push(&path, proj_get_str(proj, pkg->strs + PKG_DIR));
	}
	size_t path_len = path.len;

	cfg_t scfg = {0};
	cfg_var_t root;
	cfg_init(&scfg, 4, 4, alloc);

	path.len = path_len;
	path_push(&path, STRV("build.cfg"));
	if (fs_isfile(fs, STRVS(path))) {
		cfg_prs_t prs = {0};
		cfg_prs_init(&prs, alloc);
		fs_read(fs, STRVS(path), 0, buf);
		cfg_prs_parse(&prs, STRVS(*buf), &scfg, alloc, &root, DST_STD());
		cfg_prs_free(&prs);

		cfg_t *cfg = &scfg;
		cfg_var_t var;
		strv_t val = {0};

		cfg_print(cfg, root, DST_STD());

		void *data;
		cfg_var_t tbl;
		cfg_foreach(cfg, root, data, &tbl)
		{
			strv_t key = cfg_get_key(cfg, tbl);
			if (strv_eq(key, STRV("pkg"))) {
				if (pkg == NULL) {
					pkg = proj_add_pkg(proj, NULL);
				}
				if (cfg_has_var(cfg, tbl, STRV("uri"), &var)) {
					cfg_get_str(cfg, var, &val);
					proj_set_uri(proj, pkg, val);
				}
				if (cfg_has_var(cfg, tbl, STRV("uriroot"), &var)) {
					cfg_get_str(cfg, var, &val);
					proj_set_str(proj, pkg->strs + PKG_URI_ROOT, val);
				}
			} else if (strv_eq(key, STRV("ext"))) {
				void *data;
				cfg_var_t ext;
				cfg_foreach(cfg, tbl, data, &ext)
				{
					strv_t uri;
					if (cfg_get_str(cfg, ext, &uri)) {
						log_error("cbuild", "pkg_loader", NULL, "invalid extern format");
						ret = 1;
						continue;
					}

					uint ext_pkg_id;
					pkg_t *ext_pkg = proj_add_pkg(proj, &ext_pkg_id);

					if (proj_set_uri(proj, ext_pkg, uri)) {
						ret = 1;
						continue;
					}

					create_tmp(fs, proj_dir);

					path_t dir = {0};
					path_init(&dir, STRV("tmp"));
					path_push(&dir, STRV("ext"));
					path_push(&dir, STRV("IDK"));
					path_push(&dir, STRV(""));

					proj_set_str(proj, ext_pkg->strs + PKG_DIR, STRVS(dir));

					path_t path = {0};
					path_init(&path, proj_dir);
					path_push(&path, STRVS(dir));
					size_t path_len = path.len;

					if (!fs_isdir(fs, STRVS(path))) {
						fs_mkpath(fs, proj_dir, STRVS(dir));
					}

					path_push(&path, STRV(".git"));
					if (!fs_isdir(fs, STRVS(path))) {
						path.len = path_len;
						if (buf) {
							buf->len = 0;
							str_cat(buf, STRV("git clone "));
							str_cat(buf, uri);
							str_cat(buf, STRV(" "));
							str_cat(buf, STRVS(path));
							if (proc) {
								log_info("cbuild",
									 "proj_fs",
									 NULL,
									 "cloning package: %.*s",
									 uri.len,
									 uri.data);
								proc_cmd(proc, STRVS(*buf));
							}
						}
					}

					proj_fs_child_ng(proj, fs, proc, proj_dir, &ext_pkg_id, buf, alloc);
				}
			}
		}
		cfg_free(&scfg);
	} else {
		target_t *target = NULL;
		path.len	 = path_len;
		path_push(&path, STRV("src"));
		if (fs_isdir(fs, STRVS(path))) {
			proj_set_str(proj, pkg->strs + PKG_SRC, STRV("src"));
			if (target == NULL) {
				target = proj_add_target(proj, *pkg_id, NULL);
			}
			target->type = TARGET_TYPE_EXE;
		}
		// TODO: create target type for target which has only include dir: TARGET_TYPE_HEADER?
		path.len = path_len;
		path_push(&path, STRV("include"));
		if (fs_isdir(fs, STRVS(path))) {
			proj_set_str(proj, pkg->strs + PKG_INC, STRV("include"));
			if (target == NULL) {
				target = proj_add_target(proj, *pkg_id, NULL);
			}
			target->type = TARGET_TYPE_LIB;
		}
		path.len = path_len;
	}
	return ret;
}

/*
int proj_fs_child(proj_t *proj, fs_t *fs, proc_t *proc, strv_t proj_dir, strv_t pkg_dir, strv_t pkg_name, str_t *buf, alloc_t alloc)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;

	path_t path = {0};
	path_init(&path, proj_dir);
	path_push(&path, pkg_dir);
	size_t path_len = path.len;

	path_push(&path, STRV("pkgs"));

	path_t dir = {0};
	path_init(&dir, pkg_dir);
	path_push(&dir, STRV("pkgs"));
	size_t dir_len = dir.len;

	if (fs_isdir(fs, STRVS(path))) {
		strbuf_t pkgs = {0};
		strbuf_init(&pkgs, 8, 16, alloc);

		fs_lsdir(fs, STRVS(path), &pkgs);

		uint i = 0;
		strv_t pkg;
		strbuf_foreach(&pkgs, i, pkg)
		{
			path_push(&dir, pkg);
			path_push(&dir, STRV(""));

			ret |= proj_fs_child(proj, fs, proc, proj_dir, STRVS(dir), pkg, buf, alloc);

			dir.len = dir_len;
		}

		strbuf_free(&pkgs);
		path.len = path_len;
		pkg_name = STRV_NULL;
	} else {
		path.len = path_len;
		log_info("cbuild", "proj_fs", NULL, "loading package: '%.*s'", path.len, path.data);
	}

	cfg_t cfg = {0};
	cfg_var_t root;
	cfg_init(&cfg, 4, 4, alloc);

	path.len = path_len;
	path_push(&path, STRV("build.cfg"));
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

	ret |= proj_cfg(proj, &cfg, root, fs, proc, proj_dir, pkg_dir, pkg_name, buf, alloc);

	cfg_free(&cfg);

	return ret;
}

int proj_fs_git(proj_t *proj, fs_t *fs, proc_t *proc, strv_t proj_dir, strv_t pkg_dir, strv_t pkg_name, strv_t url, str_t *buf,
		alloc_t alloc)
{
	if (proj == NULL) {
		return 1;
	}

	create_tmp(fs, proj_dir);

	path_t dir = {0};
	path_init(&dir, STRV("tmp"));
	path_push(&dir, STRV("ext"));
	path_push(&dir, pkg_dir);

	path_t path = {0};
	path_init(&path, proj_dir);
	path_push(&path, STRVS(dir));
	size_t path_len = path.len;

	if (!fs_isdir(fs, STRVS(path))) {
		fs_mkpath(fs, proj_dir, STRVS(dir));
	}

	path_push(&path, STRV(".git"));
	if (!fs_isdir(fs, STRVS(path))) {
		path.len = path_len;
		if (buf) {
			buf->len = 0;
			str_cat(buf, STRV("git clone "));
			str_cat(buf, url);
			str_cat(buf, STRV(" "));
			str_cat(buf, STRVS(path));
			if (proc) {
				log_info("cbuild", "proj_fs", NULL, "cloning package: %.*s", url.len, url.data);
				proc_cmd(proc, STRVS(*buf));
			}
		}
	}

	return proj_fs_child(proj, fs, proc, proj_dir, STRVS(dir), pkg_name, buf, alloc);
}
*/
