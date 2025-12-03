cmake_minimum_required(VERSION 3.10)

project(build LANGUAGES C)

include(FetchContent)

FetchContent_Declare(cbase
	GIT_REPOSITORY https://github.com/cgware/cbase.git
	GIT_TAG        main
)

FetchContent_Declare(cutils
	GIT_REPOSITORY https://github.com/cgware/cutils.git
	GIT_TAG        main
)

FetchContent_Declare(cparse
	GIT_REPOSITORY https://github.com/cgware/cparse.git
	GIT_TAG        main
)

FetchContent_Populate(cbase)
file(GLOB_RECURSE cbase_src ${cbase_SOURCE_DIR}/src/*.c)
add_library(cbase ${cbase_src})
target_include_directories(cbase PUBLIC ${cbase_SOURCE_DIR}/include)

FetchContent_Populate(cutils)
file(GLOB_RECURSE cutils_src ${cutils_SOURCE_DIR}/src/*.c)
add_library(cutils ${cutils_src})
target_link_libraries(cutils PUBLIC cbase)
target_include_directories(cutils PUBLIC ${cutils_SOURCE_DIR}/include)

FetchContent_Populate(cparse)
file(GLOB_RECURSE cparse_src ${cparse_SOURCE_DIR}/src/*.c)
add_library(cparse ${cparse_src})
target_link_libraries(cparse PUBLIC cutils)
target_include_directories(cparse PRIVATE ${cparse_SOURCE_DIR}/src PUBLIC ${cparse_SOURCE_DIR}/include)

file(GLOB_RECURSE cbuild_src pkgs/cbuild/src/*.h pkgs/cbuild/src/*.c)
add_library(cbuild ${cbuild_src})
target_link_libraries(cbuild PUBLIC cparse)
target_include_directories(cbuild PUBLIC pkgs/cbuild/include)

file(GLOB_RECURSE build_src pkgs/build/src/*.h pkgs/build/src/*.c ${CMAKE_SOURCE_DIR}/pkgs/cbuild/drivers/*.c)
add_executable(build ${build_src})
target_link_libraries(build cbuild)
set_target_properties(build PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/bootstrap/
)
