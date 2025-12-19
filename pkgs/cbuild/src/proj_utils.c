#include "proj_utils.h"

#include "log.h"
#include "path.h"
#include <stdio.h>

int proj_set_uri(proj_t *proj, pkg_t *pkg, strv_t uri)
{
	if (proj == NULL || pkg == NULL) {
		return 1;
	}

	strv_t proto = {0};
	strv_t tmp   = {0};
	strv_t host  = {0};
	strv_t file  = {0};
	strv_t name  = {0};
	strv_t ext   = {0};

	proj_set_str(proj, pkg->strs + PKG_STR_URI, uri);

	if (strv_lsplit(uri, ':', &proto, &tmp)) {
		log_error("cbuild", "proj", NULL, "invalid uri: '%.*s'", uri.len, uri.data);
		return 1;
	}

	if (strv_eq(proto, STRV("https"))) {
		pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	} else {
		log_error("cbuild", "proj", NULL, "not supported protocol: '%.*s'", proto.len, proto.data);
		return 1;
	}

	if (strv_cmpn(tmp, STRV("//"), 2) != 0) {
		log_error("cbuild", "proj", NULL, "invalid uri (//): '%.*s': '%.*s'", uri.len, uri.data, tmp.len, tmp.data);
		return 1;
	}

	tmp = STRVN(tmp.data + 2, tmp.len - 2);
	strv_lsplit(tmp, '/', &host, &tmp);

	if (strv_rsplit(tmp, '/', &tmp, &file) || strv_rsplit(file, '.', &name, &ext)) {
		log_error("cbuild", "proj", NULL, "invalid uri (file): '%.*s': '%.*s'", uri.len, uri.data, tmp.len, tmp.data);
		return 1;
	}

	proj_set_str(proj, pkg->strs + PKG_STR_URI_FILE, file);

	if (strv_eq(ext, STRV("git"))) {
		pkg->uri.proto = PKG_URI_PROTO_GIT;
		pkg->uri.ext   = PKG_URI_EXT_UNKNOWN;
	} else {
		if (strv_eq(ext, STRV("zip"))) {
			pkg->uri.ext = PKG_URI_EXT_ZIP;
		} else if (strv_eq(ext, STRV("gz"))) {
			strv_t name2 = {0};
			strv_t ext2  = {0};
			if (strv_rsplit(name, '.', &name2, &ext2) == 0 && strv_eq(ext2, STRV("tar"))) {
				name = name2;
				ext  = STRV("tar.gz");
			}
			pkg->uri.ext = PKG_URI_EXT_TAR_GZ;
		} else {
			pkg->uri.ext = PKG_URI_EXT_UNKNOWN;
		}
	}

	if (strv_cmpn(host, STRV("github.com"), 10) == 0) {
		strv_t user    = {0};
		strv_t repo    = {0};
		strv_t archive = {0};
		strv_t refs    = {0};
		strv_t cat     = {0};

		if (strv_lsplit(tmp, '/', &user, &tmp)) {
			log_error("cbuild", "proj", NULL, "invalid uri (user): '%.*s': '%.*s'", uri.len, uri.data, tmp.len, tmp.data);
			return 1;
		}

		if (strv_lsplit(tmp, '/', &repo, &tmp)) {
			log_error("cbuild", "proj", NULL, "invalid uri (repo): '%.*s': '%.*s'", uri.len, uri.data, tmp.len, tmp.data);
			return 1;
		}

		if (strv_lsplit(tmp, '/', &archive, &tmp) || !strv_eq(archive, STRV("archive"))) {
			log_error("cbuild", "proj", NULL, "invalid uri (archive): '%.*s': '%.*s'", uri.len, uri.data, tmp.len, tmp.data);
			return 1;
		}

		if (strv_lsplit(tmp, '/', &refs, &tmp) || !strv_eq(refs, STRV("refs"))) {
			log_error("cbuild", "proj", NULL, "invalid uri (refs): '%.*s': '%.*s'", uri.len, uri.data, tmp.len, tmp.data);
			return 1;
		}

		if (strv_lsplit(tmp, '/', &cat, &tmp)) {
			log_error("cbuild", "proj", NULL, "invalid uri (cat): '%.*s': '%.*s'", uri.len, uri.data, tmp.len, tmp.data);
			return 1;
		}

		proj_set_str(proj, pkg->strs + PKG_STR_URI_NAME, repo);

		if (strv_eq(cat, STRV("heads"))) {
			str_t buf = strz(16);
			str_cat(&buf, repo);
			str_cat(&buf, STRV("-"));
			str_cat(&buf, name);
			proj_set_str(proj, pkg->strs + PKG_STR_URI_VER, name);
			str_cat(&buf, STRV(SEP));
			proj_set_str(proj, pkg->strs + PKG_STR_URI_DIR, STRVS(buf));
			buf.len -= 1;
			str_cat(&buf, STRV(".zip"));
			proj_set_str(proj, pkg->strs + PKG_STR_URI_FILE, STRVS(buf));
			str_free(&buf);
		} else if (strv_eq(cat, STRV("tags"))) {
			str_t buf = strz(16);
			str_cat(&buf, repo);
			str_cat(&buf, STRV("-"));
			if (strv_cmpn(name, STRV("v"), 1) == 0) {
				proj_set_str(proj, pkg->strs + PKG_STR_URI_VER, STRVN(name.data + 1, name.len - 1));
				str_cat(&buf, STRVN(name.data + 1, name.len - 1));
			} else {
				proj_set_str(proj, pkg->strs + PKG_STR_URI_VER, name);
				str_cat(&buf, name);
			}

			str_cat(&buf, STRV(SEP));
			proj_set_str(proj, pkg->strs + PKG_STR_URI_DIR, STRVS(buf));
			buf.len -= 1;
			str_cat(&buf, STRV(".zip"));
			proj_set_str(proj, pkg->strs + PKG_STR_URI_FILE, STRVS(buf));
			str_free(&buf);
		} else {
			log_error("cbuild", "proj", NULL, "invalid uri (cat): '%.*s': '%.*s'", uri.len, uri.data, cat.len, cat.data);
			return 1;
		}
	} else {
		strv_t ver = {0};

		if (strv_lsplit(name, '-', &name, &ver)) {
			log_error("cbuild", "proj", NULL, "invalid uri (user): '%.*s': '%.*s'", uri.len, uri.data, tmp.len, tmp.data);
			return 1;
		}

		proj_set_str(proj, pkg->strs + PKG_STR_URI_NAME, name);
		proj_set_str(proj, pkg->strs + PKG_STR_URI_VER, ver);
	}

	return 0;
}
