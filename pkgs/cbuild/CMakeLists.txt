file(GLOB_RECURSE cbuild_src src/*.h src/*.c)
add_library(cbuild ${cbuild_src})
target_link_libraries(cbuild PUBLIC cparse)
target_include_directories(cbuild PUBLIC include)
if(NOT MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(cbuild PRIVATE -Wall -Wextra -Werror -pedantic -coverage -O0 -ggdb)
		target_link_options(cbuild PRIVATE -coverage)
	else()
		target_compile_options(cbuild PRIVATE -Wall -Wextra -Werror -pedantic)
	endif()
endif()
set_target_properties(cbuild PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/cbuild/${ARCH}-Debug/
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/cbuild/${ARCH}-Release/
	PREFIX ""
)

file(GLOB_RECURSE cbuild_test_src test/*.h test/*.c drivers/*.c)
add_executable(cbuild_test ${cbuild_test_src})
target_link_libraries(cbuild_test cbuild ctest)
if(NOT MSVC)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_options(cbuild_test PRIVATE -Wall -Wextra -Werror -pedantic -coverage -O0 -ggdb)
		target_link_options(cbuild_test PRIVATE -coverage)
	else()
		target_compile_options(cbuild_test PRIVATE -Wall -Wextra -Werror -pedantic)
	endif()
endif()
set_target_properties(cbuild_test PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/cbuild_test/${ARCH}-Debug/
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/cbuild_test/${ARCH}-Release/
	VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
add_test(cbuild_build "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --config ${CMAKE_BUILD_TYPE} --target cbuild_test)
add_test(cbuild ${CMAKE_SOURCE_DIR}/bin/cbuild_test/${ARCH}-${CMAKE_BUILD_TYPE}/cbuild_test)
set_tests_properties(cbuild PROPERTIES
	DEPENDS cbuild_build
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
