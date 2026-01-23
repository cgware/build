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

static int gen_tgt(const proj_t *proj, const vars_t *vars, fs_t *fs, void *f, uint pkg_id, const pkg_t *pkg, uint tgt_id,
		   const target_t *target, arr_t *deps, str_t *buf)
{
	strv_t svalues[__VARS_CNT] = {
		[ARCH]	 = STRVT("${ARCH}"),
		[CONFIG] = STRVT("${CONFIG}"),
	};

	fs_write(fs, f, STRV("set(TN \""));
	strv_t name = proj_get_str(proj, target->strs + TARGET_NAME);
	if (name.len > 0) {
		fs_write(fs, f, name);
	}
	fs_write(fs, f, STRV("\")\n"));

	for (int i = 0; i < __VARS_CNT; i++) {
		if (!(vars->vars[i].deps & (1 << TN))) {
			continue;
		}

		strv_t val = vars->vars[i].val;
		switch (i) {
		case TGT_SRC: {
			if (target->type != TARGET_TYPE_EXT) {
				continue;
			}
			val = STRV("${DIR_TMP_EXT_PKG_SRC_ROOT}");
			break;
		}
		case TGT_BUILD: {
			if (target->type != TARGET_TYPE_EXT) {
				continue;
			}
			val = STRV("${DIR_TMP_EXT_PKG_BUILD}");
			break;
		}
		case TGT_OUT: {
			strv_t out = proj_get_str(proj, target->strs + TARGET_OUT);
			if (out.len > 0) {
				resolve_var(vars, out, svalues, buf);
				val = STRVS(*buf);
			} else {
				switch (target->type) {
				case TARGET_TYPE_EXE:
					val = STRV("${DIR_OUT_BIN}");
					break;
				case TARGET_TYPE_LIB:
					val = STRV("${DIR_OUT_LIB}");
					break;
				case TARGET_TYPE_EXT:
					val = STRV("${DIR_OUT_EXT}");
					break;
				case TARGET_TYPE_TST:
					val = STRV("${DIR_OUT_TST}");
					break;
				default:
					val = STRV_NULL;
					break;
				}
			}
			break;
		}
		case TGT_PREP: {
			if (target->type != TARGET_TYPE_EXT) {
				continue;
			}
			strv_t prep = proj_get_str(proj, target->strs + TARGET_PREP);
			resolve_var(vars, prep, svalues, buf);
			val = STRVS(*buf);
			break;
		}
		case TGT_CONF: {
			if (target->type != TARGET_TYPE_EXT) {
				continue;
			}
			strv_t conf = proj_get_str(proj, target->strs + TARGET_CONF);
			resolve_var(vars, conf, svalues, buf);
			val = STRVS(*buf);
			break;
		}
		case TGT_COMP: {
			if (target->type != TARGET_TYPE_EXT) {
				continue;
			}
			strv_t comp = proj_get_str(proj, target->strs + TARGET_COMP);
			resolve_var(vars, comp, svalues, buf);
			val = STRVS(*buf);
			break;
		}
		case TGT_INST: {
			if (target->type != TARGET_TYPE_EXT) {
				continue;
			}
			strv_t inst = proj_get_str(proj, target->strs + TARGET_INST);
			resolve_var(vars, inst, svalues, buf);
			val = STRVS(*buf);
			break;
		}
		case TGT_TGT: {
			if (target->type != TARGET_TYPE_EXT) {
				continue;
			}
			strv_t tgt = proj_get_str(proj, target->strs + TARGET_TGT);
			resolve_var(vars, tgt, svalues, buf);
			val = STRVS(*buf);
			break;
		}
		case DIR_OUT_LIB_FILE:
			if (target->type != TARGET_TYPE_LIB) {
				continue;
			}
			break;
		case DIR_OUT_BIN_FILE:
			if (target->type != TARGET_TYPE_EXE) {
				continue;
			}
			break;
		case DIR_OUT_TST_FILE:
			if (target->type != TARGET_TYPE_TST) {
				continue;
			}
			break;
		case DIR_OUT_EXT_PKG:
		case DIR_OUT_EXT_FILE:
			if (target->type != TARGET_TYPE_EXT) {
				continue;
			}
			break;
		default:       // LCOV_EXCL_LINE
			break; // LCOV_EXCL_LINE
		}

		if (val.data == NULL) {
			continue; // LCOV_EXCL_LINE
		}

		buf->len = 0;
		str_cat(buf, val);

		var_convert(buf, '{', '}', '{', '}');

		fs_write(fs, f, STRV("set("));
		fs_write(fs, f, vars->vars[i].name);
		fs_write(fs, f, STRV(" "));
		if (buf->len > 0) {
			fs_write(fs, f, STRVS(*buf));
		}
		fs_write(fs, f, STRV(")\n"));
	}
	fs_write(fs, f, STRV("\n"));

	strv_t inc = proj_get_str(proj, pkg->strs + PKG_STR_INC);
	strv_t drv = proj_get_str(proj, pkg->strs + PKG_STR_DRV);
	strv_t tst = proj_get_str(proj, pkg->strs + PKG_STR_TST);

	if (target->type != TARGET_TYPE_EXT) {
		fs_write(fs, f, STRV("file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}"));

		path_t tmp = {0};

		switch (target->type) {
		case TARGET_TYPE_TST:
			path_init_s(&tmp, tst, '/');
			break;
		default:
			path_init_s(&tmp, proj_get_str(proj, pkg->strs + PKG_STR_SRC), '/');
			break;
		}

		size_t src_len = tmp.len;
		path_push_s(&tmp, STRV("*.h"), '/');
		fs_write(fs, f, STRVS(tmp));

		fs_write(fs, f, STRV(" ${DIR_PKG}"));
		tmp.len = src_len;
		path_push_s(&tmp, STRV("*.c"), '/');
		fs_write(fs, f, STRVS(tmp));

		if (target->type != TARGET_TYPE_LIB) {
			int lib_drv = 0;

			deps->cnt = 0;
			proj_get_deps(proj, tgt_id, deps);
			if (deps->cnt > 0) {
				uint i = 0;
				const uint *dep;
				arr_foreach(deps, i, dep)
				{
					const target_t *dtarget = list_get(&proj->targets, *dep);
					const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
					strv_t ddriver		= proj_get_str(proj, dpkg->strs + PKG_STR_DRV);
					if (ddriver.len > 0) {
						fs_write(fs, f, STRV(" ${"));
						fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_STR_NAME));
						fs_write(fs, f, STRV("_DRIVERS}"));
						lib_drv |= dtarget->pkg == pkg_id;
					}
				}
			}

			if (!lib_drv && drv.len > 0) {
				tmp.len = 0;
				path_init_s(&tmp, drv, '/');
				path_push_s(&tmp, STRV("*.c"), '/');
				fs_write(fs, f, STRV(" ${DIR_PKG}"));
				fs_write(fs, f, STRVS(tmp));
			}
		}

		fs_write(fs, f, STRV(")\n"));
	}

	strv_t src = proj_get_str(proj, pkg->strs + PKG_STR_SRC);

	switch (target->type) {
	case TARGET_TYPE_EXE: {
		fs_write(fs, f, STRV("add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"));
		if (inc.len > 0 || src.len > 0) {
			fs_write(fs, f, STRV("target_include_directories(${PN}_${TN} PRIVATE"));
			if (inc.len > 0) {
				fs_write(fs, f, STRV(" ${DIR_PKG}"));
				fs_write(fs, f, inc);
			}

			if (src.len > 0) {
				fs_write(fs, f, STRV(" ${DIR_PKG}"));
				fs_write(fs, f, src);
			}

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

		if (target->has_deps) {
			fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} PRIVATE"));
			const list_node_t *dep_target_id;
			list_node_t j = target->deps;
			list_foreach(&proj->deps, j, dep_target_id)
			{
				const target_t *dtarget = list_get(&proj->targets, *dep_target_id);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
				fs_write(fs, f, STRV(" "));
				fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_STR_NAME));
				fs_write(fs, f, STRV("_"));
				fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
			}
			fs_write(fs, f, STRV(")\n"));
		}
		break;
	}
	case TARGET_TYPE_LIB: {
		fs_write(fs, f, STRV("add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"));
		if (inc.len > 0 || src.len > 0) {
			fs_write(fs, f, STRV("target_include_directories(${PN}_${TN}"));
			if (inc.len > 0) {
				fs_write(fs, f, STRV(" PUBLIC ${DIR_PKG}"));
				fs_write(fs, f, inc);
			}

			if (src.len > 0) {
				fs_write(fs, f, STRV(" PRIVATE ${DIR_PKG}"));
				fs_write(fs, f, src);
			}
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

		if (target->has_deps) {
			fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} PUBLIC"));
			const list_node_t *dep_target_id;
			list_node_t j = target->deps;
			list_foreach(&proj->deps, j, dep_target_id)
			{
				const target_t *dtarget = list_get(&proj->targets, *dep_target_id);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
				fs_write(fs, f, STRV(" "));
				fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_STR_NAME));
				fs_write(fs, f, STRV("_"));
				fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
			}
			fs_write(fs, f, STRV(")\n"));
		}
		break;
	}
	case TARGET_TYPE_EXT: {
		fs_write(fs,
			 f,
			 STRV("string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
			      "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
			      "file(MAKE_DIRECTORY \"${DIR_TMP_DL}\" \"${TGT_BUILD_DEBUG}\" "
			      "\"${TGT_BUILD_RELEASE}\")\n"));

		strv_t uri = proj_get_str(proj, pkg->strs + PKG_STR_URI);
		if (uri.len > 0) {
			fs_write(fs,
				 f,
				 STRV("if(NOT EXISTS ${DIR_TMP_DL}${PKG_URI_FILE})\n"
				      "\tfile(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL}${PKG_URI_FILE}\n"
				      "\t\tSHOW_PROGRESS\n"
				      ")\n"
				      "endif()\n"
				      "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL}${PKG_URI_FILE}\" DESTINATION "
				      "\"${DIR_TMP_EXT_PKG_SRC}\")\n"));
		}

		fs_write(fs,
			 f,
			 STRV("add_custom_target(${PN}_${TN}_build\n"
			      "\tALL\n"
			      "\tCOMMAND ${TGT_PREP}\n"
			      "\tCOMMAND ${TGT_CONF}\n"
			      "\tCOMMAND ${TGT_COMP}\n"
			      "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
			      "\tCOMMAND ${TGT_INST}\n"
			      "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
			      ")\n"));

		if (target->has_deps) {
			int header = 0;
			const list_node_t *dep_target_id;
			list_node_t j = target->deps;
			list_foreach(&proj->deps, j, dep_target_id)
			{
				const target_t *dtarget = list_get(&proj->targets, *dep_target_id);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
				if (dtarget->type == TARGET_TYPE_LIB ||
				    (dtarget->type == TARGET_TYPE_EXT && dtarget->out_type == TARGET_TGT_TYPE_LIB)) {
					continue;
				}

				if (header == 0) {
					fs_write(fs, f, STRV("add_dependencies(${PN}_${TN}_build"));
					header = 1;
				}
				fs_write(fs, f, STRV(" "));
				fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_STR_NAME));
				fs_write(fs, f, STRV("_"));
				fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
			}
			if (header) {
				fs_write(fs, f, STRV(")\n"));
			}
		}

		switch (target->out_type) {
		case TARGET_TGT_TYPE_LIB:
			fs_write(fs, f, STRV("add_library(${PN}_${TN} STATIC IMPORTED)\n"));
			break;
		case TARGET_TGT_TYPE_EXE:
			fs_write(fs, f, STRV("add_executable(${PN}_${TN} IMPORTED)\n"));
			break;
		default:
			break;
		}

		fs_write(fs, f, STRV("add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"));

		if (target->has_deps) {
			int header = 0;
			const list_node_t *dep_target_id;
			list_node_t j = target->deps;
			list_foreach(&proj->deps, j, dep_target_id)
			{
				const target_t *dtarget = list_get(&proj->targets, *dep_target_id);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
				if (dtarget->type == TARGET_TYPE_LIB ||
				    (dtarget->type == TARGET_TYPE_EXT && dtarget->out_type == TARGET_TGT_TYPE_LIB)) {
					if (header == 0) {
						fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} INTERFACE"));
						header = 1;
					}
					fs_write(fs, f, STRV(" "));
					fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_STR_NAME));
					fs_write(fs, f, STRV("_"));
					fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
				}
			}
			if (header) {
				fs_write(fs, f, STRV(")\n"));
			}
		}

		break;
	}
	case TARGET_TYPE_TST: {
		fs_write(fs, f, STRV("add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"));

		if (src.len > 0 || tst.len > 0) {
			fs_write(fs, f, STRV("target_include_directories(${PN}_${TN} PRIVATE"));
			if (src.len > 0) {
				fs_write(fs, f, STRV(" ${DIR_PKG}"));
				fs_write(fs, f, src);
			}
			if (tst.len > 0) {
				fs_write(fs, f, STRV(" ${DIR_PKG}"));
				fs_write(fs, f, tst);
			}
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

		if (target->has_deps) {
			fs_write(fs, f, STRV("target_link_libraries(${PN}_${TN} PRIVATE"));
			const list_node_t *dep_target_id;
			list_node_t j = target->deps;
			list_foreach(&proj->deps, j, dep_target_id)
			{
				const target_t *dtarget = list_get(&proj->targets, *dep_target_id);
				const pkg_t *dpkg	= proj_get_pkg(proj, dtarget->pkg);
				fs_write(fs, f, STRV(" "));
				fs_write(fs, f, proj_get_str(proj, dpkg->strs + PKG_STR_NAME));
				fs_write(fs, f, STRV("_"));
				fs_write(fs, f, proj_get_str(proj, dtarget->strs + TARGET_NAME));
			}
			fs_write(fs, f, STRV(")\n"));
		}

		fs_write(fs,
			 f,
			 STRV("add_test(\n"
			      "\tNAME ${PN}_${TN}\n"
			      "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
			      ")\n"));
		break;
	}
	default:
		break;
	}

	strv_t values[__VARS_CNT] = {0};

	if (target->type == TARGET_TYPE_EXT) {
		fs_write(fs,
			 f,
			 STRV("string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
			      "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE "
			      "\"${DIR_OUT_EXT_FILE}\")\n"));
	}

	fs_write(fs, f, STRV("set_target_properties(${PN}_${TN} PROPERTIES\n"));

	if (target->type == TARGET_TYPE_EXT) {
		fs_write(fs,
			 f,
			 STRV("\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
			      "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
			      "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"));

		if (target->out_type == TARGET_TGT_TYPE_LIB && inc.len > 0) {
			fs_write(fs, f, STRV("\tINTERFACE_INCLUDE_DIRECTORIES ${DIR_TMP_EXT_PKG_SRC_ROOT}"));
			fs_write(fs, f, inc);
			fs_write(fs, f, STRV("\n"));
		}
	}

	fs_write(fs, f, STRV("\tOUTPUT_NAME \"${PN}\"\n"));

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
		resolve_dir(proj, vars, values, target->type, buf, &outdir);
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

	return 0;
}

static int gen_pkg(const proj_t *proj, const vars_t *vars, fs_t *fs, uint id, arr_t *deps, strv_t build_dir)
{
	const pkg_t *pkg = proj_get_pkg(proj, id);

	path_t path = {0};
	path_t tmp  = {0};

	path_init_s(&path, build_dir, '/');
	get_path(proj, pkg->strs + PKG_STR_PATH, &tmp);
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
	strv_t name = proj_get_str(proj, pkg->strs + PKG_STR_NAME);
	if (name.len > 0) {
		fs_write(fs, f, name);
	}
	fs_write(fs, f, STRV("\")\n"));

	fs_write(fs, f, STRV("set(${PN}_DIR \""));
	if (tmp.len > 0) {
		fs_write(fs, f, STRVS(tmp));
	}
	fs_write(fs, f, STRV("\")\n"));

	strv_t uri = proj_get_str(proj, pkg->strs + PKG_STR_URI);
	if (uri.len > 0) {
		fs_write(fs, f, STRV("set(${PN}_URI "));
		fs_write(fs, f, uri);
		fs_write(fs, f, STRV(")\n"));

		strv_t uri_file = proj_get_str(proj, pkg->strs + PKG_STR_URI_FILE);
		fs_write(fs, f, STRV("set(${PN}_URI_FILE "));
		fs_write(fs, f, uri_file);
		fs_write(fs, f, STRV(")\n"));

		strv_t uri_name = proj_get_str(proj, pkg->strs + PKG_STR_URI_NAME);
		strv_t uri_ver	= proj_get_str(proj, pkg->strs + PKG_STR_URI_VER);
		fs_write(fs, f, STRV("set(${PN}_URI_NAME "));
		fs_write(fs, f, uri_name);
		if (uri_ver.len > 0) {
			fs_write(fs, f, STRV("-"));
			fs_write(fs, f, uri_ver);
		}
		fs_write(fs, f, STRV(")\n"));

		get_path(proj, pkg->strs + PKG_STR_URI_DIR, &tmp);
		if (tmp.len > 0) {
			fs_write(fs, f, STRV("set(${PN}_URI_ROOT "));
			fs_write(fs, f, STRVS(tmp));
			fs_write(fs, f, STRV(")\n"));
		}
	}

	strv_t drv = proj_get_str(proj, pkg->strs + PKG_STR_DRV);

	for (int i = 0; i < __VARS_CNT; i++) {
		strv_t val = vars->vars[i].val;
		switch (i) {
		case PKG_URI:
		case PKG_URI_FILE:
		case PKG_URI_NAME:
		case PKG_URI_ROOT:
		case DIR_TMP_EXT_PKG:
		case DIR_TMP_EXT_PKG_SRC:
		case DIR_TMP_EXT_PKG_SRC_ROOT:
		case DIR_TMP_EXT_PKG_BUILD:
			if (uri.len == 0) {
				continue;
			}
			break;
		case DIR_PKG_DRV:
		case DIR_PKG_DRV_C:
		case DIR_OUT_DRV_PKG:
			if (drv.len == 0) {
				continue;
			}
			break;
		case DIR_PKG_SRC:
		case DIR_PKG_INC:
		case DIR_PKG_TST:
		case DIR_OUT_INT_SRC:
		case DIR_OUT_INT_TST:
			continue;
		case PN_DRIVERS:
			if (drv.len == 0) {
				continue;
			}
			fs_write(fs, f, STRV("set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"));
			continue;
		default:
			if ((vars->vars[i].deps & ((1 << PN) | (1 << TN))) != (1 << PN)) {
				continue;
			}
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
		fs_write(fs, f, STRV(" "));
		if (buf.len > 0) {
			fs_write(fs, f, STRVS(buf));
		}
		fs_write(fs, f, STRV(")\n"));
	}

	fs_write(fs, f, STRV("\n"));

	if (pkg->has_targets) {
		const target_t *target;
		list_node_t i = pkg->targets;
		list_foreach(&proj->targets, i, target)
		{
			gen_tgt(proj, vars, fs, f, id, pkg, i, target, deps, &buf);
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

	int ret = 0;

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

	fs_write(drv->fs, f, STRV("option(OPEN \"Open HTML coverage report\" ON)\n\n"));

	fs_write(drv->fs,
		 f,
		 STRV("set(ARCHS \"host\" CACHE STRING \"List of architectures to build\")\n"
		      "list(LENGTH ARCHS _arch_count)\n"
		      "set(CONFIGS \"Debug\" CACHE STRING \"List of configurations to build\")\n"
		      "list(LENGTH CONFIGS _config_count)\n"
		      "get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)\n"
		      "\n"
		      "if(WIN32)\n"
		      "\tset(EXT_LIB .lib)\n"
		      "\tset(EXT_EXE .exe)\n"
		      "else()\n"
		      "\tset(EXT_LIB .a)\n"
		      "\tset(EXT_EXE )\n"
		      "endif()\n"
		      "\n"));

	path_t tmp = {0};
	str_t buf  = strz(16);

	for (int i = 0; i < __VARS_CNT; i++) {
		if (vars.vars[i].deps & ((1 << ARCH) | (1 << CONFIG) | (1 << PN) | (1 << TN))) {
			continue;
		}

		strv_t val = vars.vars[i].val;
		switch (i) {
		case CONFIG: {
			fs_write(drv->fs, f, STRV("if(is_multi_config)\n"));
			fs_write(drv->fs, f, STRV("\tset("));
			fs_write(drv->fs, f, vars.vars[i].name);
			fs_write(drv->fs, f, STRV(" $<CONFIG>)\n"));
			fs_write(drv->fs, f, STRV("else()\n"));
			fs_write(drv->fs, f, STRV("\tset("));
			fs_write(drv->fs, f, vars.vars[i].name);
			fs_write(drv->fs, f, STRV(" ${CMAKE_BUILD_TYPE})\n"));
			fs_write(drv->fs, f, STRV("endif()\n"));
			break;
		}
		case CP: {
			val = STRV("${CMAKE_COMMAND} -E copy");
			break;
		}
		case DIR_PROJ: {
			path_calc_rel_s(build_dir, proj_dir, '/', &tmp);
			buf.len = 0;
			str_cat(&buf, STRV("${CMAKE_SOURCE_DIR}/"));
			str_cat(&buf, STRVS(tmp));
			val = STRVS(buf);
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
		fs_write(drv->fs, f, STRV(" "));
		if (buf.len > 0) {
			fs_write(drv->fs, f, STRVS(buf));
		}
		fs_write(drv->fs, f, STRV(")\n"));
	}

	fs_write(drv->fs,
		 f,
		 STRV("\n"
		      "enable_testing()\n"
		      "\n"
		      "if(_arch_count GREATER 0 AND _config_count GREATER 0)\n"
		      "\tinclude(ExternalProject)\n"
		      "\tforeach(arch IN LISTS ARCHS)\n"
		      "\t\tif (CMAKE_GENERATOR MATCHES \"Visual Studio 17 2022\")\n"
		      "\t\t\tif(arch STREQUAL \"x86\")\n"
		      "\t\t\t\tset(ARGS_ARCH \"-A Win32\")\n"
		      "\t\t\telseif(arch STREQUAL \"host\")\n"
		      "\t\t\t\tset(ARGS_ARCH \"\")\n"
		      "\t\t\telse()\n"
		      "\t\t\t\tset(ARGS_ARCH \"-A ${arch}\")\n"
		      "\t\t\tendif()\n"
		      "\t\telse()\n"
		      "\t\t\tset(ARGS_ARCH \"-DARCH=${arch}\")\n"
		      "\t\tendif()\n"
		      "\n"
		      "\t\tif(is_multi_config)\n"
		      "\t\t\tExternalProject_Add(${arch}\n"
		      "\t\t\t\tSOURCE_DIR ${CMAKE_SOURCE_DIR}\n"
		      "\t\t\t\tBINARY_DIR ${CMAKE_BINARY_DIR}/${arch}\n"
		      "\t\t\t\tINSTALL_COMMAND \"\"\n"
		      "\t\t\t\tCMAKE_ARGS -DARCHS= -DCONFIGS= ${ARGS_ARCH}\n"
		      "\t\t\t)\n"
		      "\t\t\tadd_test(\n"
		      "\t\t\t\tNAME ${arch}\n"
		      "\t\t\t\tCOMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR}/${arch} -C $<CONFIG>\n"
		      "\t\t\t)\n"
		      "\t\telse()\n"
		      "\t\t\tforeach(conf IN LISTS CONFIGS)\n"
		      "\t\t\t\tExternalProject_Add(${arch}-${conf}\n"
		      "\t\t\t\t\tSOURCE_DIR ${CMAKE_SOURCE_DIR}\n"
		      "\t\t\t\t\tBINARY_DIR ${CMAKE_BINARY_DIR}/${arch}-${conf}\n"
		      "\t\t\t\t\tINSTALL_COMMAND \"\"\n"
		      "\t\t\t\t\tCMAKE_ARGS -DARCHS= -DCONFIGS= ${ARGS_ARCH} -DCMAKE_BUILD_TYPE=${conf}\n"
		      "\t\t\t\t)\n"
		      "\t\t\t\tadd_test(\n"
		      "\t\t\t\t\tNAME ${arch}-${conf}\n"
		      "\t\t\t\t\tCOMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR}/${arch}-${conf}\n"
		      "\t\t\t\t)\n"
		      "\t\t\tendforeach()\n"
		      "\t\tendif()\n"
		      "\tendforeach()\n"
		      "else()\n"
		      "\tif (CMAKE_GENERATOR MATCHES \"Visual Studio 17 2022\")\n"
		      "\t\tif(CMAKE_GENERATOR_PLATFORM STREQUAL \"Win32\")\n"
		      "\t\t\tset(ARCH \"x86\")\n"
		      "\t\t\tset(ARGS_ARCH \"-A Win32\")\n"
		      "\t\telseif(CMAKE_GENERATOR_PLATFORM STREQUAL \"\")\n"
		      "\t\t\tset(ARCH \"host\")\n"
		      "\t\telse()\n"
		      "\t\t\tset(ARCH \"${CMAKE_GENERATOR_PLATFORM}\")\n"
		      "\t\t\tset(ARGS_ARCH \"-A ${CMAKE_GENERATOR_PLATFORM}\")\n"
		      "\t\tendif()\n"
		      "\tendif()\n"
		      "\n"));

	for (int i = 0; i < __VARS_CNT; i++) {
		if (vars.vars[i].deps & ((1 << PN) | (1 << TN))) {
			continue;
		}

		if (!(vars.vars[i].deps & ((1 << ARCH) | (1 << CONFIG)))) {
			continue;
		}

		strv_t val = vars.vars[i].val;
		switch (i) {
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
			continue; // LCOV_EXCL_LINE
		}

		buf.len = 0;
		str_cat(&buf, val);

		var_convert(&buf, '{', '}', '{', '}');

		fs_write(drv->fs, f, STRV("\tset("));
		fs_write(drv->fs, f, vars.vars[i].name);
		fs_write(drv->fs, f, STRV(" "));
		if (buf.len > 0) {
			fs_write(drv->fs, f, STRVS(buf));
		}
		fs_write(drv->fs, f, STRV(")\n"));
	}

	fs_write(drv->fs,
		 f,
		 STRV("\n"
		      "\tif(CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		      "\t\tif(ARCH STREQUAL \"x64\")\n"
		      "\t\t\tset(CMAKE_C_FLAGS \"-m64\")\n"
		      "\t\telseif(ARCH STREQUAL \"x86\")\n"
		      "\t\t\tset(CMAKE_C_FLAGS \"-m32\")\n"
		      "\t\tendif()\n"
		      "\tendif()\n"));

	if (proj->pkgs.cnt > 0) {
		arr_t order = {0};
		arr_init(&order, proj->pkgs.cnt, sizeof(uint), ALLOC_STD);
		ret |= proj_get_pkg_build_order(proj, &order, ALLOC_STD);

		arr_t deps = {0};
		arr_init(&deps, 1, sizeof(uint), ALLOC_STD);

		uint i = 0;
		const uint *id;
		arr_foreach(&order, i, id)
		{
			const pkg_t *pkg = proj_get_pkg(proj, *id);

			path_t dir = {0};
			get_path(proj, pkg->strs + PKG_STR_PATH, &dir);
			path_push_s(&dir, STRV("pkg.cmake"), '/');
			fs_write(drv->fs, f, STRV("\n\tinclude("));
			fs_write(drv->fs, f, STRVS(dir));
			fs_write(drv->fs, f, STRV(")\n"));

			gen_pkg(proj, &vars, drv->fs, i, &deps, build_dir);
		}

		arr_free(&deps);
		arr_free(&order);
	}

	fs_write(drv->fs,
		 f,
		 STRV("endif()\n"
		      "\n"
		      "if(CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		      "\tadd_custom_target(cov\n"
		      "\t\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_TMP_COV}\n"
		      "\t\tCOMMAND if [ -n \\\"$$\\(find ${CMAKE_BINARY_DIR} -name *.gcda\\)\\\" ]\\; then \n"
		      "\t\t\tlcov -q -c -o ${DIR_TMP_COV}lcov.info -d ${CMAKE_BINARY_DIR} --exclude \\\"*/test/*\\\" --exclude "
		      "\\\"*/tmp/*\\\"\\;\n"
		      "\t\t\tgenhtml -q -o ${DIR_TMP_COV} ${DIR_TMP_COV}lcov.info\\;\n"
		      "\t\t\t[ \\\"${OPEN}\\\" = \\\"1\\\" ] && open ${DIR_TMP_COV}index.html || true\\;\n"
		      "\t\tfi\n"
		      "\t\tWORKING_DIRECTORY ${CMAKE_BINARY_DIR}\n"
		      "\t)\n"
		      "endif()\n"));

	fs_close(drv->fs, f);

	str_free(&buf);

	return ret;
}

static gen_driver_t cmake = {
	.param = STRVT("C"),
	.desc  = "CMake",
	.gen   = gen_cmake,
};

GEN_DRIVER(cmake, &cmake);
