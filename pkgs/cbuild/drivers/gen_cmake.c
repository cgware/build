#include "gen.h"

#include "log.h"
#include "path.h"
#include "var.h"

static void resolve_var(strv_t var, const strv_t *values, str_t *buf)
{
	buf->len = 0;
	str_cat(buf, var);
	var_replace(buf, values);
}

static void resolve_dir(const proj_t *proj, strv_t *values, target_type_t type, str_t *buf, path_t *resolved)
{
	buf->len = 0;
	str_cat(buf, proj_get_str(proj, proj->outdir));
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
	strv_t pkg_dir = proj_get_str(proj, pkg->strs + PKG_PATH);
	if (pkg_dir.len > 0) {
		fs_mkpath(fs, STRVS(path), pkg_dir);
	}
	path_push(&path, proj_get_str(proj, pkg->strs + PKG_PATH));
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
	strv_t dir = proj_get_str(proj, pkg->strs + PKG_PATH);
	if (dir.len > 0) {
		fs_write(fs, f, dir);
		fs_write(fs, f, STRV(SEP));
	}
	fs_write(fs, f, STRV("\")\n"));

	strv_t svalues[__VAR_CNT] = {
		[VAR_ARCH]   = STRVT("${ARCH}"),
		[VAR_CONFIG] = STRVT("${CONFIG}"),
	};

	if (pkg->has_targets) {
		const target_t *target;
		list_node_t target_id = pkg->targets;
		list_foreach(&proj->targets, target_id, target)
		{
			fs_write(fs, f, STRV("set(TN \""));
			strv_t name = proj_get_str(proj, target->strs + TARGET_NAME);
			if (name.len > 0) {
				fs_write(fs, f, name);
			}
			fs_write(fs, f, STRV("\")\n"));

			if (target->type != TARGET_TYPE_EXT) {
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
			}

			switch (target->type) {
			case TARGET_TYPE_EXE: {
				fs_write(fs, f, STRV("add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"));
				fs_write(fs,
					 f,
					 STRV("if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
					      "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
					      "\t\t$<$<CONFIG:Debug>:--coverage>\n"
					      "\t)\n"
					      "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
					      "\t\t$<$<CONFIG:Debug>:--coverage>\n"
					      "\t)\n"
					      "endif()\n"));
				fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} PRIVATE"));
				if (target->has_deps) {
					const list_node_t *dep_target_id;
					list_node_t j = target->deps;
					list_foreach(&proj->deps, j, dep_target_id)
					{
						const target_t *dtarget = list_get(&proj->targets, *dep_target_id);
						const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
						fs_write(fs, f, STRV(" "));
						fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_NAME));
						fs_write(fs, f, STRV("_"));
						fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
					}
				}
				fs_write(fs, f, STRV(")\n"));
				break;
			}
			case TARGET_TYPE_LIB: {
				fs_write(fs, f, STRV("add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"));
				strv_t inc = proj_get_str(proj, pkg->strs + PKG_INC);
				if (inc.len > 0) {
					fs_write(fs, f, STRV("target_include_directories(${PN}_${TN} PUBLIC ${PROJDIR}${PKGDIR}"));
					fs_write(fs, f, inc);
					fs_write(fs, f, STRV(")\n"));
				}
				fs_write(fs,
					 f,
					 STRV("if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
					      "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
					      "\t\t$<$<CONFIG:Debug>:--coverage>\n"
					      "\t)\n"
					      "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
					      "\t\t$<$<CONFIG:Debug>:--coverage>\n"
					      "\t)\n"
					      "endif()\n"));
				fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} PUBLIC"));
				if (target->has_deps) {
					const list_node_t *dep_target_id;
					list_node_t j = target->deps;
					list_foreach(&proj->deps, j, dep_target_id)
					{
						const target_t *dtarget = list_get(&proj->targets, *dep_target_id);
						const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
						fs_write(fs, f, STRV(" "));
						fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_NAME));
						fs_write(fs, f, STRV("_"));
						fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
					}
				}
				fs_write(fs, f, STRV(")\n"));
				break;
			}
			case TARGET_TYPE_EXT: {
				strv_t uri = proj_get_str(proj, pkg->strs + PKG_URI_STR);

				fs_write(fs, f, STRV("set(URL "));
				fs_write(fs, f, uri);
				fs_write(fs, f, STRV(")\n"));

				fs_write(fs, f, STRV("set(ZIP_FILE ${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/dl/main.zip)\n"));
				fs_write(fs, f, STRV("set(EXT_DIR ${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/ext/)\n"));

				strv_t uri_dir = proj_get_str(proj, pkg->strs + PKG_URI_DIR);
				if (uri_dir.len > 0) {
					fs_write(fs, f, STRV("set(URI_ROOT "));
					fs_write(fs, f, uri_dir);
					fs_write(fs, f, STRV(")\n"));
				}

				strv_t out = proj_get_str(proj, target->strs + TARGET_OUT);
				resolve_var(out, svalues, &buf);
				if (out.len > 0) {
					fs_write(fs, f, STRV("set(OUT "));
					fs_write(fs, f, STRVS(buf));
					fs_write(fs, f, STRV(")\n"));
				}

				strv_t cmd = proj_get_str(proj, target->strs + TARGET_CMD);
				resolve_var(cmd, svalues, &buf);
				fs_write(fs, f, STRV("set(CMD "));
				fs_write(fs, f, STRVS(buf));
				fs_write(fs, f, STRV(")\n"));

				fs_write(fs,
					 f,
					 STRV("add_custom_target(${PN}_${TN} ALL\n"
					      "\tCOMMAND ${CMD}\n"
					      "\tWORKING_DIRECTORY ${EXT_DIR}${URI_ROOT}\n"
					      ")\n"));

				fs_write(fs,
					 f,
					 STRV("file(DOWNLOAD ${URL} ${ZIP_FILE}\n"
					      "\tSHOW_PROGRESS\n"
					      ")\n"));

				fs_write(fs, f, STRV("file(MAKE_DIRECTORY \"${EXT_DIR}\")\n"));

				fs_write(fs,
					 f,
					 STRV("execute_process(\n"
					      "\tCOMMAND ${CMAKE_COMMAND} -E tar xzf ${ZIP_FILE}\n"
					      "\tWORKING_DIRECTORY ${EXT_DIR}\n"
					      ")\n"));

				fs_write(fs,
					 f,
					 STRV("add_custom_command(TARGET ${PN}_${TN} POST_BUILD\n"
					      "\tCOMMAND ${CMAKE_COMMAND} -E make_directory "
					      "${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-${CONFIG}/ext/${PN}\n"
					      "\tCOMMAND ${CMAKE_COMMAND} -E copy ${EXT_DIR}${URI_ROOT}${OUT} "
					      "${CMAKE_SOURCE_DIR}/${PROJDIR}/bin/${ARCH}-${CONFIG}/ext/${PN}/\n"
					      ")\n"));
				break;
			}
			case TARGET_TYPE_TST: {
				fs_write(fs, f, STRV("add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"));
				fs_write(fs,
					 f,
					 STRV("if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
					      "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
					      "\t\t$<$<CONFIG:Debug>:--coverage>\n"
					      "\t)\n"
					      "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
					      "\t\t$<$<CONFIG:Debug>:--coverage>\n"
					      "\t)\n"
					      "endif()\n"));
				fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} PRIVATE"));
				if (target->has_deps) {
					const list_node_t *dep_target_id;
					list_node_t j = target->deps;
					list_foreach(&proj->deps, j, dep_target_id)
					{
						const target_t *dtarget = list_get(&proj->targets, *dep_target_id);
						const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
						fs_write(fs, f, STRV(" "));
						fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_NAME));
						fs_write(fs, f, STRV("_"));
						fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
					}
				}
				fs_write(fs, f, STRV(")\n"));
				fs_write(fs,
					 f,
					 STRV("add_test(${PN}_${TN}_build ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config "
					      "${CONFIG} --target ${PN}_${TN})\n"
					      "add_test(${PN} ${CMAKE_SOURCE_DIR}/${PROJDIR}/bin/${ARCH}-${CONFIG}/test/${PN})\n"
					      "set_tests_properties(${PN} PROPERTIES\n"
					      "\tDEPENDS ${PN}_${TN}_build\n"
					      "\tWORKING_DIRECTORY ${CMAKE_SOURCE_DIR}\n"
					      ")\n"));
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
	strv_t proj_name = proj_get_str(proj, proj->name);
	if (proj_name.len > 0) {
		fs_write(drv->fs, f, proj_name);
	}
	fs_write(drv->fs, f, STRV(" LANGUAGES C)\n\n"));

	fs_write(drv->fs, f, STRV("enable_testing()\n\n"));

	fs_write(drv->fs, f, STRV("option(OPEN \"Open HTML coverage report\" ON)\n"));

	fs_write(drv->fs, f, STRV("set(PROJDIR \""));

	path_t rel = {0};
	path_calc_rel_s(build_dir, proj_dir, '/', &rel);
	if (rel.len > 0) {
		fs_write(drv->fs, f, STRVS(rel));
	}

	fs_write(drv->fs, f, STRV("\")\n"));

	fs_write(drv->fs, f, STRV("set(CONFIG \"${CMAKE_BUILD_TYPE}\")\n\n"));

	strv_t values[__VAR_CNT] = {
		[VAR_ARCH]   = STRVT("${ARCH}"),
		[VAR_CONFIG] = STRVT("${CONFIG}"),
	};

	strv_t poutdir = proj_get_str(proj, proj->outdir);
	str_t outdir   = strn(poutdir.data, poutdir.len, 256);
	if (var_replace(&outdir, values)) {
		// return 1;
	}

	str_t buf = strz(16);
	if (pathv_is_rel(STRVS(outdir))) {
		str_cat(&buf, STRV("${PROJDIR}"));
	}
	str_cat(&buf, STRVN(outdir.data, outdir.len));

	fs_write(drv->fs, f, STRV("set(OUTDIR \""));
	fs_write(drv->fs, f, STRVS(buf));
	fs_write(drv->fs, f, STRV("\")\n\n"));

	fs_write(drv->fs, f, STRV("set(INTDIR \"${CMAKE_BINARY_DIR}\")\n\n"));

	fs_write(drv->fs, f, STRV("set(REPDIR \"${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/report/\")\n"));
	fs_write(drv->fs, f, STRV("set(COVDIR \"${REPDIR}cov/\")\n\n"));

	fs_write(drv->fs,
		 f,
		 STRV("add_custom_target(cov\n"
		      "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${COVDIR}\n"
		      "\tCOMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CONFIG} --target test\n"
		      "\tCOMMAND if [ -n \\\"$$\\(find ${CMAKE_BINARY_DIR} -name *.gcda\\)\\\" ]\\; then \n"
		      "\t\tlcov -q -c -o ${COVDIR}lcov.info -d ${INTDIR}\\;\n"
		      "\t\tgenhtml -q -o ${COVDIR} ${COVDIR}lcov.info\\;\n"
		      "\t\t[ \\\"${OPEN}\\\" = \\\"1\\\" ] && open ${COVDIR}index.html || true\\;\n"
		      "\tfi\n"
		      "\tWORKING_DIRECTORY ${CMAKE_BINARY_DIR}\n"
		      ")\n\n"));

	int types[__TARGET_TYPE_MAX] = {0};

	list_node_t i = 0;
	const target_t *target;
	list_foreach_all(&proj->targets, i, target)
	{
		types[target->type] = 1;
	}

	(void)types;

	i = 0;
	const pkg_t *pkg;
	arr_foreach(&proj->pkgs, i, pkg)
	{
		path_t dir = {0};
		path_init_s(&dir, proj_get_str(proj, pkg->strs + PKG_PATH), '/');
		path_push_s(&dir, STRV("pkg.cmake"), '/');
		fs_write(drv->fs, f, STRV("include("));
		fs_write(drv->fs, f, STRVS(dir));
		fs_write(drv->fs, f, STRV(")\n"));

		gen_pkg(proj, drv->fs, i, build_dir);
	}

	fs_close(drv->fs, f);

	str_free(&outdir);
	str_free(&buf);

	return 0;
}

static gen_driver_t cmake = {
	.param = STRVT("C"),
	.desc  = "CMake",
	.gen   = gen_cmake,
};

GEN_DRIVER(cmake, &cmake);
