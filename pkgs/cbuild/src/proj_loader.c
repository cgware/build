#include "proj_loader.h"

#include "file/cfg_prs.h"
#include "log.h"
#include "pkg_loader.h"
#include "proc.h"

int proj_load(fs_t *fs, proc_t *proc, strv_t dir, strv_t name, proj_t *proj, alloc_t alloc, str_t *buf)
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
	path_push(&tmp, STRV("build.cfg"));
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

	ret |= pkg_set_cfg(proj, &cfg, root, fs, proc, alloc, buf);

	cfg_free(&cfg);

	path_init(&proj->outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	tmp.len = proj->dir.len;
	path_push(&tmp, STRV("src"));
	int is_src = fs_isdir(fs, STRVS(tmp));

	path_t pkgs = proj->dir;
	path_push(&pkgs, STRV("pkgs"));
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
		ret |= pkg_load(fs, STRVS(proj->dir), name, STRV_NULL, &proj->pkgs, alloc, buf) == NULL;
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
			path_push(&tmp, folder);
			path_push(&tmp, STRV(""));
			ret |= pkg_load(fs, dir, name, STRVS(tmp), &proj->pkgs, alloc, buf) == NULL;
			tmp.len = pkgs_len;
		}

		strbuf_free(&dirs);
	}

	return ret;
}
