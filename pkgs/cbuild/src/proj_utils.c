#include "proj_utils.h"

#include "log.h"

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
}

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
}

int proj_set_ext_uri(proj_t *proj, pkg_t *pkg, strv_t uri)
{
	if (proj == NULL || pkg == NULL) {
		return 1;
	}

	strv_t proto;
	strv_t url;

	if (strv_lsplit(uri, ':', &proto, &url)) {
		log_error("cbuild", "proj", NULL, "failed to resolve dependency: '%.*s'", uri.len, uri.data);
		return 1;
	}

	if (strv_eq(proto, STRV("git"))) {
		pkg->proto = PKG_URL_GIT;
	} else {
		log_error("cbuild", "proj", NULL, "not supported protocol: '%.*s'", proto.len, proto.data);
		return 1;
	}

	if (url.data == NULL) {
		return 1;
	}

	return proj_set_str(proj, pkg->strs + PKG_URL, url);
}
