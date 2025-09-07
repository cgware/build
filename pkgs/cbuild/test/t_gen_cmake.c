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

	char buf[1024] = {0};
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
		    "set(PROJDIR \"../../\")\n"
		    "set(CONFIG \"${CMAKE_BUILD_TYPE}\")\n"
		    "\n"
		    "set(OUTDIR \"\")\n"
		    "\n"
		    "set(INTDIR \"${CMAKE_BINARY_DIR}\")\n"
		    "\n"
		    "set(REPDIR \"${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/report/\")\n"
		    "set(COVDIR \"${REPDIR}cov/\")\n"
		    "\n"
		    "add_custom_target(cov\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${COVDIR}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CONFIG} --target test\n"
		    "\tCOMMAND if [ -n \\\"$$\\(find ${CMAKE_BINARY_DIR} -name *.gcda\\)\\\" ]\\; then \n"
		    "\t\tlcov -q -c -o ${COVDIR}lcov.info -d ${INTDIR}\\;\n"
		    "\t\tgenhtml -q -o ${COVDIR} ${COVDIR}lcov.info\\;\n"
		    "\t\t[ \\\"${OPEN}\\\" = \\\"1\\\" ] && open ${COVDIR}index.html || true\\;\n"
		    "\tfi\n"
		    "\tWORKING_DIRECTORY ${CMAKE_BINARY_DIR}\n"
		    ")\n"
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

	char buf[1024] = {0};
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
		    "set(PROJDIR \"\")\n"
		    "set(CONFIG \"${CMAKE_BUILD_TYPE}\")\n"
		    "\n"
		    "set(OUTDIR \"\")\n"
		    "\n"
		    "set(INTDIR \"${CMAKE_BINARY_DIR}\")\n"
		    "\n"
		    "set(REPDIR \"${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/report/\")\n"
		    "set(COVDIR \"${REPDIR}cov/\")\n"
		    "\n"
		    "add_custom_target(cov\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${COVDIR}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CONFIG} --target test\n"
		    "\tCOMMAND if [ -n \\\"$$\\(find ${CMAKE_BINARY_DIR} -name *.gcda\\)\\\" ]\\; then \n"
		    "\t\tlcov -q -c -o ${COVDIR}lcov.info -d ${INTDIR}\\;\n"
		    "\t\tgenhtml -q -o ${COVDIR} ${COVDIR}lcov.info\\;\n"
		    "\t\t[ \\\"${OPEN}\\\" = \\\"1\\\" ] && open ${COVDIR}index.html || true\\;\n"
		    "\tfi\n"
		    "\tWORKING_DIRECTORY ${CMAKE_BINARY_DIR}\n"
		    ")\n"
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

	char buf[1024] = {0};
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
		    "set(PROJDIR \"\")\n"
		    "set(CONFIG \"${CMAKE_BUILD_TYPE}\")\n"
		    "\n"
		    "set(OUTDIR \"\")\n"
		    "\n"
		    "set(INTDIR \"${CMAKE_BINARY_DIR}\")\n"
		    "\n"
		    "set(REPDIR \"${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/report/\")\n"
		    "set(COVDIR \"${REPDIR}cov/\")\n"
		    "\n"
		    "add_custom_target(cov\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${COVDIR}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CONFIG} --target test\n"
		    "\tCOMMAND if [ -n \\\"$$\\(find ${CMAKE_BINARY_DIR} -name *.gcda\\)\\\" ]\\; then \n"
		    "\t\tlcov -q -c -o ${COVDIR}lcov.info -d ${INTDIR}\\;\n"
		    "\t\tgenhtml -q -o ${COVDIR} ${COVDIR}lcov.info\\;\n"
		    "\t\t[ \\\"${OPEN}\\\" = \\\"1\\\" ] && open ${COVDIR}index.html || true\\;\n"
		    "\tfi\n"
		    "\tWORKING_DIRECTORY ${CMAKE_BINARY_DIR}\n"
		    ")\n"
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

	char buf[1024] = {0};
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
		    "set(PROJDIR \"\")\n"
		    "set(CONFIG \"${CMAKE_BUILD_TYPE}\")\n"
		    "\n"
		    "set(OUTDIR \"\")\n"
		    "\n"
		    "set(INTDIR \"${CMAKE_BINARY_DIR}\")\n"
		    "\n"
		    "set(REPDIR \"${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/report/\")\n"
		    "set(COVDIR \"${REPDIR}cov/\")\n"
		    "\n"
		    "add_custom_target(cov\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${COVDIR}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CONFIG} --target test\n"
		    "\tCOMMAND if [ -n \\\"$$\\(find ${CMAKE_BINARY_DIR} -name *.gcda\\)\\\" ]\\; then \n"
		    "\t\tlcov -q -c -o ${COVDIR}lcov.info -d ${INTDIR}\\;\n"
		    "\t\tgenhtml -q -o ${COVDIR} ${COVDIR}lcov.info\\;\n"
		    "\t\t[ \\\"${OPEN}\\\" = \\\"1\\\" ] && open ${COVDIR}index.html || true\\;\n"
		    "\tfi\n"
		    "\tWORKING_DIRECTORY ${CMAKE_BINARY_DIR}\n"
		    ")\n"
		    "\n"
		    "include(pkg.cmake)\n",
		    tmp.len);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(PKGDIR \"\")\n"
		    "set(TN \"pkg\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(PKGDIR \"\")\n"
		    "set(TN \"pkg\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(PKGDIR \"\")\n"
		    "set(TN \"pkg\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(PKGDIR \"\")\n"
		    "set(TN \"pkg\")\n"
		    "set(URL url)\n"
		    "set(ZIP_FILE ${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/dl/main.zip)\n"
		    "set(EXT_DIR ${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/ext/)\n"
		    "set(CMD )\n"
		    "add_custom_target(${PN}_${TN} ALL\n"
		    "\tCOMMAND ${CMD}\n"
		    "\tWORKING_DIRECTORY ${EXT_DIR}${URI_ROOT}\n"
		    ")\n"
		    "file(DOWNLOAD ${URL} ${ZIP_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(MAKE_DIRECTORY \"${EXT_DIR}\")\n"
		    "execute_process(\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E tar xzf ${ZIP_FILE}\n"
		    "\tWORKING_DIRECTORY ${EXT_DIR}\n"
		    ")\n"
		    "add_custom_command(TARGET ${PN}_${TN} POST_BUILD\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-${CONFIG}/ext/${PN}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E copy ${EXT_DIR}${URI_ROOT}${OUT} "
		    "${CMAKE_SOURCE_DIR}/${PROJDIR}/bin/${ARCH}-${CONFIG}/ext/${PN}/\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"\")\n"
		    "set(PKGDIR \"\")\n"
		    "set(TN \"pkg\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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
		    "add_test(${PN}_${TN}_build ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CONFIG} --target ${PN}_${TN})\n"
		    "add_test(${PN} ${CMAKE_SOURCE_DIR}/${PROJDIR}/bin/${ARCH}-${CONFIG}/test/${PN})\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(PKGDIR \"\")\n"
		    "set(TN \"pkg\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}src/*.h ${PROJDIR}${PKGDIR}src/*.c)\n"
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
		    "\tRUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Release/bin/\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(PKGDIR \"\")\n"
		    "set(TN \"pkg\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}src/*.h ${PROJDIR}${PKGDIR}src/*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
		    "target_include_directories(${PN}_${TN} PUBLIC ${PROJDIR}${PKGDIR}include)\n"
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
		    "\tARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Release/lib/\n"
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

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"lib\")\n"
		    "set(PKGDIR \"\")\n"
		    "set(TN \"lib\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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
		    "\tARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n"
		    "set(TN \"test\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}test/*.h ${PROJDIR}${PKGDIR}test/*.c)\n"
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
		    "add_test(${PN}_${TN}_build ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --config ${CONFIG} --target ${PN}_${TN})\n"
		    "add_test(${PN} ${CMAKE_SOURCE_DIR}/${PROJDIR}/bin/${ARCH}-${CONFIG}/test/${PN})\n"
		    "set_tests_properties(${PN} PROPERTIES\n"
		    "\tDEPENDS ${PN}_${TN}_build\n"
		    "\tWORKING_DIRECTORY ${CMAKE_SOURCE_DIR}\n"
		    ")\n"
		    "set_target_properties(${PN}_${TN} PROPERTIES\n"
		    "\tOUTPUT_NAME \"${PN}\"\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/test/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Release/test/\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("./a/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"a\")\n"
		    "set(PKGDIR \"a" SEP "\")\n"
		    "set(TN \"a\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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
		    "set(PKGDIR \"b" SEP "\")\n"
		    "set(TN \"b\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("./lib/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"lib\")\n"
		    "set(PKGDIR \"lib" SEP "\")\n"
		    "set(TN \"lib\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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
		    "set(PKGDIR \"exe" SEP "\")\n"
		    "set(TN \"exe\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("./base/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"base\")\n"
		    "set(PKGDIR \"base" SEP "\")\n"
		    "set(TN \"base\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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
		    "set(PKGDIR \"lib1" SEP "\")\n"
		    "set(TN \"lib1\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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
		    "set(PKGDIR \"lib2" SEP "\")\n"
		    "set(TN \"lib2\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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
		    "set(PKGDIR \"exe" SEP "\")\n"
		    "set(TN \"exe\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
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

TEST(gen_cmake_pkg_zip)
{
	START;

	t_gen_common_t com = {0};
	EXPECT_EQ(t_gen_pkg_zip(&com, STRV("C")), 0);

	char buf[2048] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&com.fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"pkg\")\n"
		    "set(PKGDIR \"\")\n"
		    "set(TN \"pkg\")\n"
		    "set(URL url)\n"
		    "set(ZIP_FILE ${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/dl/main.zip)\n"
		    "set(EXT_DIR ${CMAKE_SOURCE_DIR}/${PROJDIR}tmp/ext/)\n"
		    "set(URI_ROOT main)\n"
		    "set(OUT out)\n"
		    "set(CMD cmd)\n"
		    "add_custom_target(${PN}_${TN} ALL\n"
		    "\tCOMMAND ${CMD}\n"
		    "\tWORKING_DIRECTORY ${EXT_DIR}${URI_ROOT}\n"
		    ")\n"
		    "file(DOWNLOAD ${URL} ${ZIP_FILE}\n"
		    "\tSHOW_PROGRESS\n"
		    ")\n"
		    "file(MAKE_DIRECTORY \"${EXT_DIR}\")\n"
		    "execute_process(\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E tar xzf ${ZIP_FILE}\n"
		    "\tWORKING_DIRECTORY ${EXT_DIR}\n"
		    ")\n"
		    "add_custom_command(TARGET ${PN}_${TN} POST_BUILD\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-${CONFIG}/ext/${PN}\n"
		    "\tCOMMAND ${CMAKE_COMMAND} -E copy ${EXT_DIR}${URI_ROOT}${OUT} "
		    "${CMAKE_SOURCE_DIR}/${PROJDIR}/bin/${ARCH}-${CONFIG}/ext/${PN}/\n"
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
	RUN(gen_cmake_pkg_lib);
	RUN(gen_cmake_pkg_lib_test);
	RUN(gen_cmake_pkg_multi);
	RUN(gen_cmake_pkg_depends);
	RUN(gen_cmake_pkg_rdepends);
	RUN(gen_cmake_pkg_zip);

	SEND;
}
