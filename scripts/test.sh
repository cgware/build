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

	for target in $targets; do
		if [ ! -f "$bin/$p_arch-$p_config/$target" ]; then
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

	if ! out="$(cmake -S "$tmp/build" -B "$build" -G "Unix Makefiles" -DARCH="$p_arch" -DCMAKE_BUILD_TYPE="$p_config" 2>&1)"; then
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

	for target in $targets; do
		path="$bin/$p_arch-$p_config/$target"
		if [ ! -f "$path" ]; then
			printf "\033[0;31mFAIL\033[0m\n"
			echo "Target not found: $path"
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
	gen "$@" 00_exe bin/00_exe
	gen "$@" 01_lib lib/01_lib.a
	gen "$@" 02_multi "bin/a bin/b"
	gen "$@" 03_depends "bin/exe lib/lib.a"
	gen "$@" 04_extern bin/04_extern
}

test x64 Debug
test x64 Release
test x86 Debug
test x86 Release

exit $ret
