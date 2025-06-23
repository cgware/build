#include "gen.h"

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

	fs_t fs = {0};
	fs_init(&fs, 3, 1, ALLOC_STD);

	fs_mkpath(&fs, STRV_NULL, STRV("tmp/build"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV(""), STRV("tmp/build/")), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&fs, STRV("tmp/build/CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project( LANGUAGES C)\n"
		    "\n"
		    "set(PROJDIR \"../../\")\n"
		    "\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_cmake_proj_empty)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkpath(&fs, STRV_NULL, STRV_NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV_NULL, STRV_NULL), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&fs, STRV("CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project( LANGUAGES C)\n"
		    "\n"
		    "set(PROJDIR \"\")\n"
		    "\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_cmake_proj_name)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 1, 1, ALLOC_STD);

	fs_mkpath(&fs, STRV_NULL, STRV_NULL);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	proj.name = STRV("proj");

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV_NULL, STRV_NULL), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&fs, STRV("CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project(proj LANGUAGES C)\n"
		    "\n"
		    "set(PROJDIR \"\")\n"
		    "\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_cmake_pkg_unknown)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 2, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint pkg;
	proj_add_pkg(&proj, STRV_NULL, &pkg);
	proj_add_target(&proj, pkg, STRV("pkg"), NULL);

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV_NULL, STRV_NULL), 0);

	char buf[256] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&fs, STRV("CMakeLists.txt"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "cmake_minimum_required(VERSION 3.10)\n"
		    "\n"
		    "project( LANGUAGES C)\n"
		    "\n"
		    "set(PROJDIR \"\")\n"
		    "\n"
		    "include(pkg.cmake)\n",
		    tmp.len);

	fs_read(&fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"\")\n"
		    "set(PKGDIR \"\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    ")\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_cmake_pkg_exe_empty)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 2, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint exe;
	target_t *target;

	proj_add_pkg(&proj, STRV_NULL, &exe);
	target	     = proj_add_target(&proj, exe, STRV("pkg"), &exe);
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV_NULL, STRV_NULL), 0);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"\")\n"
		    "set(PKGDIR \"\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "add_executable(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PRIVATE)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_cmake_pkg_exe)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 2, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	path_init(&proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&proj, STRV("pkg"), &exe);
	proj_set_str(&proj, pkg->strs + PKG_SRC, STRV("src"));
	target	     = proj_add_target(&proj, exe, STRV("pkg"), &exe);
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV_NULL, STRV_NULL), 0);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"pkg\")\n"
		    "set(PKGDIR \"\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}src/*.h ${PROJDIR}${PKGDIR}src/*.c)\n"
		    "add_executable(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PRIVATE)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Release/bin/\n"
		    ")\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_cmake_pkg_lib)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 2, 1, ALLOC_STD);

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	path_init(&proj.outdir, STRV("bin/${ARCH}-${CONFIG}/"));

	uint lib;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&proj, STRV("pkg"), &lib);
	proj_set_str(&proj, pkg->strs + PKG_SRC, STRV("src"));
	proj_set_str(&proj, pkg->strs + PKG_INC, STRV("include"));
	target	     = proj_add_target(&proj, lib, STRV("pkg"), &lib);
	target->type = TARGET_TYPE_LIB;

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV_NULL, STRV_NULL), 0);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&fs, STRV("pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"pkg\")\n"
		    "set(PKGDIR \"\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}src/*.h ${PROJDIR}${PKGDIR}src/*.c)\n"
		    "add_library(${PKG} ${${PKG}_src})\n"
		    "target_include_directories(${PKG} PUBLIC ${PROJDIR}${PKGDIR}include)\n"
		    "target_link_libraries(${PKG} PUBLIC)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Debug/lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/${PROJDIR}bin/${ARCH}-Release/lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_cmake_pkg_multi)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 6, 1, ALLOC_STD);

	fs_mkpath(&fs, STRV_NULL, STRV("./a"));
	fs_mkpath(&fs, STRV_NULL, STRV("./b"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint a, b;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&proj, STRV("a"), &a);
	proj_set_str(&proj, pkg->strs + PKG_DIR, STRV("a"));
	target	     = proj_add_target(&proj, a, STRV("a"), &a);
	target->type = TARGET_TYPE_EXE;

	pkg = proj_add_pkg(&proj, STRV("b"), &b);
	proj_set_str(&proj, pkg->strs + PKG_DIR, STRV("b"));
	target	     = proj_add_target(&proj, b, STRV("b"), &b);
	target->type = TARGET_TYPE_EXE;

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV("."), STRV(".")), 0);

	char buf[512] = {0};
	str_t tmp     = STRB(buf, 0);

	fs_read(&fs, STRV("./a/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"a\")\n"
		    "set(PKGDIR \"a\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "add_executable(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PRIVATE)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	fs_read(&fs, STRV("./b/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"b\")\n"
		    "set(PKGDIR \"b\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "add_executable(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PRIVATE)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_cmake_pkg_depends)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 6, 1, ALLOC_STD);

	fs_mkpath(&fs, STRV_NULL, STRV("./lib"));
	fs_mkpath(&fs, STRV_NULL, STRV("./exe"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint lib, exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&proj, STRV("lib"), &lib);
	proj_set_str(&proj, pkg->strs + PKG_DIR, STRV("lib"));
	target	     = proj_add_target(&proj, lib, STRV("lib"), &lib);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&proj, STRV("exe"), &exe);
	proj_set_str(&proj, pkg->strs + PKG_DIR, STRV("exe"));
	target	     = proj_add_target(&proj, exe, STRV("exe"), &exe);
	target->type = TARGET_TYPE_EXE;

	proj_add_dep(&proj, exe, lib);

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV("."), STRV(".")), 0);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&fs, STRV("./lib/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"lib\")\n"
		    "set(PKGDIR \"lib\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "add_library(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PUBLIC)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	fs_read(&fs, STRV("./exe/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"exe\")\n"
		    "set(PKGDIR \"exe\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "add_executable(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PRIVATE lib)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

TEST(gen_cmake_pkg_rdepends)
{
	START;

	fs_t fs = {0};
	fs_init(&fs, 6, 1, ALLOC_STD);

	fs_mkpath(&fs, STRV_NULL, STRV("./lib"));
	fs_mkpath(&fs, STRV_NULL, STRV("./exe"));

	proj_t proj = {0};
	proj_init(&proj, 1, 1, ALLOC_STD);

	uint base, lib1, lib2, exe;
	pkg_t *pkg;
	target_t *target;

	pkg = proj_add_pkg(&proj, STRV("base"), &base);
	proj_set_str(&proj, pkg->strs + PKG_DIR, STRV("base"));
	target	     = proj_add_target(&proj, base, STRV("base"), &base);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&proj, STRV("lib1"), &lib1);
	proj_set_str(&proj, pkg->strs + PKG_DIR, STRV("lib1"));
	target	     = proj_add_target(&proj, lib1, STRV("lib1"), &lib1);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&proj, STRV("lib2"), &lib2);
	proj_set_str(&proj, pkg->strs + PKG_DIR, STRV("lib2"));
	target	     = proj_add_target(&proj, lib2, STRV("lib2"), &lib2);
	target->type = TARGET_TYPE_LIB;

	pkg = proj_add_pkg(&proj, STRV("exe"), &exe);
	proj_set_str(&proj, pkg->strs + PKG_DIR, STRV("exe"));
	target	     = proj_add_target(&proj, exe, STRV("exe"), &exe);
	target->type = TARGET_TYPE_EXE;

	proj_add_dep(&proj, lib1, base);
	proj_add_dep(&proj, lib2, base);
	proj_add_dep(&proj, exe, lib1);
	proj_add_dep(&proj, exe, lib2);

	gen_driver_t drv = *gen_find_param(STRV("C"));

	drv.fs = &fs;

	EXPECT_EQ(drv.gen(&drv, &proj, STRV("."), STRV(".")), 0);

	char buf[1024] = {0};
	str_t tmp      = STRB(buf, 0);

	fs_read(&fs, STRV("./base/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"base\")\n"
		    "set(PKGDIR \"base\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "add_library(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PUBLIC)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	fs_read(&fs, STRV("./lib1/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"lib1\")\n"
		    "set(PKGDIR \"lib1\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "add_library(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PUBLIC base)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	fs_read(&fs, STRV("./lib2/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"lib2\")\n"
		    "set(PKGDIR \"lib2\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "add_library(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PUBLIC base)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_DEBUG lib/\n"
		    "\tARCHIVE_OUTPUT_DIRECTORY_RELEASE lib/\n"
		    "\tPREFIX \"\"\n"
		    ")\n",
		    tmp.len);

	fs_read(&fs, STRV("./exe/pkg.cmake"), 0, &tmp);
	EXPECT_STRN(tmp.data,
		    "set(PKG \"exe\")\n"
		    "set(PKGDIR \"exe\")\n"
		    "file(GLOB_RECURSE ${PKG}_src ${PROJDIR}${PKGDIR}/*.h ${PROJDIR}${PKGDIR}/*.c)\n"
		    "add_executable(${PKG} ${${PKG}_src})\n"
		    "target_link_libraries(${PKG} PRIVATE lib1 lib2)\n"
		    "set_target_properties(${PKG} PROPERTIES\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_DEBUG bin/\n"
		    "\tRUNTIME_OUTPUT_DIRECTORY_RELEASE bin/\n"
		    ")\n",
		    tmp.len);

	proj_free(&proj);
	fs_free(&fs);

	END;
}

STEST(gen_cmake)
{
	SSTART;

	RUN(gen_cmake_null);
	RUN(gen_cmake_proj_build_dir);
	RUN(gen_cmake_proj_empty);
	RUN(gen_cmake_proj_name);
	RUN(gen_cmake_pkg_unknown);
	RUN(gen_cmake_pkg_exe_empty);
	RUN(gen_cmake_pkg_exe);
	RUN(gen_cmake_pkg_lib);
	RUN(gen_cmake_pkg_multi);
	RUN(gen_cmake_pkg_depends);
	RUN(gen_cmake_pkg_rdepends);

	SEND;
}
