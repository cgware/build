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
		    "option(OPEN \"Open HTML coverage report\" ON)\n"
		    "\n"
		    "set(ARCHS \"host\" CACHE STRING \"List of architectures to build\")\n"
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
		    "\n"
		    "if(is_multi_config)\n"
		    "\tset(CONFIG $<CONFIG>)\n"
		    "else()\n"
		    "\tset(CONFIG ${CMAKE_BUILD_TYPE})\n"
		    "endif()\n"
		    "set(CP ${CMAKE_COMMAND} -E copy)\n"
		    "set(DIR_PROJ ${CMAKE_SOURCE_DIR}/../../)\n"
		    "set(DIR_BUILD )\n"
		    "set(DIR_TMP ${DIR_PROJ}tmp/)\n"
		    "set(DIR_TMP_EXT ${DIR_TMP}ext/)\n"
		    "set(DIR_TMP_REP ${DIR_TMP}report/)\n"
		    "set(DIR_TMP_COV ${DIR_TMP_REP}cov/)\n"
		    "set(DIR_TMP_DL ${DIR_TMP}dl/)\n"
		    "\n"
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
		    "\n"
		    "\tset(DIR_OUT ${DIR_PROJ}bin/${ARCH}-${CONFIG}/)\n"
		    "\tset(DIR_OUT_INT ${CMAKE_BINARY_DIR})\n"
		    "\tset(DIR_OUT_LIB ${DIR_OUT}lib/)\n"
		    "\tset(DIR_OUT_DRV ${DIR_OUT}drivers/)\n"
		    "\tset(DIR_OUT_BIN ${DIR_OUT}bin/)\n"
		    "\tset(DIR_OUT_TST ${DIR_OUT}test/)\n"
		    "\n"
		    "\tif(CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\t\tif(ARCH STREQUAL \"x64\")\n"
		    "\t\t\tset(CMAKE_C_FLAGS \"-m64\")\n"
		    "\t\telseif(ARCH STREQUAL \"x86\")\n"
		    "\t\t\tset(CMAKE_C_FLAGS \"-m32\")\n"
		    "\t\tendif()\n"
		    "\tendif()\n"
		    "endif()\n"
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
		    "endif()\n",
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
		    "option(OPEN \"Open HTML coverage report\" ON)\n"
		    "\n"
		    "set(ARCHS \"host\" CACHE STRING \"List of architectures to build\")\n"
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
		    "\n"
		    "if(is_multi_config)\n"
		    "\tset(CONFIG $<CONFIG>)\n"
		    "else()\n"
		    "\tset(CONFIG ${CMAKE_BUILD_TYPE})\n"
		    "endif()\n"
		    "set(CP ${CMAKE_COMMAND} -E copy)\n"
		    "set(DIR_PROJ ${CMAKE_SOURCE_DIR}/)\n"
		    "set(DIR_BUILD )\n"
		    "set(DIR_TMP ${DIR_PROJ}tmp/)\n"
		    "set(DIR_TMP_EXT ${DIR_TMP}ext/)\n"
		    "set(DIR_TMP_REP ${DIR_TMP}report/)\n"
		    "set(DIR_TMP_COV ${DIR_TMP_REP}cov/)\n"
		    "set(DIR_TMP_DL ${DIR_TMP}dl/)\n"
		    "\n"
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
		    "\n"
		    "\tset(DIR_OUT ${DIR_PROJ}bin/${ARCH}-${CONFIG}/)\n"
		    "\tset(DIR_OUT_INT ${CMAKE_BINARY_DIR})\n"
		    "\tset(DIR_OUT_LIB ${DIR_OUT}lib/)\n"
		    "\tset(DIR_OUT_DRV ${DIR_OUT}drivers/)\n"
		    "\tset(DIR_OUT_BIN ${DIR_OUT}bin/)\n"
		    "\tset(DIR_OUT_TST ${DIR_OUT}test/)\n"
		    "\n"
		    "\tif(CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\t\tif(ARCH STREQUAL \"x64\")\n"
		    "\t\t\tset(CMAKE_C_FLAGS \"-m64\")\n"
		    "\t\telseif(ARCH STREQUAL \"x86\")\n"
		    "\t\t\tset(CMAKE_C_FLAGS \"-m32\")\n"
		    "\t\tendif()\n"
		    "\tendif()\n"
		    "endif()\n"
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
		    "endif()\n",
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
		    "option(OPEN \"Open HTML coverage report\" ON)\n"
		    "\n"
		    "set(ARCHS \"host\" CACHE STRING \"List of architectures to build\")\n"
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
		    "\n"
		    "if(is_multi_config)\n"
		    "\tset(CONFIG $<CONFIG>)\n"
		    "else()\n"
		    "\tset(CONFIG ${CMAKE_BUILD_TYPE})\n"
		    "endif()\n"
		    "set(CP ${CMAKE_COMMAND} -E copy)\n"
		    "set(DIR_PROJ ${CMAKE_SOURCE_DIR}/)\n"
		    "set(DIR_BUILD )\n"
		    "set(DIR_TMP ${DIR_PROJ}tmp/)\n"
		    "set(DIR_TMP_EXT ${DIR_TMP}ext/)\n"
		    "set(DIR_TMP_REP ${DIR_TMP}report/)\n"
		    "set(DIR_TMP_COV ${DIR_TMP_REP}cov/)\n"
		    "set(DIR_TMP_DL ${DIR_TMP}dl/)\n"
		    "\n"
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
		    "\n"
		    "\tset(DIR_OUT ${DIR_PROJ}bin/${ARCH}-${CONFIG}/)\n"
		    "\tset(DIR_OUT_INT ${CMAKE_BINARY_DIR})\n"
		    "\tset(DIR_OUT_LIB ${DIR_OUT}lib/)\n"
		    "\tset(DIR_OUT_DRV ${DIR_OUT}drivers/)\n"
		    "\tset(DIR_OUT_BIN ${DIR_OUT}bin/)\n"
		    "\tset(DIR_OUT_TST ${DIR_OUT}test/)\n"
		    "\n"
		    "\tif(CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\t\tif(ARCH STREQUAL \"x64\")\n"
		    "\t\t\tset(CMAKE_C_FLAGS \"-m64\")\n"
		    "\t\telseif(ARCH STREQUAL \"x86\")\n"
		    "\t\t\tset(CMAKE_C_FLAGS \"-m32\")\n"
		    "\t\tendif()\n"
		    "\tendif()\n"
		    "endif()\n"
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
		    "endif()\n",
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
		    "option(OPEN \"Open HTML coverage report\" ON)\n"
		    "\n"
		    "set(ARCHS \"host\" CACHE STRING \"List of architectures to build\")\n"
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
		    "\n"
		    "if(is_multi_config)\n"
		    "\tset(CONFIG $<CONFIG>)\n"
		    "else()\n"
		    "\tset(CONFIG ${CMAKE_BUILD_TYPE})\n"
		    "endif()\n"
		    "set(CP ${CMAKE_COMMAND} -E copy)\n"
		    "set(DIR_PROJ ${CMAKE_SOURCE_DIR}/)\n"
		    "set(DIR_BUILD )\n"
		    "set(DIR_TMP ${DIR_PROJ}tmp/)\n"
		    "set(DIR_TMP_EXT ${DIR_TMP}ext/)\n"
		    "set(DIR_TMP_REP ${DIR_TMP}report/)\n"
		    "set(DIR_TMP_COV ${DIR_TMP_REP}cov/)\n"
		    "set(DIR_TMP_DL ${DIR_TMP}dl/)\n"
		    "\n"
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
		    "\n"
		    "\tset(DIR_OUT ${DIR_PROJ}bin/${ARCH}-${CONFIG}/)\n"
		    "\tset(DIR_OUT_INT ${CMAKE_BINARY_DIR})\n"
		    "\tset(DIR_OUT_LIB ${DIR_OUT}lib/)\n"
		    "\tset(DIR_OUT_DRV ${DIR_OUT}drivers/)\n"
		    "\tset(DIR_OUT_BIN ${DIR_OUT}bin/)\n"
		    "\tset(DIR_OUT_TST ${DIR_OUT}test/)\n"
		    "\n"
		    "\tif(CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\t\tif(ARCH STREQUAL \"x64\")\n"
		    "\t\t\tset(CMAKE_C_FLAGS \"-m64\")\n"
		    "\t\telseif(ARCH STREQUAL \"x86\")\n"
		    "\t\t\tset(CMAKE_C_FLAGS \"-m32\")\n"
		    "\t\tendif()\n"
		    "\tendif()\n"
		    "\n"
		    "\tinclude(pkg.cmake)\n"
		    "endif()\n"
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
		    "endif()\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_BIN})\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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

	char buf[3072] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(${PN}_URI url)\n"
		    "set(${PN}_URI_FILE )\n"
		    "set(${PN}_URI_NAME )\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(PKG_URI ${${PN}_URI})\n"
		    "set(PKG_URI_FILE ${${PN}_URI_FILE})\n"
		    "set(PKG_URI_NAME ${${PN}_URI_NAME})\n"
		    "set(PKG_URI_ROOT ${${PN}_URI_ROOT})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_TMP_EXT_PKG ${DIR_TMP_EXT}${PKG_DIR})\n"
		    "set(DIR_TMP_EXT_PKG_SRC ${DIR_TMP_EXT_PKG}${PKG_URI_NAME}-src/)\n"
		    "set(DIR_TMP_EXT_PKG_SRC_ROOT ${DIR_TMP_EXT_PKG_SRC}${PKG_URI_ROOT})\n"
		    "set(DIR_TMP_EXT_PKG_BUILD ${DIR_TMP_EXT_PKG}${PKG_URI_NAME}-build-${ARCH}-${CONFIG}/)\n"
		    "set(DIR_TMP_DL_PKG ${DIR_TMP_DL}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_TST})\n"
		    "set(DIR_OUT_TST_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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
		    "add_test(\n"
		    "\tNAME ${PN}_${TN}\n"
		    "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
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

TEST(gen_cmake_pkg)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n",
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
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_BIN})\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PRIVATE ${DIR_PKG}src)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
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

TEST(gen_cmake_pkg_exe_out)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe_out(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT exes)\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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

TEST(gen_cmake_pkg_exe_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe_drv(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_PKG_DRV ${DIR_PKG}drivers/)\n"
		    "set(DIR_PKG_DRV_C ${DIR_PKG_DRV}*.c)\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_DRV_PKG ${DIR_OUT_DRV}${PN}/)\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_BIN})\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c ${DIR_PKG}drivers/*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
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

TEST(gen_cmake_pkg_exe_drv_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_exe_drv_inc(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_PKG_DRV ${DIR_PKG}drivers/)\n"
		    "set(DIR_PKG_DRV_C ${DIR_PKG_DRV}*.c)\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_DRV_PKG ${DIR_OUT_DRV}${PN}/)\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_BIN})\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c ${DIR_PKG}drivers/*.c)\n"
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

TEST(gen_cmake_pkg_lib)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PRIVATE ${DIR_PKG}src)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
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

TEST(gen_cmake_pkg_lib_out)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_out(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT libs)\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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

TEST(gen_cmake_pkg_lib_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_inc(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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

TEST(gen_cmake_pkg_lib_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_drv(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_PKG_DRV ${DIR_PKG}drivers/)\n"
		    "set(DIR_PKG_DRV_C ${DIR_PKG_DRV}*.c)\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_DRV_PKG ${DIR_OUT_DRV}${PN}/)\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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

TEST(gen_cmake_pkg_lib_drv_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_drv_inc(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_PKG_DRV ${DIR_PKG}drivers/)\n"
		    "set(DIR_PKG_DRV_C ${DIR_PKG_DRV}*.c)\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_DRV_PKG ${DIR_OUT_DRV}${PN}/)\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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

TEST(gen_cmake_pkg_test)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_test(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"test\")\n"
		    "set(TGT_OUT ${DIR_OUT_TST})\n"
		    "set(DIR_OUT_TST_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}test/*.h ${DIR_PKG}test/*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PRIVATE ${DIR_PKG}test)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "add_test(\n"
		    "\tNAME ${PN}_${TN}\n"
		    "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
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

TEST(gen_cmake_pkg_test_out)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_test_out(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"test\")\n"
		    "set(TGT_OUT tests)\n"
		    "set(DIR_OUT_TST_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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
		    "add_test(\n"
		    "\tNAME ${PN}_${TN}\n"
		    "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
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

TEST(gen_cmake_pkg_test_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_test_drv(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_PKG_DRV ${DIR_PKG}drivers/)\n"
		    "set(DIR_PKG_DRV_C ${DIR_PKG_DRV}*.c)\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_DRV_PKG ${DIR_OUT_DRV}${PN}/)\n"
		    "\n"
		    "set(TN \"test\")\n"
		    "set(TGT_OUT ${DIR_OUT_TST})\n"
		    "set(DIR_OUT_TST_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c ${DIR_PKG}drivers/*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "add_test(\n"
		    "\tNAME ${PN}_${TN}\n"
		    "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
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

TEST(gen_cmake_pkg_lib_test)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "set(TGT_OUT ${DIR_OUT_TST})\n"
		    "set(DIR_OUT_TST_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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
		    "target_link_libraries(${PN}_${TN} PRIVATE _lib)\n"
		    "add_test(\n"
		    "\tNAME ${PN}_${TN}\n"
		    "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
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

TEST(gen_cmake_pkg_lib_test_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_inc(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "set(TGT_OUT ${DIR_OUT_TST})\n"
		    "set(DIR_OUT_TST_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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
		    "target_link_libraries(${PN}_${TN} PRIVATE _lib)\n"
		    "add_test(\n"
		    "\tNAME ${PN}_${TN}\n"
		    "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
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

TEST(gen_cmake_pkg_lib_test_inc_src)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_inc_src(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PUBLIC ${DIR_PKG}include PRIVATE ${DIR_PKG}src)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "set(TGT_OUT ${DIR_OUT_TST})\n"
		    "set(DIR_OUT_TST_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PRIVATE ${DIR_PKG}src)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE _lib)\n"
		    "add_test(\n"
		    "\tNAME ${PN}_${TN}\n"
		    "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
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

TEST(gen_cmake_pkg_lib_exe_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_exe_drv(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_PKG_DRV ${DIR_PKG}drivers/)\n"
		    "set(DIR_PKG_DRV_C ${DIR_PKG_DRV}*.c)\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_DRV_PKG ${DIR_OUT_DRV}${PN}/)\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PRIVATE ${DIR_PKG}src)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"exe\")\n"
		    "set(TGT_OUT ${DIR_OUT_BIN})\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}src/*.h ${DIR_PKG}src/*.c ${_DRIVERS})\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PRIVATE ${DIR_PKG}src)\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE _lib)\n"
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

TEST(gen_cmake_pkg_lib_test_drv)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_drv(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_PKG_DRV ${DIR_PKG}drivers/)\n"
		    "set(DIR_PKG_DRV_C ${DIR_PKG_DRV}*.c)\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_DRV_PKG ${DIR_OUT_DRV}${PN}/)\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "set(TGT_OUT ${DIR_OUT_TST})\n"
		    "set(DIR_OUT_TST_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c ${_DRIVERS})\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE _lib)\n"
		    "add_test(\n"
		    "\tNAME ${PN}_${TN}\n"
		    "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
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

TEST(gen_cmake_pkg_lib_test_drv_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_lib_test_drv_inc(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_PKG_DRV ${DIR_PKG}drivers/)\n"
		    "set(DIR_PKG_DRV_C ${DIR_PKG_DRV}*.c)\n"
		    "set(${PN}_DRIVERS \"${DIR_PKG_DRV_C}\")\n"
		    "set(DIR_OUT_DRV_PKG ${DIR_OUT_DRV}${PN}/)\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "set(TGT_OUT ${DIR_OUT_TST})\n"
		    "set(DIR_OUT_TST_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
		    "\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${DIR_PKG}*.h ${DIR_PKG}*.c ${_DRIVERS})\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "if (CMAKE_C_COMPILER_ID MATCHES \"GNU|Clang\")\n"
		    "\ttarget_compile_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "\ttarget_link_options(${PN}_${TN} PRIVATE\n"
		    "\t\t$<$<CONFIG:Debug>:--coverage>\n"
		    "\t)\n"
		    "endif()\n"
		    "target_link_libraries(${PN}_${TN} PRIVATE _lib)\n"
		    "add_test(\n"
		    "\tNAME ${PN}_${TN}\n"
		    "\tCOMMAND $<TARGET_FILE:${PN}_${TN}>\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"a\")\n"
		    "set(TGT_OUT ${DIR_OUT_BIN})\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"b\")\n"
		    "set(TGT_OUT ${DIR_OUT_BIN})\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"exe\")\n"
		    "set(TGT_OUT ${DIR_OUT_BIN})\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"base\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"lib1\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"lib2\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"exe\")\n"
		    "set(TGT_OUT ${DIR_OUT_BIN})\n"
		    "set(DIR_OUT_BIN_FILE ${TGT_OUT}${PN}${EXT_EXE})\n"
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
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_uri)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_uri(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(${PN}_URI url)\n"
		    "set(${PN}_URI_FILE file)\n"
		    "set(${PN}_URI_NAME name)\n"
		    "set(${PN}_URI_ROOT main/)\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(PKG_URI ${${PN}_URI})\n"
		    "set(PKG_URI_FILE ${${PN}_URI_FILE})\n"
		    "set(PKG_URI_NAME ${${PN}_URI_NAME})\n"
		    "set(PKG_URI_ROOT ${${PN}_URI_ROOT})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_TMP_EXT_PKG ${DIR_TMP_EXT}${PKG_DIR})\n"
		    "set(DIR_TMP_EXT_PKG_SRC ${DIR_TMP_EXT_PKG}${PKG_URI_NAME}-src/)\n"
		    "set(DIR_TMP_EXT_PKG_SRC_ROOT ${DIR_TMP_EXT_PKG_SRC}${PKG_URI_ROOT})\n"
		    "set(DIR_TMP_EXT_PKG_BUILD ${DIR_TMP_EXT_PKG}${PKG_URI_NAME}-build-${ARCH}-${CONFIG}/)\n"
		    "set(DIR_TMP_DL_PKG ${DIR_TMP_DL}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_cmd)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_cmd(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"pkg\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP prep)\n"
		    "set(TGT_CONF conf)\n"
		    "set(TGT_COMP comp)\n"
		    "set(TGT_INST inst)\n"
		    "set(TGT_TGT out)\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_out)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_out(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT exts)\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_library(${PN}_${TN} STATIC IMPORTED)\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_inc)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_inc(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_library(${PN}_${TN} STATIC IMPORTED)\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
		    "\tINTERFACE_INCLUDE_DIRECTORIES ${DIR_TMP_EXT_PKG_SRC_ROOT}include\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_lib)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_lib(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_library(${PN}_${TN} STATIC IMPORTED)\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_exe)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_exe(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_executable(${PN}_${TN} IMPORTED)\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
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
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(${PN}_URI url)\n"
		    "set(${PN}_URI_FILE file)\n"
		    "set(${PN}_URI_NAME name-1.0)\n"
		    "set(${PN}_URI_ROOT main/)\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(PKG_URI ${${PN}_URI})\n"
		    "set(PKG_URI_FILE ${${PN}_URI_FILE})\n"
		    "set(PKG_URI_NAME ${${PN}_URI_NAME})\n"
		    "set(PKG_URI_ROOT ${${PN}_URI_ROOT})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_TMP_EXT_PKG ${DIR_TMP_EXT}${PKG_DIR})\n"
		    "set(DIR_TMP_EXT_PKG_SRC ${DIR_TMP_EXT_PKG}${PKG_URI_NAME}-src/)\n"
		    "set(DIR_TMP_EXT_PKG_SRC_ROOT ${DIR_TMP_EXT_PKG_SRC}${PKG_URI_ROOT})\n"
		    "set(DIR_TMP_EXT_PKG_BUILD ${DIR_TMP_EXT_PKG}${PKG_URI_NAME}-build-${ARCH}-${CONFIG}/)\n"
		    "set(DIR_TMP_DL_PKG ${DIR_TMP_DL}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_tar_gz)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_tar_gz(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(${PN}_URI url)\n"
		    "set(${PN}_URI_FILE file)\n"
		    "set(${PN}_URI_NAME name-1.0)\n"
		    "set(${PN}_URI_ROOT main/)\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(PKG_URI ${${PN}_URI})\n"
		    "set(PKG_URI_FILE ${${PN}_URI_FILE})\n"
		    "set(PKG_URI_NAME ${${PN}_URI_NAME})\n"
		    "set(PKG_URI_ROOT ${${PN}_URI_ROOT})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "set(DIR_TMP_EXT_PKG ${DIR_TMP_EXT}${PKG_DIR})\n"
		    "set(DIR_TMP_EXT_PKG_SRC ${DIR_TMP_EXT_PKG}${PKG_URI_NAME}-src/)\n"
		    "set(DIR_TMP_EXT_PKG_SRC_ROOT ${DIR_TMP_EXT_PKG_SRC}${PKG_URI_ROOT})\n"
		    "set(DIR_TMP_EXT_PKG_BUILD ${DIR_TMP_EXT_PKG}${PKG_URI_NAME}-build-${ARCH}-${CONFIG}/)\n"
		    "set(DIR_TMP_DL_PKG ${DIR_TMP_DL}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    ")\n",
		    tmp.len);

	t_gen_free(&com);

	END;
}

TEST(gen_cmake_pkg_ext_deps)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_ext_deps(&com, STRV("C")), 0);

	char buf[4096] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(${PN}_DIR \"\")\n"
		    "set(DIR_OUT_EXT ${DIR_OUT}ext/${PN}/)\n"
		    "set(PKG_DIR ${${PN}_DIR})\n"
		    "set(DIR_PKG ${DIR_PROJ}${PKG_DIR})\n"
		    "\n"
		    "set(TN \"lib\")\n"
		    "set(TGT_OUT ${DIR_OUT_LIB})\n"
		    "set(DIR_OUT_LIB_FILE ${TGT_OUT}${PN}${EXT_LIB})\n"
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
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"ext\")\n"
		    "set(TGT_SRC ${DIR_TMP_EXT_PKG_SRC_ROOT})\n"
		    "set(TGT_BUILD ${DIR_TMP_EXT_PKG_BUILD})\n"
		    "set(TGT_OUT ${DIR_OUT_EXT})\n"
		    "set(TGT_PREP )\n"
		    "set(TGT_CONF )\n"
		    "set(TGT_COMP )\n"
		    "set(TGT_INST )\n"
		    "set(TGT_TGT )\n"
		    "set(DIR_OUT_EXT_PKG ${TGT_OUT})\n"
		    "set(DIR_OUT_EXT_FILE ${DIR_OUT_EXT_PKG}${TGT_TGT})\n"
		    "\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" TGT_BUILD_DEBUG \"${TGT_BUILD}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" TGT_BUILD_RELEASE \"${TGT_BUILD}\")\n"
		    "file(MAKE_DIRECTORY \"${DIR_TMP_DL_PKG}\" \"${TGT_BUILD_DEBUG}\" \"${TGT_BUILD_RELEASE}\")\n"
		    "file(DOWNLOAD ${PKG_URI} ${DIR_TMP_DL_PKG}${PKG_URI_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(ARCHIVE_EXTRACT INPUT \"${DIR_TMP_DL_PKG}${PKG_URI_FILE}\" DESTINATION \"${DIR_TMP_EXT_PKG_SRC}\")\n"
		    "add_custom_target(${PN}_${TN}_build\n"
		    "\tALL\n"
		    "\tCOMMAND ${TGT_PREP}\n"
		    "\tCOMMAND ${TGT_CONF}\n"
		    "\tCOMMAND ${TGT_COMP}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${DIR_OUT_EXT_PKG}\n"
		    "\tCOMMAND ${TGT_INST}\n"
		    "\tWORKING_DIRECTORY ${TGT_BUILD}\n"
		    ")\n"
		    "add_dependencies(${PN}_${TN} ${PN}_${TN}_build)\n"
		    "target_link_libraries(${PN}_${TN} INTERFACE _lib)\n"
		    "string(REPLACE \"$<CONFIG>\" \"Debug\" DIR_OUT_EXT_FILE_DEBUG \"${DIR_OUT_EXT_FILE}\")\n"
		    "string(REPLACE \"$<CONFIG>\" \"Release\" DIR_OUT_EXT_FILE_RELEASE \"${DIR_OUT_EXT_FILE}\")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tIMPORTED_LOCATION ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_DEBUG ${DIR_OUT_EXT_FILE_DEBUG}\n"
		    "\tIMPORTED_LOCATION_RELEASE ${DIR_OUT_EXT_FILE_RELEASE}\n"
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
	RUN(gen_cmake_pkg);
	RUN(gen_cmake_pkg_exe);
	RUN(gen_cmake_pkg_exe_out);
	RUN(gen_cmake_pkg_exe_drv);
	RUN(gen_cmake_pkg_exe_drv_inc);
	RUN(gen_cmake_pkg_lib);
	RUN(gen_cmake_pkg_lib_out);
	RUN(gen_cmake_pkg_lib_inc);
	RUN(gen_cmake_pkg_lib_drv);
	RUN(gen_cmake_pkg_lib_drv_inc);
	RUN(gen_cmake_pkg_test);
	RUN(gen_cmake_pkg_test_out);
	RUN(gen_cmake_pkg_test_drv);
	RUN(gen_cmake_pkg_lib_test);
	RUN(gen_cmake_pkg_lib_test_inc);
	RUN(gen_cmake_pkg_lib_test_inc_src);
	RUN(gen_cmake_pkg_lib_exe_drv);
	RUN(gen_cmake_pkg_lib_test_drv);
	RUN(gen_cmake_pkg_lib_test_drv_inc);
	RUN(gen_cmake_pkg_multi);
	RUN(gen_cmake_pkg_depends);
	RUN(gen_cmake_pkg_rdepends);
	RUN(gen_cmake_pkg_ext_unknown);
	RUN(gen_cmake_pkg_ext_uri);
	RUN(gen_cmake_pkg_ext_cmd);
	RUN(gen_cmake_pkg_ext_out);
	RUN(gen_cmake_pkg_ext_inc);
	RUN(gen_cmake_pkg_ext_lib);
	RUN(gen_cmake_pkg_ext_exe);
	RUN(gen_cmake_pkg_ext_zip);
	RUN(gen_cmake_pkg_ext_tar_gz);
	RUN(gen_cmake_pkg_ext_deps);

	SEND;
}
