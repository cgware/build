#include "proj_utils.h"

#include "log.h"
#include "mem.h"
#include "path.h"
#include "test.h"

TEST(proj_set_uri)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);

	EXPECT_EQ(proj_set_uri(NULL, NULL, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_uri(&proj, NULL, STRV_NULL), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV_NULL), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https:")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://domain/")), 1);
	log_set_quiet(0, 0);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_github_invalid)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);

	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/heads")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/unknown/main")), 1);
	log_set_quiet(0, 0);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_github_branch_zip)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/heads/master.zip")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_ZIP);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://github.com/user/repo/archive/refs/heads/master.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "repo-master.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "repo", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "master", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_DIR);
	EXPECT_STRN(val.data, "repo-master" SEP, val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_github_tag_zip)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/tags/1.0.zip")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_ZIP);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://github.com/user/repo/archive/refs/tags/1.0.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "repo-1.0.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "repo", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "1.0", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_DIR);
	EXPECT_STRN(val.data, "repo-1.0" SEP, val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_github_vtag)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/tags/v1.0.zip")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_ZIP);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://github.com/user/repo/archive/refs/tags/v1.0.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "repo-1.0.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "repo", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "1.0", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_DIR);
	EXPECT_STRN(val.data, "repo-1.0" SEP, val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_github_branch_tar_gz)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/heads/master.tar.gz")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_TAR_GZ);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://github.com/user/repo/archive/refs/heads/master.tar.gz", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "repo-master.tar.gz", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "repo", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "master", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_DIR);
	EXPECT_STRN(val.data, "repo-master" SEP, val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_github_tag_tar_gz)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/tags/1.0.tar.gz")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_TAR_GZ);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://github.com/user/repo/archive/refs/tags/1.0.tar.gz", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "repo-1.0.tar.gz", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "repo", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "1.0", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_DIR);
	EXPECT_STRN(val.data, "repo-1.0" SEP, val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_git)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://domain/repo.git")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_GIT);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_UNKNOWN);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://domain/repo.git", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "repo.git", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "repo", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "", val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_name_zip)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://domain/name.zip")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_ZIP);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://domain/name.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "name.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "name", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "", val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_name_ver_zip)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://domain/name-ver.zip")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_ZIP);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://domain/name-ver.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "name-ver.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "name", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "ver", val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_name_tar_gz)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://domain/name.tar.gz")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_TAR_GZ);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://domain/name.tar.gz", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "name.tar.gz", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "name", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "", val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_name_ver_tar_gz)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://domain/name-ver.tar.gz")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_TAR_GZ);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI);
	EXPECT_STRN(val.data, "https://domain/name-ver.tar.gz", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_FILE);
	EXPECT_STRN(val.data, "name-ver.tar.gz", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_NAME);
	EXPECT_STRN(val.data, "name", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_STR_URI_VER);
	EXPECT_STRN(val.data, "ver", val.len);

	proj_free(&proj);

	END;
}

STEST(proj_utils)
{
	SSTART;

	RUN(proj_set_uri);
	RUN(proj_set_uri_github_invalid);
	RUN(proj_set_uri_github_branch_zip);
	RUN(proj_set_uri_github_tag_zip);
	RUN(proj_set_uri_github_vtag);
	RUN(proj_set_uri_github_branch_tar_gz);
	RUN(proj_set_uri_github_tag_tar_gz);
	RUN(proj_set_uri_git);
	RUN(proj_set_uri_name_zip);
	RUN(proj_set_uri_name_ver_zip);
	RUN(proj_set_uri_name_tar_gz);
	RUN(proj_set_uri_name_ver_tar_gz);

	SEND;
}
