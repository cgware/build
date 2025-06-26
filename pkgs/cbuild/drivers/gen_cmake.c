#include "gen.h"

#include "log.h"
#include "var.h"

static void resolve_dir(const proj_t *proj, strv_t *values, target_type_t type, str_t *buf, path_t *resolved)
{
	buf->len = 0;
	str_cat(buf, STRVS(proj->outdir));
	var_replace(buf, values);
	path_t tmp = {0};
	path_init(&tmp, STRVS(*buf));

	buf->len = 0;
	if (pathv_is_rel(STRVS(tmp))) {
		str_cat(buf, STRV("${CMAKE_SOURCE_DIR}/${PROJDIR}"));
	}
	str_cat(buf, STRVS(tmp));

	path_init_s(resolved, STRVS(*buf), '/');

	switch (type) {
	case TARGET_TYPE_EXE:
		path_push_s(resolved, STRV("bin"), '/');
		break;
	case TARGET_TYPE_LIB:
		path_push_s(resolved, STRV("lib"), '/');
		break;
	case TARGET_TYPE_TST:
		path_push_s(resolved, STRV("test"), '/');
		break;
	default:       // LCOV_EXCL_LINE
		break; // LCOV_EXCL_LINE
	}

	path_push_s(resolved, STRV(""), '/');
}

static int gen_pkg(const proj_t *proj, fs_t *fs, uint id, strv_t build_dir)
{
	const pkg_t *pkg = proj_get_pkg(proj, id);

	path_t path = {0};
	path_init(&path, build_dir);
	strv_t pkg_dir = proj_get_str(proj, pkg->strs + PKG_DIR);
	if (pkg_dir.len > 0) {
		fs_mkpath(fs, STRVS(path), pkg_dir);
	}
	path_push(&path, proj_get_str(proj, pkg->strs + PKG_DIR));
	path_push(&path, STRV("pkg.cmake"));

	log_info("cbuild", "gen_make", NULL, "generating package: '%.*s'", path.len, path.data);

	void *f;
	fs_open(fs, STRVS(path), "w", &f);

	str_t buf = strz(64);

	fs_write(fs, f, STRV("set(PN \""));
	strv_t name = proj_get_str(proj, pkg->strs + PKG_NAME);
	if (name.len > 0) {
		fs_write(fs, f, name);
	}
	fs_write(fs, f, STRV("\")\n"));
	fs_write(fs, f, STRV("set(PKGDIR \""));
	strv_t dir = proj_get_str(proj, pkg->strs + PKG_DIR);
	if (dir.len > 0) {
		fs_write(fs, f, dir);
	}
	fs_write(fs, f, STRV("\")\n"));

	uint i = 0;
	const target_t *target;
	arr_foreach(&proj->targets, i, target)
	{
		if (target->pkg != id) {
			continue;
		}

		fs_write(fs, f, STRV("set(TN \""));
		strv_t name = proj_get_str(proj, target->strs + PKG_NAME);
		if (name.len > 0) {
			fs_write(fs, f, name);
		}
		fs_write(fs, f, STRV("\")\n"));

		fs_write(fs, f, STRV("file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}"));

		path_t tmp = {0};

		switch (target->type) {
		case TARGET_TYPE_TST:
			path_init_s(&tmp, proj_get_str(proj, pkg->strs + PKG_TST), '/');
			break;
		default:
			path_init_s(&tmp, proj_get_str(proj, pkg->strs + PKG_SRC), '/');
			break;
		}

		size_t src_len = tmp.len;
		path_push_s(&tmp, STRV("*.h"), '/');
		fs_write(fs, f, STRVS(tmp));

		fs_write(fs, f, STRV(" ${PROJDIR}${PKGDIR}"));
		tmp.len = src_len;
		path_push_s(&tmp, STRV("*.c"), '/');
		fs_write(fs, f, STRVS(tmp));
		fs_write(fs, f, STRV(")\n"));

		switch (target->type) {
		case TARGET_TYPE_EXE: {
			fs_write(fs, f, STRV("add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"));
			fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} PRIVATE"));
			uint j = 0;
			dep_t *dep;
			arr_foreach(&proj->deps, j, dep)
			{
				if (dep->to != i) {
					continue;
				}

				const target_t *dtarget = proj_get_target(proj, dep->from);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
				fs_write(fs, f, STRV(" "));
				fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_NAME));
				fs_write(fs, f, STRV("_"));
				fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
			}
			fs_write(fs, f, STRV(")\n"));

			break;
		}
		case TARGET_TYPE_LIB: {
			fs_write(fs, f, STRV("add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"));
			strv_t inc = proj_get_str(proj, pkg->strs + PKG_INC);
			if (inc.len > 0) {
				fs_write(fs, f, STRV("target_include_directories(${PN}_${TN} PUBLIC "));
				fs_write(fs, f, STRV("${PROJDIR}${PKGDIR}"));
				fs_write(fs, f, inc);
				fs_write(fs, f, STRV(")\n"));
			}
			fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} PUBLIC"));
			uint j = 0;
			dep_t *dep;
			arr_foreach(&proj->deps, j, dep)
			{
				if (dep->to != i) {
					continue;
				}

				const target_t *dtarget = proj_get_target(proj, dep->from);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
				fs_write(fs, f, STRV(" "));
				fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_NAME));
				fs_write(fs, f, STRV("_"));
				fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
			}
			fs_write(fs, f, STRV(")\n"));
			break;
		}
		case TARGET_TYPE_TST: {
			fs_write(fs, f, STRV("add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"));
			fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} PRIVATE"));
			uint j = 0;
			dep_t *dep;
			arr_foreach(&proj->deps, j, dep)
			{
				if (dep->to != i) {
					continue; // LCOV_EXCL_LINE
				}

				const target_t *dtarget = proj_get_target(proj, dep->from);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
				fs_write(fs, f, STRV(" "));
				fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_NAME));
				fs_write(fs, f, STRV("_"));
				fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
			}
			fs_write(fs, f, STRV(")\n"));

			break;
		}
		default:
			break;
		}

		strv_t values[__VAR_CNT] = {0};

		fs_write(fs,
			 f,
			 STRV("set_target_properties(${PN}_${TN} PROPERTIES\n"
			      "\tOUTPUT_NAME \"${PN}\"\n"));

		path_t outdir = {0};

		struct {
			strv_t name;
			strv_t config;
		} props[__TARGET_TYPE_MAX][3] = {
			[TARGET_TYPE_EXE] =
				{
					{STRVT("RUNTIME_OUTPUT_DIRECTORY"), STRVT("Debug")},
					{STRVT("RUNTIME_OUTPUT_DIRECTORY_DEBUG"), STRVT("Debug")},
					{STRVT("RUNTIME_OUTPUT_DIRECTORY_RELEASE"), STRVT("Release")},
				},
			[TARGET_TYPE_LIB] =
				{
					{STRVT("ARCHIVE_OUTPUT_DIRECTORY"), STRVT("Debug")},
					{STRVT("ARCHIVE_OUTPUT_DIRECTORY_DEBUG"), STRVT("Debug")},
					{STRVT("ARCHIVE_OUTPUT_DIRECTORY_RELEASE"), STRVT("Release")},
				},
			[TARGET_TYPE_TST] =
				{
					{STRVT("RUNTIME_OUTPUT_DIRECTORY"), STRVT("Debug")},
					{STRVT("RUNTIME_OUTPUT_DIRECTORY_DEBUG"), STRVT("Debug")},
					{STRVT("RUNTIME_OUTPUT_DIRECTORY_RELEASE"), STRVT("Release")},
				},
		};

		size_t props_cnt = sizeof(props[target->type]) / sizeof(props[target->type][0]);

		for (size_t i = 0; i < props_cnt; i++) {
			if (props[target->type]->name.data == NULL) {
				continue;
			}

			fs_write(fs, f, STRV("\t"));
			fs_write(fs, f, props[target->type][i].name);
			fs_write(fs, f, STRV(" "));
			values[VAR_CONFIG] = props[target->type][i].config;
			resolve_dir(proj, values, target->type, &buf, &outdir);
			fs_write(fs, f, STRVS(outdir));
			fs_write(fs, f, STRV("\n"));
		}

		switch (target->type) {
		case TARGET_TYPE_LIB:
			fs_write(fs, f, STRV("\tPREFIX \"\"\n"));
			break;
		default:
			break;
		}

		fs_write(fs, f, STRV(")\n"));
	}

	str_free(&buf);
	fs_close(fs, f);

	return 0;
}

static int gen_cmake(const gen_driver_t *drv, const proj_t *proj, strv_t proj_dir, strv_t build_dir)
{
	if (drv == NULL || proj == NULL) {
		return 1;
	}

	path_t path = {0};
	path_init(&path, build_dir);
	path_push(&path, STRV("CMakeLists.txt"));

	log_info("cbuild", "gen_make", NULL, "generating project: '%.*s'", path.len, path.data);

	void *f;
	fs_open(drv->fs, STRVS(path), "w", &f);

	fs_write(drv->fs, f, STRV("cmake_minimum_required(VERSION 3.10)\n\n"));

	fs_write(drv->fs, f, STRV("project("));
	if (proj->name.len > 0) {
		fs_write(drv->fs, f, proj->name);
	}
	fs_write(drv->fs, f, STRV(" LANGUAGES C)\n\n"));

	fs_write(drv->fs, f, STRV("set(PROJDIR \""));

	path_t rel = {0};
	path_calc_rel_s(build_dir, proj_dir, '/', &rel);
	if (rel.len > 0) {
		fs_write(drv->fs, f, STRVS(rel));
	}

	fs_write(drv->fs, f, STRV("\")\n\n"));

	uint i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		path_t dir = {0};
		path_init_s(&dir, proj_get_str(proj, pkg->strs + PKG_DIR), '/');
		path_push_s(&dir, STRV("pkg.cmake"), '/');
		fs_write(drv->fs, f, STRV("include("));
		fs_write(drv->fs, f, STRVS(dir));
		fs_write(drv->fs, f, STRV(")\n"));

		gen_pkg(proj, drv->fs, i, build_dir);
	}

	fs_close(drv->fs, f);

	return 0;
}

static gen_driver_t cmake = {
	.param = STRVT("C"),
	.desc  = "CMake",
	.gen   = gen_cmake,
};

GEN_DRIVER(cmake, &cmake);
