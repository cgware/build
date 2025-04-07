file(GLOB_RECURSE build_src src/*.h src/*.c ${CMAKE_SOURCE_DIR}/pkgs/cbuild/drivers/*.c)
add_executable(build ${build_src})
target_link_libraries(build cbuild)
if(NOT MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(build PRIVATE -Wall -Wextra -Werror -pedantic -coverage -O0 -ggdb)
		target_link_options(build PRIVATE -coverage)
	else()
		target_compile_options(build PRIVATE -Wall -Wextra -Werror -pedantic)
	endif()
endif()
set_target_properties(build PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/build/${ARCH}-Debug/
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/build/${ARCH}-Release/
)
