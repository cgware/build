cmake_minimum_required(VERSION 3.10)

project(build LANGUAGES C)

enable_testing()

find_program(LCOV lcov)
find_program(GENHTML genhtml)
add_custom_target(build_coverage
	COMMAND ${LCOV} -q -c -d ${CMAKE_BINARY_DIR} -o ${CMAKE_BINARY_DIR}/lcov.info
	COMMAND ${GENHTML} -q ${CMAKE_BINARY_DIR}/lcov.info -o ${CMAKE_SOURCE_DIR}/report/coverage
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

if(NOT MSVC)
	if(ARCH STREQUAL "x64")
		set(CMAKE_C_FLAGS "-m64")
	elseif(ARCH STREQUAL "x86")
		set(CMAKE_C_FLAGS "-m32")
	endif()
endif()

if(WIN32)
    set(LIB_EXT ".lib")
else()
	set(LIB_EXT ".a")
endif()

include(FetchContent)
FetchContent_Declare(cparse
  GIT_REPOSITORY    https://github.com/cgware/cparse.git
  GIT_TAG           main
  SOURCE_DIR        ${CMAKE_SOURCE_DIR}/deps/cparse
)
FetchContent_MakeAvailable(cparse)

add_subdirectory(pkgs)
