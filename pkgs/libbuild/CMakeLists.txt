file(GLOB_RECURSE libbuild_src src/*.h src/*.c)
add_library(libbuild ${libbuild_src})
target_link_libraries(libbuild PUBLIC cparse)
target_include_directories(libbuild PUBLIC include)
if(NOT MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(libbuild PRIVATE -Wall -Wextra -Werror -pedantic -coverage -O0 -ggdb)
		target_link_options(libbuild PRIVATE -coverage)
	else()
		target_compile_options(libbuild PRIVATE -Wall -Wextra -Werror -pedantic)
	endif()
endif()
set_target_properties(libbuild PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/libbuild/${ARCH}-Debug/
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/libbuild/${ARCH}-Release/
	PREFIX ""
)

file(GLOB_RECURSE libbuild_test_src test/*.h test/*.c drivers/*.c)
add_executable(libbuild_test ${libbuild_test_src})
target_link_libraries(libbuild_test libbuild ctest)
if(NOT MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(libbuild_test PRIVATE -Wall -Wextra -Werror -pedantic -coverage -O0 -ggdb)
		target_link_options(libbuild_test PRIVATE -coverage)
	else()
		target_compile_options(libbuild_test PRIVATE -Wall -Wextra -Werror -pedantic)
	endif()
endif()
set_target_properties(libbuild_test PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/libbuild_test/${ARCH}-Debug/
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/libbuild_test/${ARCH}-Release/
)
add_test(libbuild_build "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --config ${CMAKE_BUILD_TYPE} --target libbuild_test)
add_test(libbuild ${CMAKE_SOURCE_DIR}/bin/libbuild_test/${ARCH}-${CMAKE_BUILD_TYPE}/libbuild_test)
set_tests_properties(libbuild PROPERTIES
	DEPENDS libbuild_build
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
