#!/bin/sh

usage() {
	echo "Usage: $0 <arch> <config>"
	exit 1
}

if [ "$#" -ne 2 ]; then
	usage
fi

arch="$1"
config="$2"

archs="host"
configs="Debug"

ret=0

run() {
	proj=$1
	targets=$2
	name=$3
	gen=$4

	dir="./examples/$proj"
	bin="$dir/bin"
	tmp="$dir/tmp"
	build="$dir/build"

	printf "%-13s %-5s " "$proj" "$name"

	rm -rf "$bin" "$build" "$tmp"

	if ! out="$(./bin/"$arch"-"$config"/bin/build -p "$dir" -g "$gen" -a "$archs" -c "$configs" -t "all test cov" -O 0 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "build: Failed to build $name"
		echo "$out"
		ret=1
		return
	fi

	for a in $archs; do
		for c in $configs; do
			for t in $targets; do
				if [ ! -f "$dir/bin/$a-$c/$t" ]; then
					printf "\033[0;31mFAIL\033[0m\n"
					echo "Target not found: $a-$c/$t"
					echo "$out"
					ret=1
					return
				fi
			done
		done
	done

	printf "\033[0;32mPASS\033[0m\n"
	rm -rf "$bin" "$build" "$tmp"
}

gen() {
	run "$@" Make M
	#run "$@" CMake C
}

test() {
	#gen 00_exe "bin/00_exe"
	#gen 01_lib "lib/01_lib.a"
	#gen 02_test "test/02_test"
	#gen 03_lib_test "lib/03_lib_test.a test/03_lib_test ../../tmp/report/cov/index.html"
	#gen 04_driver_test "drivers/04_driver_test/drv.o test/04_driver_test ../../tmp/report/cov/index.html"
	#gen 05_driver_exe "drivers/05_driver_exe/drv.o bin/05_driver_exe"
	gen 06_lib_driver_test "lib/06_lib_driver_test.a drivers/06_lib_driver_test/drv.o test/06_lib_driver_test ../../tmp/report/cov/index.html"
	#gen 08_multi "bin/a bin/b"
	#gen 09_rdepends "lib/base.a lib/lib1.a lib/lib2.a bin/exe"
	#gen 10_exe_dep_lib "libs/lib/lib.a bin/10_exe_dep_lib"
	#gen 11_exe_dep_driver "drivers/driver/drv.o 11_exe_dep_driver.a bin/11_exe_dep_driver"
	#gen 12_extern "lib/cbase.a"
	#gen 13_zip "../../tmp/dl/cbase-cef9bd84547f055b91d7dd80ad9b6a769e6c99a8.zip ext/cbase/cbase.a bin/exe"
	#gen 14_tar_gz "../../tmp/dl/cbase-cef9bd84547f055b91d7dd80ad9b6a769e6c99a8.tar.gz libs/cbase.a bin/exe"
	#gen 15_hello "exe/bin/hello"
	#gen 16_hello_deps "ext/hello/bin/hello"
}

test

exit $ret
