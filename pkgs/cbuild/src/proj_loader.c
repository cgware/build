#include "proj_loader.h"

#include "file/cfg_prs.h"
#include "log.h"
#include "pkg_loader.h"
#include "proc.h"

static int create_tmp(fs_t *fs, strv_t dir)
{
	path_t path = {0};
	path_init(&path, dir);

	path_child(&path, STRV("tmp"));
	if (!fs_isdir(fs, STRVS(path))) {
		fs_mkdir(fs, STRVS(path));
	}

	path_child(&path, STRV(".gitignore"));
	if (!fs_isfile(fs, STRVS(path))) {
		void *f;
		fs_open(fs, STRVS(path), "w", &f);
		fs_write(fs, f, STRV("*"));
		fs_close(fs, f);
	}

	return 0;
}

int proj_load(fs_t *fs, proc_t *proc, strv_t dir, proj_t *proj, alloc_t alloc, str_t *buf)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;

	path_init(&proj->dir, dir);

	cfg_t cfg = {0};
	cfg_var_t root;
	cfg_init(&cfg, 4, 4, alloc);

	path_t tmp = proj->dir;
	path_child(&tmp, STRV("proj.cfg"));
	if (fs_isfile(fs, STRVS(tmp))) {
		if (buf) {
			cfg_prs_t prs = {0};
			cfg_prs_init(&prs, alloc);
			fs_read(fs, STRVS(tmp), 0, buf);
			cfg_prs_parse(&prs, STRVS(*buf), &cfg, alloc, &root, DST_STD());
			cfg_prs_free(&prs);
		}
	} else {
		cfg_root(&cfg, &root);
	}

	ret |= proj_set_cfg(proj, &cfg, root, fs, proc, alloc, buf);

	cfg_free(&cfg);

	path_init(&proj->outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	tmp.len = proj->dir.len;
	path_child(&tmp, STRV("src"));
	int is_src = fs_isdir(fs, STRVS(tmp));

	path_t pkgs = proj->dir;
	path_child(&pkgs, STRV("pkgs"));
	int is_pkgs = fs_isdir(fs, STRVS(pkgs));

	if (!is_src && !is_pkgs) {
		log_error("cbuild", "proj_loader", NULL, "No 'src' or 'pkgs' folder found: %.*s\n", proj->dir.len, proj->dir.data);
		return 1;
	} else if (is_src && is_pkgs) {
		log_error("cbuild",
			  "proj_loader",
			  NULL,
			  "Only one of 'src' or 'pkgs' folder expected: %.*s\n",
			  proj->dir.len,
			  proj->dir.data);
		return 1;
	}

	if (is_src) {
		ret |= pkg_load(fs, STRVS(proj->dir), STRV_NULL, &proj->pkgs, alloc, buf) == NULL;
	}

	if (is_pkgs) {
		strbuf_t dirs = {0};
		strbuf_init(&dirs, 4, 8, alloc);
		fs_lsdir(fs, STRVS(pkgs), &dirs);

		path_init(&tmp, STRV("pkgs/"));

		uint index = 0;
		strv_t folder;
		size_t pkgs_len = tmp.len;
		strbuf_foreach(&dirs, index, folder)
		{
			path_child(&tmp, folder);
			path_child(&tmp, STRV(""));
			ret |= pkg_load(fs, dir, STRVS(tmp), &proj->pkgs, alloc, buf) == NULL;
			tmp.len = pkgs_len;
		}

		strbuf_free(&dirs);
	}

	return ret;
}

int proj_set_cfg(proj_t *proj, const cfg_t *cfg, cfg_var_t root, fs_t *fs, proc_t *proc, alloc_t alloc, str_t *buf)
{
	if (proj == NULL || cfg == NULL) {
		return 1;
	}

	int ret = 0;
	(void)fs;
	(void)buf;

	cfg_var_t deps;
	if (cfg_has_var(cfg, root, STRV("ext"), &deps)) {
		path_t ext_dir = {0};
		path_init(&ext_dir, STRV("tmp"));
		path_child(&ext_dir, STRV("ext"));
		path_t tmp     = proj->dir;
		size_t ext_len = ext_dir.len;

		create_tmp(fs, STRVS(proj->dir));

		tmp.len = proj->dir.len;
		path_child(&tmp, STRVS(ext_dir));
		if (!fs_isdir(fs, STRVS(tmp))) {
			fs_mkdir(fs, STRVS(tmp));
		}
		size_t proj_ext_len = tmp.len;

		void *data;
		cfg_var_t dep;
		cfg_foreach(cfg, deps, data, &dep)
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

			pkg_t *pkg = proj_add_pkg(proj, NULL);
			if (pkg == NULL) {
				ret = 1;
				continue;
			}

			pkgs_set_str(&proj->pkgs, pkg->strs[PKG_NAME], name);

			if (pkgs_set_uri(&proj->pkgs, pkg, uri)) {
				ret = 1;
				continue;
			}

			path_child(&tmp, name);

			size_t git_len = tmp.len;
			path_child(&tmp, STRV(".git"));
			if (!fs_isdir(fs, STRVS(tmp))) {
				tmp.len	 = git_len;
				buf->len = 0;
				str_cat(buf, STRV("git clone "));
				str_cat(buf, strvbuf_get(&proj->pkgs.strs, pkg->strs[PKG_URL]));
				str_cat(buf, STRV(" "));
				str_cat(buf, STRVS(tmp));
				if (proc) {
					log_info("cbuild", "proj_loader", NULL, "build: %.*s", buf->len, buf->data);
					proc_cmd(proc, STRVS(*buf));
				}
			}

			path_child(&ext_dir, name);
			path_child(&ext_dir, STRV(""));
			ret |= pkg_load(fs, STRVS(proj->dir), STRVS(ext_dir), &proj->pkgs, alloc, buf) == NULL;
		}
	}

	return ret;
}
