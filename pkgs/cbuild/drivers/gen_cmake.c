#include "gen.h"

#include "log.h"
#include "path.h"
#include "vars.h"

static void resolve_var(const vars_t *vars, strv_t var, const strv_t *values, str_t *buf)
{
	buf->len = 0;
	str_cat(buf, var);
	vars_replace(vars, buf, values);
}

static void resolve_dir(const proj_t *proj, const vars_t *vars, strv_t *values, target_type_t type, str_t *buf, path_t *resolved)
{
	buf->len = 0;
	str_cat(buf, proj_get_str(proj, proj->outdir));
	vars_replace(vars, buf, values);
	path_t tmp = {0};
	path_init(&tmp, STRVS(*buf));

	buf->len = 0;
	if (pathv_is_rel(STRVS(tmp))) {
		str_cat(buf, STRV("${DIR_PROJ}"));
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

static void get_path(const proj_t *proj, uint id, path_t *path)
{
	path_init_s(path, proj_get_str(proj, id), '/');
	if (path->len > 0) {
		path_push_s(path, STRV(""), '/');
	}
}

static int gen_pkg(const proj_t *proj, const vars_t *vars, fs_t *fs, uint id, strv_t build_dir)
{
	const pkg_t *pkg = proj_get_pkg(proj, id);

	path_t path = {0};
	path_t tmp  = {0};

	path_init_s(&path, build_dir, '/');
	get_path(proj, pkg->strs + PKG_PATH, &tmp);
	if (tmp.len > 0) {
		fs_mkpath(fs, STRVS(path), STRVS(tmp));
	}
	path_push(&path, STRVS(tmp));
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

	fs_write(fs, f, STRV("set(${PN}_DIR \""));
	if (tmp.len > 0) {
		fs_write(fs, f, STRVS(tmp));
	}
	fs_write(fs, f, STRV("\")\n"));

	strv_t uri = proj_get_str(proj, pkg->strs + PKG_URI_STR);
	if (uri.len > 0) {
		fs_write(fs, f, STRV("set(${PN}_URI "));
		fs_write(fs, f, uri);
		fs_write(fs, f, STRV(")\n"));

		strv_t uri_file = proj_get_str(proj, pkg->strs + PKG_URI_NAME);
		fs_write(fs, f, STRV("set(${PN}_DLFILE "));
		if (uri_file.len > 0) {
			fs_write(fs, f, uri_file);
			switch (pkg->uri.ext) {
			case PKG_URI_EXT_ZIP:
				fs_write(fs, f, STRV(".zip"));
				break;
			default:
				break;
			}
		}
		fs_write(fs, f, STRV(")\n"));

		get_path(proj, pkg->strs + PKG_URI_DIR, &tmp);
		if (tmp.len > 0) {
			fs_write(fs, f, STRV("set(${PN}_DLROOT "));
			fs_write(fs, f, STRVS(tmp));
			fs_write(fs, f, STRV(")\n"));
		}
	}

	for (int i = 0; i < __VARS_CNT; i++) {
		if ((vars->vars[i].deps & ((1 << PN) | (1 << TN))) != (1 << PN)) {
			continue;
		}

		strv_t val = vars->vars[i].val;
		switch (i) {
		default:
			break;
		}

		if (val.data == NULL) {
			continue; // LCOV_EXCL_LINE
		}

		buf.len = 0;
		str_cat(&buf, val);

		var_convert(&buf, '{', '}', '{', '}');

		fs_write(fs, f, STRV("set("));
		fs_write(fs, f, vars->vars[i].name);
		fs_write(fs, f, STRV(" \""));
		if (buf.len > 0) {
			fs_write(fs, f, STRVS(buf));
		}
		fs_write(fs, f, STRV("\")\n"));
	}

	fs_write(fs, f, STRV("\n"));

	strv_t svalues[__VARS_CNT] = {
		[ARCH]	 = STRVT("${ARCH}"),
		[CONFIG] = STRVT("${CONFIG}"),
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

			switch (target->type) {
			case TARGET_TYPE_EXT: {
				strv_t cmd = proj_get_str(proj, target->strs + TARGET_CMD);
				resolve_var(vars, cmd, svalues, &buf);
				fs_write(fs, f, STRV("set(${PN}_${TN}_CMD "));
				fs_write(fs, f, STRVS(buf));
				fs_write(fs, f, STRV(")\n"));

				strv_t out = proj_get_str(proj, target->strs + TARGET_OUT);
				resolve_var(vars, out, svalues, &buf);
				if (out.len > 0) {
					fs_write(fs, f, STRV("set(${PN}_${TN}_OUT "));
					fs_write(fs, f, STRVS(buf));
					fs_write(fs, f, STRV(")\n"));
				}

				break;
			}
			default:
				break;
			}

			for (int i = 0; i < __VARS_CNT; i++) {
				if (!(vars->vars[i].deps & (1 << TN))) {
					continue;
				}

				strv_t val = vars->vars[i].val;
				switch (i) {
				default:
					break;
				}

				if (val.data == NULL) {
					continue; // LCOV_EXCL_LINE
				}

				buf.len = 0;
				str_cat(&buf, val);

				var_convert(&buf, '{', '}', '{', '}');

				fs_write(fs, f, STRV("set("));
				fs_write(fs, f, vars->vars[i].name);
				fs_write(fs, f, STRV(" \""));
				if (buf.len > 0) {
					fs_write(fs, f, STRVS(buf));
				}
				fs_write(fs, f, STRV("\")\n"));
			}
			fs_write(fs, f, STRV("\n"));

			if (target->type != TARGET_TYPE_EXT) {
				fs_write(fs, f, STRV("file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}"));

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

				fs_write(fs, f, STRV(" ${DIR_PKG}"));
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
					fs_write(fs, f, STRV("target_include_directories(${PN}_${TN} PUBLIC ${DIR_PKG}"));
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
				fs_write(fs, f, STRV("file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\")\n"));
				fs_write(fs,
					 f,
					 STRV("file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_DLFILE}\n"
					      "\tSHOW_PROGRESS\n"
					      ")\n"));

				fs_write(fs, f, STRV("file(MAKE_DIRECTORY \"${DIR_TMP_EXT_PKG}\")\n"));

				fs_write(fs,
					 f,
					 STRV("add_custom_target(${PN}_${TN} ALL\n"
					      "\tCOMMAND ${TGT_CMD}\n"
					      "\tWORKING_DIRECTORY ${DIR_TMP_EXT_PKG_ROOT}\n"
					      ")\n"));

				fs_write(fs,
					 f,
					 STRV("if(CMAKE_GENERATOR MATCHES \"Visual Studio\")\n"
					      "\tadd_custom_command(TARGET ${PN}_${TN} PRE_BUILD\n"
					      "\t\tCOMMAND ${CMAKE_COMMAND} -E tar xzf ${DIR_TMP_DL_PKG}${PKG_DLFILE}\n"
					      "\t\tWORKING_DIRECTORY ${DIR_TMP_EXT_PKG}\n"
					      "\t\tDEPENDS ${DIR_TMP_DL_PKG}${PKG_DLFILE}\n"
					      "\t)\n"
					      "else()\n"
					      "\texecute_process(\n"
					      "\t\tCOMMAND ${CMAKE_COMMAND} -E tar xzf ${DIR_TMP_DL_PKG}${PKG_DLFILE}\n"
					      "\t\tWORKING_DIRECTORY ${DIR_TMP_EXT_PKG}\n"
					      "\t)\n"
					      "endif()\n"));

				fs_write(fs,
					 f,
					 STRV("add_custom_command(TARGET ${PN}_${TN} POST_BUILD\n"
					      "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
					      "\tCOMMAND ${CMAKE_COMMAND} -E copy ${DIR_TMP_EXT_PKG_ROOT_OUT} ${DIR_OUT_EXT_PKG}\n"
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
					      "${CMAKE_BUILD_TYPE} --target ${PN}_${TN})\n"
					      "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_TST_FILE_DEBUG \"${DIR_OUT_TST_FILE}\")\n"
					      "add_test(${PN} ${DIR_OUT_TST_FILE_DEBUG})\n"
					      "set_tests_properties(${PN} PROPERTIES\n"
					      "\tDEPENDS ${PN}_${TN}_build\n"
					      "\tWORKING_DIRECTORY ${CMAKE_SOURCE_DIR}\n"
					      ")\n"));
				break;
			}
			default:
				break;
			}

			strv_t values[__VARS_CNT] = {0};

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
				values[CONFIG] = props[target->type][i].config;
				resolve_dir(proj, vars, values, target->type, &buf, &outdir);
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

	vars_t vars = {0};
	vars_init(&vars);

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

	fs_write(drv->fs, f, STRV("option(OPEN \"Open HTML coverage report\" ON)\n\n"));

	fs_write(drv->fs, f, STRV("set(CONFIGS \"Debug;Release\" CACHE STRING \"List of build configurations\")\n"));
	fs_write(drv->fs, f, STRV("list(LENGTH CONFIGS _config_count)\n"));
	fs_write(drv->fs, f, STRV("get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)\n\n"));

	path_t tmp = {0};
	str_t buf  = strz(16);

	for (int i = 0; i < __VARS_CNT; i++) {
		if (vars.vars[i].deps & ((1 << PN) | (1 << TN))) {
			continue;
		}

		strv_t val = vars.vars[i].val;
		switch (i) {
		case CONFIG: {
			fs_write(drv->fs, f, STRV("if(is_multi_config)\n"));
			fs_write(drv->fs, f, STRV("\tset("));
			fs_write(drv->fs, f, vars.vars[i].name);
			fs_write(drv->fs, f, STRV(" \"$<CONFIG>\")\n"));
			fs_write(drv->fs, f, STRV("else()\n"));
			fs_write(drv->fs, f, STRV("\tset("));
			fs_write(drv->fs, f, vars.vars[i].name);
			fs_write(drv->fs, f, STRV(" \"${CMAKE_BUILD_TYPE}\")\n"));
			fs_write(drv->fs, f, STRV("endif()\n"));
			continue;
		}
		case DIR_PROJ: {
			path_calc_rel_s(build_dir, proj_dir, '/', &tmp);
			buf.len = 0;
			str_cat(&buf, STRV("${CMAKE_SOURCE_DIR}/"));
			str_cat(&buf, STRVS(tmp));
			val = STRVS(buf);
			break;
		}
		case DIR_OUT: {
			strv_t poutdir = proj_get_str(proj, proj->outdir);
			if (poutdir.len == 0) {
				break;
			}

			buf.len = 0;
			if (pathv_is_rel(STRVS(poutdir))) {
				str_cat(&buf, STRV("${DIR_PROJ}"));
			}
			str_cat(&buf, STRVS(poutdir));
			val = STRVS(buf);
			break;
		}
		case DIR_OUT_INT: {
			val = STRV("${CMAKE_BINARY_DIR}");
			break;
		}
		default:
			break;
		}

		if (val.data == NULL) {
			continue;
		}

		buf.len = 0;
		str_cat(&buf, val);

		var_convert(&buf, '{', '}', '{', '}');

		fs_write(drv->fs, f, STRV("set("));
		fs_write(drv->fs, f, vars.vars[i].name);
		fs_write(drv->fs, f, STRV(" \""));
		if (buf.len > 0) {
			fs_write(drv->fs, f, STRVS(buf));
		}
		fs_write(drv->fs, f, STRV("\")\n"));
	}

	fs_write(drv->fs,
		 f,
		 STRV("if(WIN32)\n"
		      "\tset(EXT_LIB \".lib\")\n"
		      "\tset(EXT_EXE \".exe\")\n"
		      "else()\n"
		      "\tset(EXT_LIB \".a\")\n"
		      "\tset(EXT_EXE \"\")\n"
		      "endif()\n"));

	fs_write(drv->fs,
		 f,
		 STRV("if(_config_count GREATER 0 AND NOT is_multi_config)\n"
		      "set(TEST_DIR \"${CMAKE_BINARY_DIR}/Debug\")\n"
		      "set(TEST_DEPENDS \"Debug\")\n"
		      "include(ExternalProject)\n"
		      "foreach(cfg IN LISTS CONFIGS)\n"
		      "\tExternalProject_Add(${cfg}\n"
		      "\t\tSOURCE_DIR ${CMAKE_SOURCE_DIR}\n"
		      "\t\tBINARY_DIR ${CMAKE_BINARY_DIR}/${cfg}\n"
		      "\t\tINSTALL_COMMAND \"\"\n"
		      "\t\tCMAKE_ARGS\n"
		      "\t\t\t-DARCH=${ARCH}\n"
		      "\t\t\t-DCMAKE_BUILD_TYPE=${cfg}\n"
		      "\t\t\t-DCONFIGS=\n"
		      "\t)\n"
		      "endforeach()\n"
		      "else()\n"
		      "set(TEST_DIR \"${CMAKE_BINARY_DIR}\")\n"
		      "set(TEST_DEPENDS \"\")\n"));

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
		get_path(proj, pkg->strs + PKG_PATH, &dir);
		path_push_s(&dir, STRV("pkg.cmake"), '/');
		fs_write(drv->fs, f, STRV("include("));
		fs_write(drv->fs, f, STRVS(dir));
		fs_write(drv->fs, f, STRV(")\n"));

		gen_pkg(proj, &vars, drv->fs, i, build_dir);
	}

	fs_write(drv->fs, f, STRV("endif()\n\n"));

	fs_write(drv->fs,
		 f,
		 STRV("if(WIN32)\n"
		      "\tadd_custom_target(cov\n"
		      "\t\tCOMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${TEST_DIR}\n"
		      "\t\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_TMP_COV}\n"
		      "\t\tCOMMAND if exist \"${CMAKE_BINARY_DIR}\\\\*.gcda\" (\n"
		      "\t\t\tlcov -q -c -o \"${DIR_TMP_COV}lcov.info\" -d \"${DIR_OUT_INT}\"\n"
		      "\t\t\tgenhtml -q -o \"${DIR_TMP_COV}\" \"${DIR_TMP_COV}lcov.info\"\n"
		      "\t\t\t\"if \\\"${OPEN}\\\"==\\\"1\\\" start \\\"\\\" \\\"${DIR_TMP_COV}index.html\\\"\"\n"
		      "\t\t)\n"
		      "\t\tDEPENDS ${TEST_DEPENDS}\n"
		      "\t\tWORKING_DIRECTORY ${CMAKE_BINARY_DIR}\n"
		      "\t)\n"
		      "else()\n"
		      "\tadd_custom_target(cov\n"
		      "\t\tCOMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${TEST_DIR}\n"
		      "\t\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_TMP_COV}\n"
		      "\t\tCOMMAND if [ -n \\\"$$\\(find ${CMAKE_BINARY_DIR} -name *.gcda\\)\\\" ]\\; then \n"
		      "\t\t\tlcov -q -c -o ${DIR_TMP_COV}lcov.info -d ${DIR_OUT_INT}\\;\n"
		      "\t\t\tgenhtml -q -o ${DIR_TMP_COV} ${DIR_TMP_COV}lcov.info\\;\n"
		      "\t\t\t[ \\\"${OPEN}\\\" = \\\"1\\\" ] && open ${DIR_TMP_COV}index.html || true\\;\n"
		      "\t\tfi\n"
		      "\t\tDEPENDS ${TEST_DEPENDS}\n"
		      "\t\tWORKING_DIRECTORY ${CMAKE_BINARY_DIR}\n"
		      "\t)\n"
		      "endif()\n\n"));

	fs_close(drv->fs, f);

	str_free(&buf);

	return 0;
}

static gen_driver_t cmake = {
	.param = STRVT("C"),
	.desc  = "CMake",
	.gen   = gen_cmake,
};

GEN_DRIVER(cmake, &cmake);
