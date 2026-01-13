#include "proj_utils.h"

#include "log.h"
#include "path.h"

static strv_t until_char(strv_t str, size_t *i, char c)
{
	size_t start = *i;

	while (*i < str.len && str.data[*i] != c) {
		(*i)++;
	}

	if (*i >= str.len || str.data[*i] != c) {
		*i = start;
		return STRV_NULL;
	}

	(*i)++;

	return STRVN(&str.data[start], (*i - 1) - start);
}

static strv_t after_char(strv_t str, size_t *i, char c)
{
	size_t start = *i;
	size_t end   = *i;

	while (start > 0 && str.data[start - 1] != c) {
		start--;
	}
	*i = start;

	if (*i - 1 == 0 || str.data[*i - 1] != c) {
		*i = end;
		return STRV_NULL;
	}

	(*i)--;

	return STRVN(&str.data[start], end - start);
}

static int next_char(strv_t str, size_t *i, char c)
{
	if (*i >= str.len || str.data[*i] != c) {
		return 0;
	}

	(*i)++;
	return 1;
}

int proj_set_uri(proj_t *proj, pkg_t *pkg, strv_t uri)
{
	if (proj == NULL || pkg == NULL) {
		return 1;
	}

	proj_set_str(proj, pkg->strs + PKG_STR_URI, uri);

	size_t i = 0;

	strv_t proto = until_char(uri, &i, ':');
	if (proto.data == NULL) {
		log_error("cbuild", "proj", NULL, "expected protocol: '%.*s'", uri.len, uri.data);
		log_error("cbuild", "proj", NULL, "                    %*s^", i, "");
		return 1;
	}

	if (strv_eq(proto, STRV("https"))) {
		pkg->uri.proto = PKG_URI_PROTO_HTTPS;
	} else {
		pkg->uri.proto = PKG_URI_PROTO_UNKNOWN;
	}

	if (!next_char(uri, &i, '/')) {
		log_error("cbuild", "proj", NULL, "expected /: '%.*s'", uri.len, uri.data);
		log_error("cbuild", "proj", NULL, "             %*s^", i, "");
		return 1;
	}

	if (!next_char(uri, &i, '/')) {
		log_error("cbuild", "proj", NULL, "expected /: '%.*s'", uri.len, uri.data);
		log_error("cbuild", "proj", NULL, "             %*s^", i, "");
		return 1;
	}

	strv_t host = until_char(uri, &i, '/');
	if (host.data == NULL) {
		log_error("cbuild", "proj", NULL, "expected host: '%.*s'", uri.len, uri.data);
		log_error("cbuild", "proj", NULL, "                %*s^", i, "");
		return 1;
	}

	size_t tmp  = uri.len;
	strv_t file = after_char(uri, &tmp, '/');

	tmp	    = file.len;
	strv_t ext1 = after_char(file, &tmp, '.');
	if (ext1.data == NULL) {
		log_error("cbuild", "proj", NULL, "expected .: '%.*s'", uri.len, uri.data);
		log_error("cbuild", "proj", NULL, "             %*s^", i, "");
		return 1;
	}

	if (strv_eq(ext1, STRV("zip"))) {
		pkg->uri.ext = PKG_URI_EXT_ZIP;
	} else if (strv_eq(ext1, STRV("gz")) || strv_eq(ext1, STRV("xz"))) {
		strv_t ext2 = after_char(file, &tmp, '.');
		if (strv_eq(ext2, STRV("tar"))) {
			pkg->uri.ext = PKG_URI_EXT_TAR;
		}
	} else if (strv_eq(ext1, STRV("git"))) {
		pkg->uri.proto = PKG_URI_PROTO_GIT;
		pkg->uri.ext   = PKG_URI_EXT_UNKNOWN;
	} else {
		pkg->uri.ext = PKG_URI_EXT_UNKNOWN;
	}

	strv_t name = STRVN(file.data, tmp);
	strv_t ver  = STRV_NULL;
	strv_t ext  = STRVN(&file.data[tmp], file.len - tmp);

	if (strv_cmpn(host, STRV("github.com"), 10) == 0) {
		strv_t user = until_char(uri, &i, '/');
		if (user.data == NULL) {
			log_error("cbuild", "proj", NULL, "expected user: '%.*s'", uri.len, uri.data);
			log_error("cbuild", "proj", NULL, "                %*s^", i, "");
			return 1;
		}

		strv_t repo = until_char(uri, &i, '/');
		if (repo.data == NULL) {
			log_error("cbuild", "proj", NULL, "expected repo: '%.*s'", uri.len, uri.data);
			log_error("cbuild", "proj", NULL, "                %*s^", i, "");
			return 1;
		}

		strv_t ar = until_char(uri, &i, '/');
		if (strv_eq(ar, STRV("archive"))) {
			strv_t refs = until_char(uri, &i, '/');
			if (strv_eq(refs, STRV("refs"))) {
				strv_t ht = until_char(uri, &i, '/');
				if (strv_eq(ht, STRV("tags"))) {
					if (name.data[0] == 'v') {
						name = STRVN(&name.data[1], name.len - 1);
					}
				} else if (strv_eq(ht, STRV("heads"))) {

				} else {
					log_error("cbuild", "proj", NULL, "expexted tags or heads: '%.*s'", uri.len, uri.data);
					log_error("cbuild", "proj", NULL, "                         %*s^", i, "");
					return 1;
				}
			}
		} else if (strv_eq(ar, STRV("releases"))) {
			strv_t download = until_char(uri, &i, '/');
			if (!strv_eq(download, STRV("download"))) {
				log_error("cbuild", "proj", NULL, "expected download: '%.*s'", uri.len, uri.data);
				log_error("cbuild", "proj", NULL, "                    %*s^", i, "");
				return 1;
			}

			strv_t tag = until_char(uri, &i, '/');
			if (tag.len < 1) {
				log_error("cbuild", "proj", NULL, "expected tag: '%.*s'", uri.len, uri.data);
				log_error("cbuild", "proj", NULL, "               %*s^", i, "");
				return 1;
			}

			if (tag.data[0] == 'v') {
				name = STRVN(&tag.data[1], tag.len - 1);
			} else {
				name = tag;
			}
		} else {
			log_error("cbuild", "proj", NULL, "expected archive or releases: '%.*s'", uri.len, uri.data);
			log_error("cbuild", "proj", NULL, "                               %*s^", i, "");
			return 1;
		}

		ver  = name;
		name = repo;
	} else {
		size_t tmp  = name.len;
		strv_t part = after_char(name, &tmp, '-');
		if (part.data != NULL) {
			name = STRVN(name.data, tmp);
			ver  = part;
		}
	}

	proj_set_str(proj, pkg->strs + PKG_STR_URI_NAME, name);
	proj_set_str(proj, pkg->strs + PKG_STR_URI_VER, ver);

	str_t buf = strz(16);
	str_cat(&buf, name);
	if (ver.data) {
		str_cat(&buf, STRV("-"));
		str_cat(&buf, ver);
	}
	size_t len = buf.len;
	str_cat(&buf, ext);
	proj_set_str(proj, pkg->strs + PKG_STR_URI_FILE, STRVS(buf));

	buf.len = len;
	str_cat(&buf, STRV(SEP));
	proj_set_str(proj, pkg->strs + PKG_STR_URI_DIR, STRVS(buf));

	str_free(&buf);

	return 0;
}
