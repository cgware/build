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

configs="Debug Release"

ret=0

if ! res="$(make build ARCH="$arch" CONFIG="$config" 2>&1)"; then
	echo "Failed to compile build"
	echo "$res"
	exit 1
fi

mk() {
	p_arch=$1
	proj=$2
	targets=$3

	dir="./examples/$proj"
	bin="$dir/bin"
	tmp="$dir/tmp"

	printf "%s %-13s %-5s " "$p_arch" "$proj" "Make"

	rm -rf "$bin" "$tmp"

	if ! out="$(./bin/"$arch"-"$config"/exes/build -p "$dir" -g M -a "$p_arch" -c "$configs" -t "all cov" -O 0 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "build: Failed to generate make"
		echo "$out"
		ret=1
		return
	fi

	for c in $configs; do
		for t in $targets; do
			if [ ! -f "$dir/bin/$p_arch-$c/$t" ]; then
				printf "\033[0;31mFAIL\033[0m\n"
				echo "Target not found: $p_arch-$c/$t"
				echo "$build_out"
				echo "$cov_out"
				ret=1
				return
			fi
		done
	done

	printf "\033[0;32mPASS\033[0m\n"
	rm -rf "$bin" "$tmp"
}

cm() {
	p_arch=$1
	proj=$2
	targets=$3

	dir="./examples/$proj"
	bin="$dir/bin"
	tmp="$dir/tmp"
	build="$dir/build"

	printf "%s %-13s %-5s " "$p_arch" "$proj" "CMake"

	rm -rf "$bin" "$build" "$tmp"

	if ! out="$(./bin/"$arch"-"$config"/exes/build -p "$dir" -g C -a "$p_arch" -c "$config" -O 0 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "build: Failed to generate cmake"
		echo "$out"
		ret=1
		return
	fi

	for c in $configs; do
		for t in $targets; do
			if [ ! -f "$dir/bin/$p_arch-$c/$t" ]; then
				printf "\033[0;31mFAIL\033[0m\n"
				echo "Target not found: $p_arch-$c/$t"
				echo "$out"
				echo "$gen_out"
				echo "$build_out"
				echo "$cov_out"
				ret=1
				return
			fi
		done
	done

	printf "\033[0;32mPASS\033[0m\n"
	rm -rf "$bin" "$build" "$tmp"
}

gen() {
	#mk "$@"
	cm "$@"
}

test() {
	gen "$@" 00_exe "bin/00_exe"
	gen "$@" 01_lib "lib/01_lib.a"
	gen "$@" 02_multi "bin/a bin/b"
	gen "$@" 03_depends "bin/exe lib/lib.a"
	gen "$@" 04_rdepends "lib/base.a lib/lib1.a lib/lib2.a bin/exe"
	if [ "$2" = "Debug" ]; then
		gen "$@" 05_extern "lib/cbase.a ../../tmp/report/cov/index.html"
	else
		gen "$@" 05_extern "lib/cbase.a"
	fi
	gen "$@" 06_lib_test "lib/06_lib_test.a test/06_lib_test"
	gen "$@" 07_zip "../../tmp/dl/cbase-main.zip ext/cbase/cbase.a"
	gen "$@" 08_exe_driver "bin/08_exe_driver"
	gen "$@" 09_lib_driver "bin/exe test/lib"
}

test x64
#test x86

exit $ret
