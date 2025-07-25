#include "t_gen_common.h"

#include "log.h"
#include "mem.h"
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

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("tmp/build/CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project( LANGUAGES C)\n"
		    "\n"
		    "set(PROJDIR \"../../\")\n"
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

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project( LANGUAGES C)\n"
		    "\n"
		    "set(PROJDIR \"\")\n"
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

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project(proj LANGUAGES C)\n"
		    "\n"
		    "set(PROJDIR \"\")\n"
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

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project( LANGUAGES C)\n"
		    "\n"
		    "set(PROJDIR \"\")\n"
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
		    "target_link_libraries(${PN}_${TN} PRIVATE)\n"
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
		    "target_link_libraries(${PN}_${TN} PRIVATE lib_lib)\n"
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

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&com.fs, STRV("./a/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PN \"a\")\n"
		    "set(PKGDIR \"a\")\n"
		    "set(TN \"a\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
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
		    "set(PKGDIR \"b\")\n"
		    "set(TN \"b\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
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
		    "set(PKGDIR \"lib\")\n"
		    "set(TN \"lib\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
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
		    "set(PKGDIR \"exe\")\n"
		    "set(TN \"exe\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
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
		    "set(PKGDIR \"base\")\n"
		    "set(TN \"base\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
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
		    "set(PKGDIR \"lib1\")\n"
		    "set(TN \"lib1\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
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
		    "set(PKGDIR \"lib2\")\n"
		    "set(TN \"lib2\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
		    "add_library(${PN}_${TN} ${${PN}_${TN}_src})\n"
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
		    "set(PKGDIR \"exe\")\n"
		    "set(TN \"exe\")\n"
		    "file(GLOB_RECURSE ${PN}_${TN}_src ${PROJDIR}${PKGDIR}*.h ${PROJDIR}${PKGDIR}*.c)\n"
		    "add_executable(${PN}_${TN} ${${PN}_${TN}_src})\n"
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
	RUN(gen_cmake_proj_test);
	RUN(gen_cmake_pkg_exe);
	RUN(gen_cmake_pkg_lib);
	RUN(gen_cmake_pkg_lib_test);
	RUN(gen_cmake_pkg_multi);
	RUN(gen_cmake_pkg_depends);
	RUN(gen_cmake_pkg_rdepends);

	SEND;
}
