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

ret=0

if ! res="$(make build ARCH="$arch" CONFIG="$config" 2>&1)"; then
	echo "Failed to compile build"
	echo "$res"
	exit 1
fi

mk() {
	p_arch=$1
	p_config=$2
	proj=$3
	targets=$4

	dir="./examples/$proj"
	bin="$dir/bin"
	tmp="$dir/tmp"

	printf "%s %-7s %-12s %-5s " "$p_arch" "$p_config" "$proj" "Make"

	if ! out="$(./bin/"$arch"-"$config"/exes/build -p "$dir" -g M 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "build: Failed to generate make"
		echo "$out"
		ret=1
		return
	fi

	if ! out="$(make -C "$tmp/build" ARCH="$p_arch" CONFIG="$p_config" 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "make: Failed to build project"
		echo "$out"
		ret=1
		return
	fi

	if ! out="$(make -C "$tmp/build" cov ARCH="$p_arch" CONFIG="$p_config" OPEN=0 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "make: Failed to cov project"
		echo "$out"
		ret=1
		return
	fi

	for target in $targets; do
		if [ ! -f "$dir/$target" ]; then
			printf "\033[0;31mFAIL\033[0m\n"
			echo "Target not found: $target"
			ret=1
			return
		fi
	done

	printf "\033[0;32mPASS\033[0m\n"
	rm -rf "$bin" "$tmp"
}

cm() {
	p_arch=$1
	p_config=$2
	proj=$3
	targets=$4

	dir="./examples/$proj"
	bin="$dir/bin"
	tmp="$dir/tmp"
	build="$dir/build"

	printf "%s %-7s %-12s %-5s " "$p_arch" "$p_config" "$proj" "CMake"

	if ! out="$(./bin/"$arch"-"$config"/exes/build -p "$dir" -g C 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "build: Failed to generate cmake"
		echo "$out"
		ret=1
		return
	fi

	if ! out="$(cmake -S "$tmp/build" -B "$build" -G "Unix Makefiles" -DARCH="$p_arch" -DCMAKE_BUILD_TYPE="$p_config" -DOPEN=0 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "cmake: Failed to generate make"
		echo "$out"
		ret=1
		return
	fi

	if ! out="$(cmake --build "$build" 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "cmake: Failed to build project"
		echo "$out"
		ret=1
		return
	fi

	if ! out="$(cmake --build "$build" --target cov 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "cmake: Failed to cov project"
		echo "$out"
		ret=1
		return
	fi

	for target in $targets; do
		if [ ! -f "$dir/$target" ]; then
			printf "\033[0;31mFAIL\033[0m\n"
			echo "Target not found: $target"
			ret=1
			return
		fi
	done

	printf "\033[0;32mPASS\033[0m\n"
	rm -rf "$bin" "$build" "$tmp"
}

gen() {
	mk "$@"
	cm "$@"
}

test() {
	gen "$@" 00_exe "bin/$1-$2/bin/00_exe"
	gen "$@" 01_lib "bin/$1-$2/lib/01_lib.a"
	gen "$@" 02_multi "bin/$1-$2/bin/a bin/$1-$2/bin/b"
	gen "$@" 03_depends "bin/$1-$2/bin/exe bin/$1-$2/lib/lib.a"
	gen "$@" 04_rdepends "bin/$1-$2/lib/base.a bin/$1-$2/lib/lib1.a bin/$1-$2/lib/lib2.a bin/$1-$2/bin/exe"
	if [ "$2" = "Debug" ]; then
		gen "$@" 05_extern "bin/$1-$2/lib/cbase.a tmp/report/cov/index.html"
	else
		gen "$@" 05_extern "bin/$1-$2/lib/cbase.a"
	fi
	gen "$@" 06_lib_test "bin/$1-$2/lib/06_lib_test.a bin/$1-$2/test/06_lib_test"
	gen "$@" 07_zip "bin/$1-$2/ext/cbase/cbase.a"
}

test x64 Debug
test x64 Release
test x86 Debug
test x86 Release

exit $ret
