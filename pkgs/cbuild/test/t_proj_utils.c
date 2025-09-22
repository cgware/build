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
	strv_t val;

	EXPECT_EQ(proj_set_uri(NULL, NULL, STRV_NULL), 1);
	log_set_quiet(0, 1);
	EXPECT_EQ(proj_set_uri(&proj, NULL, STRV_NULL), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV_NULL), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https:")), 1);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://")), 1);
	log_set_quiet(0, 0);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://host.com/file.zip")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_ZIP);
	val = proj_get_str(&proj, pkg->strs + PKG_URI_STR);
	EXPECT_STRN(val.data, "https://host.com/file.zip", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_URI_NAME);
	EXPECT_STRN(val.data, "file", val.len);

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://host.com/file")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_UNKNOWN);
	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://host.com/repo.git")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_GIT);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_UNKNOWN);

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

TEST(proj_set_uri_github_branch)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/heads/master.zip")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_ZIP);
	val = proj_get_str(&proj, pkg->strs + PKG_NAME);
	EXPECT_STRN(val.data, "repo", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_URI_DIR);
	EXPECT_STRN(val.data, "repo-master" SEP, val.len);

	proj_free(&proj);

	END;
}

TEST(proj_set_uri_github_tag)
{
	START;

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	pkg_t *pkg = proj_add_pkg(&proj, NULL);
	strv_t val;

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/tags/v1.0.zip")), 0);
	EXPECT_EQ(pkg->uri.proto, PKG_URI_PROTO_HTTPS);
	EXPECT_EQ(pkg->uri.ext, PKG_URI_EXT_ZIP);
	val = proj_get_str(&proj, pkg->strs + PKG_NAME);
	EXPECT_STRN(val.data, "repo", val.len);
	val = proj_get_str(&proj, pkg->strs + PKG_URI_DIR);
	EXPECT_STRN(val.data, "repo-1.0" SEP, val.len);

	EXPECT_EQ(proj_set_uri(&proj, pkg, STRV("https://github.com/user/repo/archive/refs/tags/1.0.zip")), 0);
	val = proj_get_str(&proj, pkg->strs + PKG_URI_DIR);
	EXPECT_STRN(val.data, "repo-1.0" SEP, val.len);

	proj_free(&proj);

	END;
}

STEST(proj_utils)
{
	SSTART;

	RUN(proj_set_uri);
	RUN(proj_set_uri_github_invalid);
	RUN(proj_set_uri_github_branch);
	RUN(proj_set_uri_github_tag);

	SEND;
}
