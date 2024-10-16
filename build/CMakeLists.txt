file(GLOB_RECURSE build_src src/*.h src/*.c)

add_executable(build ${build_src})
