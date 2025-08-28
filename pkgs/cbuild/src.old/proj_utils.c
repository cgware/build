#include "proj_utils.h"

#include "log.h"

/*
pkg_t *proj_add_pkg_target(proj_t *proj, strv_t name, uint *pkg_id, uint *target_id)
{
	if (proj == NULL) {
		return NULL;
	}

	uint pid;
	pkg_t *pkg = name.data ? proj_find_pkg(proj, name, &pid) : NULL;
	if (pkg) {
		pkg_id ? *pkg_id = pid : (uint)0;
		proj_find_target(proj, pid, name, target_id);
		return pkg;
	}

	uint pkgs_cnt = proj->pkgs.cnt;
	uint strs_cnt = proj->strs.off.cnt;

	pkg = proj_add_pkg(proj, name, &pid);
	if (pkg == NULL) {
		log_error("cbuild", "pkg_utils", NULL, "failed to add package: '%.*s'", name.len, name.data);
		return NULL;
	}

	if (proj_add_target(proj, pid, name, target_id) == NULL) {
		proj->pkgs.cnt	   = pkgs_cnt;
		proj->strs.off.cnt = strs_cnt;
		log_error("cbuild", "pkg_utils", NULL, "failed to add target: '%.*s'", name.len, name.data);
		return NULL;
	}

	pkg_id ? *pkg_id = pid : (uint)0;

	return pkg;
}*/
/*
int proj_add_dep_uri(proj_t *proj, uint target, strv_t uri)
{
	if (proj == NULL || proj_get_target(proj, target) == NULL) {
		return 1;
	}

	strv_t pkg_name, target_name;
	if (strv_lsplit(uri, ':', &pkg_name, &target_name)) {
		log_error("cbuild", "pkg_utils", NULL, "failed to resolve dependency: '%.*s'", uri.len, uri.data);
		return 1;
	}

	uint pkgs_cnt	 = proj->pkgs.cnt;
	uint targets_cnt = proj->targets.cnt;
	uint strs_cnt	 = proj->strs.off.cnt;

	uint pkg_id, target_id;
	if (pkg_name.data && proj_add_pkg_target(proj, pkg_name, &pkg_id, &target_id) == NULL) {
		log_error("cbuild", "pkg_utils", NULL, "failed to add dependency: '%.*s'", pkg_name.len, pkg_name.data);
		return 1;
	}

	if (target_name.data && proj_find_target(proj, pkg_id, target_name, &target_id) == NULL &&
	    proj_add_target(proj, pkg_id, target_name, &target_id) == NULL) {
		proj->pkgs.cnt	   = pkgs_cnt;
		proj->targets.cnt  = targets_cnt;
		proj->strs.off.cnt = strs_cnt;
		log_error("cbuild", "pkg_utils", NULL, "failed to add dependency: '%.*s'", pkg_name.len, pkg_name.data);
		return 1;
	}

	return proj_add_dep(proj, target, target_id);
}*/

int proj_set_uri(proj_t *proj, pkg_t *pkg, strv_t uri)
{
	if (proj == NULL || pkg == NULL) {
		return 1;
	}

	strv_t proto = {0};
	strv_t host  = {0};
	strv_t path  = {0};
	strv_t file  = {0};
	strv_t name  = {0};
	strv_t ext   = {0};

	if (strv_lsplit(uri, ':', &proto, &host) || strv_rsplit(uri, '/', &path, &file) || strv_lsplit(file, '.', &name, &ext)) {
		log_error("cbuild", "proj", NULL, "failed to resolve uri: '%.*s'", uri.len, uri.data);
		return 1;
	}

	if (strv_cmpn(host, STRV("//github.com"), 12) == 0) {
		strv_t repo    = {0};
		strv_t archive = {0};
		strv_t refs    = {0};
		strv_t cat     = {0};

		if (strv_rsplit(path, '/', &path, &cat)) {
			log_error("cbuild", "proj", NULL, "failed to resolve uri cat: '%.*s'", uri.len, uri.data);
			return 1;
		}

		if (strv_rsplit(path, '/', &path, &refs) || !strv_eq(refs, STRV("refs"))) {
			log_error("cbuild", "proj", NULL, "failed to resolve uri refs: '%.*s'", uri.len, uri.data);
			return 1;
		}

		if (strv_rsplit(path, '/', &path, &archive) || !strv_eq(archive, STRV("archive"))) {
			log_error("cbuild", "proj", NULL, "failed to resolve uri archive: '%.*s'", uri.len, uri.data);
			return 1;
		}

		if (strv_rsplit(path, '/', &path, &repo)) {
			log_error("cbuild", "proj", NULL, "failed to resolve uri archive: '%.*s'", uri.len, uri.data);
			return 1;
		}

		proj_set_str(proj, pkg->strs + PKG_NAME, repo);
		if (strv_eq(cat, STRV("heads"))) {
			str_t buf = strz(16);
			str_cat(&buf, repo);
			str_cat(&buf, STRV("-"));
			str_cat(&buf, name);
			str_cat(&buf, STRV(SEP));
			proj_set_str(proj, pkg->strs + PKG_URI_DIR, STRVS(buf));
			str_free(&buf);
		}
	}

	if (strv_eq(proto, STRV("https"))) {
		pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	} else {
		log_error("cbuild", "proj", NULL, "not supported protocol: '%.*s'", proto.len, proto.data);
		return 1;
	}

	if (strv_eq(ext, STRV("git"))) {
		pkg->uri.proto = PKG_URI_PROTO_GIT;
		pkg->uri.ext   = PKG_URI_EXT_NONE;
	} else if (strv_eq(ext, STRV("zip"))) {
		pkg->uri.ext = PKG_URI_EXT_ZIP;
	} else {
		pkg->uri.ext = PKG_URI_EXT_NONE;
	}

	return proj_set_str(proj, pkg->strs + PKG_URI, uri) || proj_set_str(proj, pkg->strs + PKG_URI_NAME, name);
}
