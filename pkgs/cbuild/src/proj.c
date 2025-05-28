#include "proj.h"

#include "fs.h"
#include "log.h"
#include "pkg_loader.h"

proj_t *proj_init(proj_t *proj, uint pkgs_cap, alloc_t alloc)
{
	if (proj == NULL) {
		return NULL;
	}

	if (pkgs_init(&proj->pkgs, pkgs_cap, alloc) == NULL || targets_init(&proj->targets, pkgs_cap, alloc) == NULL) {
		return NULL;
	}

	return proj;
}

void proj_free(proj_t *proj)
{
	if (proj == NULL) {
		return;
	}

	targets_free(&proj->targets);
	pkgs_free(&proj->pkgs);
}

int proj_set_dir(proj_t *proj, fs_t *fs, strv_t dir)
{
	if (proj == NULL) {
		return 1;
	}

	int ret = 0;

	path_init(&proj->dir, dir);
	path_init(&proj->outdir, STRV("bin" SEP "${ARCH}-${CONFIG}/"));

	path_t tmp = {0};
	path_init(&tmp, dir);
	path_child(&tmp, STRV("src"));
	int is_src = fs_isdir(fs, STRVS(tmp));

	path_init(&tmp, dir);
	path_child(&tmp, STRV("pkgs"));
	int is_pkgs = fs_isdir(fs, STRVS(tmp));

	if (!is_src && !is_pkgs) {
		log_error("build", "proj", NULL, "No 'src' or 'pkgs' folder found: %.*s\n", dir.len, dir.data);
		return 1;
	} else if (is_src && is_pkgs) {
		log_error("build", "proj", NULL, "Only one of 'src' or 'pkgs' folder expected: %.*s\n", dir.len, dir.data);
		return 1;
	}

	if (is_src) {
		strv_t name;
		pathv_get_dir(dir, &name);

		uint id;
		if (proj_set_pkg(proj, name, &id) == NULL) {
			ret = 1;
		} else {
			ret |= pkg_load(id, fs, dir, &proj->pkgs, &proj->targets, proj->pkgs.alloc);
		}
	}

	if (is_pkgs) {
		strbuf_t dirs = {0};
		strbuf_init(&dirs, 4, 8, ALLOC_STD);
		fs_lsdir(fs, STRVS(tmp), &dirs);

		uint index = 0;
		strv_t dir;

		size_t tmp_len = tmp.len;
		strbuf_foreach(&dirs, index, dir)
		{
			path_child(&tmp, dir);

			uint id;
			if (pkgs_add_pkg(&proj->pkgs, dir, &id) == NULL) {
				ret = 1; // LCOV_EXCL_LINE
			} else {
				ret |= pkg_load(id, fs, STRVS(tmp), &proj->pkgs, &proj->targets, proj->pkgs.alloc);
			}

			tmp.len = tmp_len;
		}

		strbuf_free(&dirs);
	}

	return ret;
}

pkg_t *proj_set_pkg(proj_t *proj, strv_t name, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	proj->is_pkg = 1;

	return pkgs_add_pkg(&proj->pkgs, name, id);
}

pkg_t *proj_add_pkg(proj_t *proj, strv_t name, uint *id)
{
	if (proj == NULL) {
		return NULL;
	}

	return pkgs_add_pkg(&proj->pkgs, name, id);
}

size_t proj_print(const proj_t *proj, dst_t dst)
{
	if (proj == NULL) {
		return 0;
	}

	size_t off = dst.off;

	dst.off += dputf(dst,
			 "[project]\n"
			 "DIR: %.*s\n"
			 "OUTDIR: %.*s\n"
			 "\n",
			 proj->dir.len,
			 proj->dir.data,
			 proj->outdir.len,
			 proj->outdir.data);

	dst.off += pkgs_print(&proj->pkgs, &proj->targets, dst);

	return dst.off - off;
}
