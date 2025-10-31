#include "t_gen_common.h"

#include "log.h"
#include "mem.h"
#include "path.h"
#include "test.h"

TEST(gen_cmake_null)
{
	START;

	gen_driver_t drv = *gen_find_param(STRV("C"));

	EXPECT_EQ(drv.gen(&drv, NULL, STRV_NULL, STRV_NULL), 1);

	END;
}

TEST(gen_cmake_proj_build_dir)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_build_dir(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("tmp/build/CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project( LANGUAGES C)\n"
		    "\n"
		    "enable_testing()\n"
		    "\n"
		    "option(OPEN \"Open HTML coverage report\" ON)\n"
		    "\n"
		    "set(CONFIGS \"Debug;Release\" CACHE STRING \"List of build configurations\")\n"
		    "list(LENGTH CONFIGS _config_count)\n"
		    "get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)\n"
		    "\n"
		    "if(is_multi_config)\n"
		    "\tset(CONFIG \"$<CONFIG>\")\n"
		    "else()\n"
		    "\tset(CONFIG \"${CMAKE_BUILD_TYPE}\")\n"
		    "endif()\n"
		    "set(DIR_PROJ \"${CMAKE_SOURCE_DIR}/../../\")\n"
		    "set(DIR_BUILD \"\")\n"
		    "set(DIR_TMP \"${DIR_PROJ}tmp/\")\n"
		    "set(DIR_TMP_EXT \"${DIR_TMP}ext/\")\n"
		    "set(DIR_TMP_REP \"${DIR_TMP}report/\")\n"
		    "set(DIR_TMP_COV \"${DIR_TMP_REP}cov/\")\n"
		    "set(DIR_TMP_DL \"${DIR_TMP}dl/\")\n"
		    "set(DIR_OUT \"${DIR_PROJ}bin/${ARCH}-${CONFIG}/\")\n"
		    "set(DIR_OUT_INT \"${CMAKE_BINARY_DIR}\")\n"
		    "set(DIR_OUT_LIB \"${DIR_OUT}lib/\")\n"
		    "set(DIR_OUT_DRV \"${DIR_OUT}drivers/\")\n"
		    "set(DIR_OUT_BIN \"${DIR_OUT}bin/\")\n"
		    "set(DIR_OUT_EXT \"${DIR_OUT}ext/\")\n"
		    "set(DIR_OUT_TST \"${DIR_OUT}test/\")\n"
		    "if(WIN32)\n"
		    "\tset(EXT_LIB \".lib\")\n"
		    "\tset(EXT_EXE \".exe\")\n"
		    "else()\n"
		    "\tset(EXT_LIB \".a\")\n"
		    "\tset(EXT_EXE \"\")\n"
		    "endif()\n"
		    "if(_config_count GREATER 0 AND NOT is_multi_config)\n"
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
		    "set(TEST_DEPENDS \"\")\n"
		    "endif()\n"
		    "\n"
		    "if(WIN32)\n"
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
		    "endif()\n"
		    "\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_proj_empty)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_empty(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project( LANGUAGES C)\n"
		    "\n"
		    "enable_testing()\n"
		    "\n"
		    "option(OPEN \"Open HTML coverage report\" ON)\n"
		    "\n"
		    "set(CONFIGS \"Debug;Release\" CACHE STRING \"List of build configurations\")\n"
		    "list(LENGTH CONFIGS _config_count)\n"
		    "get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)\n"
		    "\n"
		    "if(is_multi_config)\n"
		    "\tset(CONFIG \"$<CONFIG>\")\n"
		    "else()\n"
		    "\tset(CONFIG \"${CMAKE_BUILD_TYPE}\")\n"
		    "endif()\n"
		    "set(DIR_PROJ \"${CMAKE_SOURCE_DIR}/\")\n"
		    "set(DIR_BUILD \"\")\n"
		    "set(DIR_TMP \"${DIR_PROJ}tmp/\")\n"
		    "set(DIR_TMP_EXT \"${DIR_TMP}ext/\")\n"
		    "set(DIR_TMP_REP \"${DIR_TMP}report/\")\n"
		    "set(DIR_TMP_COV \"${DIR_TMP_REP}cov/\")\n"
		    "set(DIR_TMP_DL \"${DIR_TMP}dl/\")\n"
		    "set(DIR_OUT \"${DIR_PROJ}bin/${ARCH}-${CONFIG}/\")\n"
		    "set(DIR_OUT_INT \"${CMAKE_BINARY_DIR}\")\n"
		    "set(DIR_OUT_LIB \"${DIR_OUT}lib/\")\n"
		    "set(DIR_OUT_DRV \"${DIR_OUT}drivers/\")\n"
		    "set(DIR_OUT_BIN \"${DIR_OUT}bin/\")\n"
		    "set(DIR_OUT_EXT \"${DIR_OUT}ext/\")\n"
		    "set(DIR_OUT_TST \"${DIR_OUT}test/\")\n"
		    "if(WIN32)\n"
		    "\tset(EXT_LIB \".lib\")\n"
		    "\tset(EXT_EXE \".exe\")\n"
		    "else()\n"
		    "\tset(EXT_LIB \".a\")\n"
		    "\tset(EXT_EXE \"\")\n"
		    "endif()\n"
		    "if(_config_count GREATER 0 AND NOT is_multi_config)\n"
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
		    "set(TEST_DEPENDS \"\")\n"
		    "endif()\n"
		    "\n"
		    "if(WIN32)\n"
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
		    "endif()\n"
		    "\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_proj_name)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_name(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project(proj LANGUAGES C)\n"
		    "\n"
		    "enable_testing()\n"
		    "\n"
		    "option(OPEN \"Open HTML coverage report\" ON)\n"
		    "\n"
		    "set(CONFIGS \"Debug;Release\" CACHE STRING \"List of build configurations\")\n"
		    "list(LENGTH CONFIGS _config_count)\n"
		    "get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)\n"
		    "\n"
		    "if(is_multi_config)\n"
		    "\tset(CONFIG \"$<CONFIG>\")\n"
		    "else()\n"
		    "\tset(CONFIG \"${CMAKE_BUILD_TYPE}\")\n"
		    "endif()\n"
		    "set(DIR_PROJ \"${CMAKE_SOURCE_DIR}/\")\n"
		    "set(DIR_BUILD \"\")\n"
		    "set(DIR_TMP \"${DIR_PROJ}tmp/\")\n"
		    "set(DIR_TMP_EXT \"${DIR_TMP}ext/\")\n"
		    "set(DIR_TMP_REP \"${DIR_TMP}report/\")\n"
		    "set(DIR_TMP_COV \"${DIR_TMP_REP}cov/\")\n"
		    "set(DIR_TMP_DL \"${DIR_TMP}dl/\")\n"
		    "set(DIR_OUT \"${DIR_PROJ}bin/${ARCH}-${CONFIG}/\")\n"
		    "set(DIR_OUT_INT \"${CMAKE_BINARY_DIR}\")\n"
		    "set(DIR_OUT_LIB \"${DIR_OUT}lib/\")\n"
		    "set(DIR_OUT_DRV \"${DIR_OUT}drivers/\")\n"
		    "set(DIR_OUT_BIN \"${DIR_OUT}bin/\")\n"
		    "set(DIR_OUT_EXT \"${DIR_OUT}ext/\")\n"
		    "set(DIR_OUT_TST \"${DIR_OUT}test/\")\n"
		    "if(WIN32)\n"
		    "\tset(EXT_LIB \".lib\")\n"
		    "\tset(EXT_EXE \".exe\")\n"
		    "else()\n"
		    "\tset(EXT_LIB \".a\")\n"
		    "\tset(EXT_EXE \"\")\n"
		    "endif()\n"
		    "if(_config_count GREATER 0 AND NOT is_multi_config)\n"
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
		    "set(TEST_DEPENDS \"\")\n"
		    "endif()\n"
		    "\n"
		    "if(WIN32)\n"
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
		    "endif()\n"
		    "\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_proj_unknown)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_unknown(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project( LANGUAGES C)\n"
		    "\n"
		    "enable_testing()\n"
		    "\n"
		    "option(OPEN \"Open HTML coverage report\" ON)\n"
		    "\n"
		    "set(CONFIGS \"Debug;Release\" CACHE STRING \"List of build configurations\")\n"
		    "list(LENGTH CONFIGS _config_count)\n"
		    "get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)\n"
		    "\n"
		    "if(is_multi_config)\n"
		    "\tset(CONFIG \"$<CONFIG>\")\n"
		    "else()\n"
		    "\tset(CONFIG \"${CMAKE_BUILD_TYPE}\")\n"
		    "endif()\n"
		    "set(DIR_PROJ \"${CMAKE_SOURCE_DIR}/\")\n"
		    "set(DIR_BUILD \"\")\n"
		    "set(DIR_TMP \"${DIR_PROJ}tmp/\")\n"
		    "set(DIR_TMP_EXT \"${DIR_TMP}ext/\")\n"
		    "set(DIR_TMP_REP \"${DIR_TMP}report/\")\n"
		    "set(DIR_TMP_COV \"${DIR_TMP_REP}cov/\")\n"
		    "set(DIR_TMP_DL \"${DIR_TMP}dl/\")\n"
		    "set(DIR_OUT \"${DIR_PROJ}bin/${ARCH}-${CONFIG}/\")\n"
		    "set(DIR_OUT_INT \"${CMAKE_BINARY_DIR}\")\n"
		    "set(DIR_OUT_LIB \"${DIR_OUT}lib/\")\n"
		    "set(DIR_OUT_DRV \"${DIR_OUT}drivers/\")\n"
		    "set(DIR_OUT_BIN \"${DIR_OUT}bin/\")\n"
		    "set(DIR_OUT_EXT \"${DIR_OUT}ext/\")\n"
		    "set(DIR_OUT_TST \"${DIR_OUT}test/\")\n"
		    "if(WIN32)\n"
		    "\tset(EXT_LIB \".lib\")\n"
		    "\tset(EXT_EXE \".exe\")\n"
		    "else()\n"
		    "\tset(EXT_LIB \".a\")\n"
		    "\tset(EXT_EXE \"\")\n"
		    "endif()\n"
		    "if(_config_count GREATER 0 AND NOT is_multi_config)\n"
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
		    "set(TEST_DEPENDS \"\")\n"
		    "include(pkg.cmake)\n"
		    "endif()\n"
		    "\n"
		    "if(WIN32)\n"
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
		    "endif()\n"
		    "\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_proj_exe)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_exe(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_proj_lib)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_lib(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_proj_ext)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_ext(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(${PN}_URI url)\n"
		    "set(${PN}_DLFILE )\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(PKG_URI \"${${PN}_URI}\")\n"
		    "set(PKG_DLFILE \"${${PN}_DLFILE}\")\n"
		    "set(PKG_DLROOT \"${${PN}_DLROOT}\")\n"
		    "set(DIR_TMP_EXT_PKG \"${DIR_TMP_EXT}${PKG_DIR}\")\n"
		    "set(DIR_TMP_EXT_PKG_ROOT \"${DIR_TMP_EXT_PKG}${PKG_DLROOT}\")\n"
		    "set(DIR_TMP_DL_PKG \"${DIR_TMP_DL}${PKG_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "set(DIR_OUT_EXT_PKG \"${DIR_OUT_EXT}${PN}/\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(${PN}_${TN}_CMD )\n"
		    "set(TGT_CMD \"${${PN}_${TN}_CMD}\")\n"
		    "set(TGT_OUT \"${${PN}_${TN}_OUT}\")\n"
		    "set(DIR_TMP_EXT_PKG_ROOT_OUT \"${DIR_TMP_EXT_PKG_ROOT}${TGT_OUT}\")\n"
		    "set(DIR_OUT_EXT_FILE \"${DIR_OUT_EXT_PKG}${TN}\")\n"
		    "\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_DLFILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_EXT_PKG}\")\n"
		    "add_custom_target(${PN}_${TN} ALL\n"
		    "\tCOMMAND ${TGT_CMD}\n"
		    "\tWORKING_DIRECTORY ${DIR_TMP_EXT_PKG_ROOT}\n"
		    ")\n"
		    "if(CMAKE_GENERATOR MATCHES \"Visual Studio\")\n"
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
		    "endif()\n"
		    "add_custom_command(TARGET ${PN}_${TN} POST_BUILD\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E copy ${DIR_TMP_EXT_PKG_ROOT_OUT} ${DIR_OUT_EXT_PKG}\n"
		    ")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_proj_test)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_proj_test(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE)\n"
		    "add_test(${PN}_${TN}_build ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CMAKE_BUILD_TYPE} --target "
		    "${PN}_${TN})\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_TST_FILE_DEBUG \"${DIR_OUT_TST_FILE}\")\n"
		    "add_test(${PN} ${DIR_OUT_TST_FILE_DEBUG})\n"
		    "set_tests_properties(${PN} PROPERTIES\n"
		    "\tDEPENDS ${PN}_${TN}_build\n"
		    "\tWORKING_DIRECTORY ${CMAKE_SOURCE_DIR}\n"
		    ")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE test/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_exe)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/bin/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_exe_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe_drv(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_PKG_DRV \"${DIR_PKG}drivers/\")\n"
		    "set(DIR_PKG_DRV_C \"${DIR_PKG_DRV}*.c\")\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_DRV_PKG \"${DIR_OUT_DRV}${PN}/\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c ${DIR_PKG}drivers/*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/bin/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_exe_drv_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe_drv_inc(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_PKG_INC \"${DIR_PKG}include/\")\n"
		    "set(DIR_PKG_DRV \"${DIR_PKG}drivers/\")\n"
		    "set(DIR_PKG_DRV_C \"${DIR_PKG_DRV}*.c\")\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_DRV_PKG \"${DIR_OUT_DRV}${PN}/\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c ${DIR_PKG}drivers/*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PRIVATE ${DIR_PKG}include)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/bin/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_lib)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_lib_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_inc(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_PKG_INC \"${DIR_PKG}include/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PUBLIC ${DIR_PKG}include)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_lib_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_drv(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_PKG_DRV \"${DIR_PKG}drivers/\")\n"
		    "set(DIR_PKG_DRV_C \"${DIR_PKG_DRV}*.c\")\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_DRV_PKG \"${DIR_OUT_DRV}${PN}/\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_lib_drv_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_drv_inc(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_PKG_INC \"${DIR_PKG}include/\")\n"
		    "set(DIR_PKG_DRV \"${DIR_PKG}drivers/\")\n"
		    "set(DIR_PKG_DRV_C \"${DIR_PKG_DRV}*.c\")\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_DRV_PKG \"${DIR_OUT_DRV}${PN}/\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PUBLIC ${DIR_PKG}include)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_lib_test)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"lib\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_PKG_TST \"${DIR_PKG}test/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_INT_TST \"${DIR_OUT_INT}${PN}/test/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "set(DIR_OUT_TST_FILE \"${DIR_OUT_TST}${PN}\")\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}test/*.h ${DIR_PKG}test/*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE lib_lib)\n"
		    "add_test(${PN}_${TN}_build ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CMAKE_BUILD_TYPE} --target "
		    "${PN}_${TN})\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_TST_FILE_DEBUG \"${DIR_OUT_TST_FILE}\")\n"
		    "add_test(${PN} ${DIR_OUT_TST_FILE_DEBUG})\n"
		    "set_tests_properties(${PN} PROPERTIES\n"
		    "\tDEPENDS ${PN}_${TN}_build\n"
		    "\tWORKING_DIRECTORY ${CMAKE_SOURCE_DIR}\n"
		    ")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/test/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_lib_test_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_inc(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"lib\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_PKG_INC \"${DIR_PKG}include/\")\n"
		    "set(DIR_PKG_TST \"${DIR_PKG}test/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_INT_TST \"${DIR_OUT_INT}${PN}/test/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "set(DIR_OUT_TST_FILE \"${DIR_OUT_TST}${PN}\")\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PUBLIC ${DIR_PKG}include)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}test/*.h ${DIR_PKG}test/*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PRIVATE ${DIR_PKG}include)\n" // FIXME: not needed
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE lib_lib)\n"
		    "add_test(${PN}_${TN}_build ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CMAKE_BUILD_TYPE} --target "
		    "${PN}_${TN})\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_TST_FILE_DEBUG \"${DIR_OUT_TST_FILE}\")\n"
		    "add_test(${PN} ${DIR_OUT_TST_FILE_DEBUG})\n"
		    "set_tests_properties(${PN} PROPERTIES\n"
		    "\tDEPENDS ${PN}_${TN}_build\n"
		    "\tWORKING_DIRECTORY ${CMAKE_SOURCE_DIR}\n"
		    ")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/test/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_lib_test_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_drv(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"lib\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_PKG_DRV \"${DIR_PKG}drivers/\")\n"
		    "set(DIR_PKG_DRV_C \"${DIR_PKG_DRV}*.c\")\n"
		    "set(DIR_PKG_TST \"${DIR_PKG}test/\")\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_INT_TST \"${DIR_OUT_INT}${PN}/test/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_DRV_PKG \"${DIR_OUT_DRV}${PN}/\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "set(DIR_OUT_TST_FILE \"${DIR_OUT_TST}${PN}\")\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}test/*.h ${DIR_PKG}test/*.c ${DIR_PKG}drivers/*.c ${lib_DRIVERS})\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE lib_lib)\n"
		    "add_test(${PN}_${TN}_build ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CMAKE_BUILD_TYPE} --target "
		    "${PN}_${TN})\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_TST_FILE_DEBUG \"${DIR_OUT_TST_FILE}\")\n"
		    "add_test(${PN} ${DIR_OUT_TST_FILE_DEBUG})\n"
		    "set_tests_properties(${PN} PROPERTIES\n"
		    "\tDEPENDS ${PN}_${TN}_build\n"
		    "\tWORKING_DIRECTORY ${CMAKE_SOURCE_DIR}\n"
		    ")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/test/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_lib_test_drv_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_drv_inc(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"lib\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_PKG_INC \"${DIR_PKG}include/\")\n"
		    "set(DIR_PKG_DRV \"${DIR_PKG}drivers/\")\n"
		    "set(DIR_PKG_DRV_C \"${DIR_PKG_DRV}*.c\")\n"
		    "set(DIR_PKG_TST \"${DIR_PKG}test/\")\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_INT_TST \"${DIR_OUT_INT}${PN}/test/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_DRV_PKG \"${DIR_OUT_DRV}${PN}/\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "set(DIR_OUT_TST_FILE \"${DIR_OUT_TST}${PN}\")\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PUBLIC ${DIR_PKG}include)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}test/*.h ${DIR_PKG}test/*.c ${DIR_PKG}drivers/*.c ${lib_DRIVERS})\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PRIVATE ${DIR_PKG}include)\n" // FIXME: not needed
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE lib_lib)\n"
		    "add_test(${PN}_${TN}_build ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CMAKE_BUILD_TYPE} --target "
		    "${PN}_${TN})\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_TST_FILE_DEBUG \"${DIR_OUT_TST_FILE}\")\n"
		    "add_test(${PN} ${DIR_OUT_TST_FILE_DEBUG})\n"
		    "set_tests_properties(${PN} PROPERTIES\n"
		    "\tDEPENDS ${PN}_${TN}_build\n"
		    "\tWORKING_DIRECTORY ${CMAKE_SOURCE_DIR}\n"
		    ")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY ${DIR_PROJ}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${DIR_PROJ}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${DIR_PROJ}bin/${ARCH}-Release/test/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_multi)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_multi(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("./a/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"a\")\n"
		    "set(${PN}_DIR \"a/\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"a\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./b/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"b\")\n"
		    "set(${PN}_DIR \"b/\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"b\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_depends)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_depends(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("./lib/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"lib\")\n"
		    "set(${PN}_DIR \"lib/\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"exe\")\n"
		    "set(${PN}_DIR \"exe/\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"exe\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE lib_lib)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_rdepends)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_rdepends(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("./base/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"base\")\n"
		    "set(${PN}_DIR \"base/\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"base\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib1/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"lib1\")\n"
		    "set(${PN}_DIR \"lib1/\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"lib1\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC base_base)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./lib2/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"lib2\")\n"
		    "set(${PN}_DIR \"lib2/\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"lib2\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PUBLIC base_base)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	fs_read(&com.fs, STRV("./exe/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"exe\")\n"
		    "set(${PN}_DIR \"exe/\")\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "\n"
		    "set(TN \"exe\")\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE lib1_lib1 lib2_lib2)\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_unknown)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_unknown(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(${PN}_URI url)\n"
		    "set(${PN}_DLFILE archive)\n"
		    "set(${PN}_DLROOT main/)\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(PKG_URI \"${${PN}_URI}\")\n"
		    "set(PKG_DLFILE \"${${PN}_DLFILE}\")\n"
		    "set(PKG_DLROOT \"${${PN}_DLROOT}\")\n"
		    "set(DIR_TMP_EXT_PKG \"${DIR_TMP_EXT}${PKG_DIR}\")\n"
		    "set(DIR_TMP_EXT_PKG_ROOT \"${DIR_TMP_EXT_PKG}${PKG_DLROOT}\")\n"
		    "set(DIR_TMP_DL_PKG \"${DIR_TMP_DL}${PKG_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "set(DIR_OUT_EXT_PKG \"${DIR_OUT_EXT}${PN}/\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(${PN}_${TN}_CMD cmd)\n"
		    "set(${PN}_${TN}_OUT out)\n"
		    "set(TGT_CMD \"${${PN}_${TN}_CMD}\")\n"
		    "set(TGT_OUT \"${${PN}_${TN}_OUT}\")\n"
		    "set(DIR_TMP_EXT_PKG_ROOT_OUT \"${DIR_TMP_EXT_PKG_ROOT}${TGT_OUT}\")\n"
		    "set(DIR_OUT_EXT_FILE \"${DIR_OUT_EXT_PKG}${TN}\")\n"
		    "\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_DLFILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_EXT_PKG}\")\n"
		    "add_custom_target(${PN}_${TN} ALL\n"
		    "\tCOMMAND ${TGT_CMD}\n"
		    "\tWORKING_DIRECTORY ${DIR_TMP_EXT_PKG_ROOT}\n"
		    ")\n"
		    "if(CMAKE_GENERATOR MATCHES \"Visual Studio\")\n"
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
		    "endif()\n"
		    "add_custom_command(TARGET ${PN}_${TN} POST_BUILD\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E copy ${DIR_TMP_EXT_PKG_ROOT_OUT} ${DIR_OUT_EXT_PKG}\n"
		    ")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_zip)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_zip(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(${PN}_URI url)\n"
		    "set(${PN}_DLFILE archive.zip)\n"
		    "set(${PN}_DLROOT main/)\n"
		    "set(PKG_DIR \"${${PN}_DIR}\")\n"
		    "set(PKG_URI \"${${PN}_URI}\")\n"
		    "set(PKG_DLFILE \"${${PN}_DLFILE}\")\n"
		    "set(PKG_DLROOT \"${${PN}_DLROOT}\")\n"
		    "set(DIR_TMP_EXT_PKG \"${DIR_TMP_EXT}${PKG_DIR}\")\n"
		    "set(DIR_TMP_EXT_PKG_ROOT \"${DIR_TMP_EXT_PKG}${PKG_DLROOT}\")\n"
		    "set(DIR_TMP_DL_PKG \"${DIR_TMP_DL}${PKG_DIR}\")\n"
		    "set(DIR_PKG \"${DIR_PROJ}${PKG_DIR}\")\n"
		    "set(DIR_PKG_SRC \"${DIR_PKG}src/\")\n"
		    "set(DIR_OUT_INT_SRC \"${DIR_OUT_INT}${PN}/src/\")\n"
		    "set(DIR_OUT_LIB_FILE \"${DIR_OUT_LIB}${PN}.a\")\n"
		    "set(DIR_OUT_BIN_FILE \"${DIR_OUT_BIN}${PN}\")\n"
		    "set(DIR_OUT_EXT_PKG \"${DIR_OUT_EXT}${PN}/\")\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(${PN}_${TN}_CMD cmd)\n"
		    "set(${PN}_${TN}_OUT out)\n"
		    "set(TGT_CMD \"${${PN}_${TN}_CMD}\")\n"
		    "set(TGT_OUT \"${${PN}_${TN}_OUT}\")\n"
		    "set(DIR_TMP_EXT_PKG_ROOT_OUT \"${DIR_TMP_EXT_PKG_ROOT}${TGT_OUT}\")\n"
		    "set(DIR_OUT_EXT_FILE \"${DIR_OUT_EXT_PKG}${TN}\")\n"
		    "\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_DLFILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_EXT_PKG}\")\n"
		    "add_custom_target(${PN}_${TN} ALL\n"
		    "\tCOMMAND ${TGT_CMD}\n"
		    "\tWORKING_DIRECTORY ${DIR_TMP_EXT_PKG_ROOT}\n"
		    ")\n"
		    "if(CMAKE_GENERATOR MATCHES \"Visual Studio\")\n"
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
		    "endif()\n"
		    "add_custom_command(TARGET ${PN}_${TN} POST_BUILD\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E copy ${DIR_TMP_EXT_PKG_ROOT_OUT} ${DIR_OUT_EXT_PKG}\n"
		    ")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

STEST(gen_cmake)
{
	SSTART;

	RUN(gen_cmake_null);
	RUN(gen_cmake_proj_build_dir);
	RUN(gen_cmake_proj_empty);
	RUN(gen_cmake_proj_name);
	RUN(gen_cmake_proj_unknown);
	RUN(gen_cmake_proj_exe);
	RUN(gen_cmake_proj_lib);
	RUN(gen_cmake_proj_ext);
	RUN(gen_cmake_proj_test);
	RUN(gen_cmake_pkg_exe);
	RUN(gen_cmake_pkg_exe_drv);
	RUN(gen_cmake_pkg_exe_drv_inc);
	RUN(gen_cmake_pkg_lib);
	RUN(gen_cmake_pkg_lib_inc);
	RUN(gen_cmake_pkg_lib_drv);
	RUN(gen_cmake_pkg_lib_drv_inc);
	RUN(gen_cmake_pkg_lib_test);
	RUN(gen_cmake_pkg_lib_test_inc);
	RUN(gen_cmake_pkg_lib_test_drv);
	RUN(gen_cmake_pkg_lib_test_drv_inc);
	RUN(gen_cmake_pkg_multi);
	RUN(gen_cmake_pkg_depends);
	RUN(gen_cmake_pkg_rdepends);
	RUN(gen_cmake_pkg_ext_unknown);
	RUN(gen_cmake_pkg_ext_zip);

	SEND;
}
