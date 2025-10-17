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

	if ! build_out="$(make -C "$tmp/build" ARCH="$p_arch" 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "make: Failed to build project"
		echo "$build_out"
		ret=1
		return
	fi

	if ! cov_out="$(make -C "$tmp/build" cov ARCH="$p_arch" OPEN=0 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "make: Failed to cov project"
		echo "$cov_out"
		ret=1
		return
	fi

	for c in $configs; do
		for target in $targets; do
			if [ ! -f "$dir/bin/$p_arch-$c/$target" ]; then
				printf "\033[0;31mFAIL\033[0m\n"
				echo "Target not found: $target"
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
	p_config=$2
	proj=$3
	targets=$4

	dir="./examples/$proj"
	bin="$dir/bin"
	tmp="$dir/tmp"
	build="$dir/build"

	rm -rf "$bin" "$build" "$tmp"

	printf "%s %-7s %-12s %-5s " "$p_arch" "$p_config" "$proj" "CMake"

	if ! out="$(./bin/"$arch"-"$config"/exes/build -p "$dir" -g C 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "build: Failed to generate cmake"
		echo "$out"
		ret=1
		return
	fi

	if ! gen_out="$(cmake -S "$tmp/build" -B "$build" -G "Unix Makefiles" -DARCH="$p_arch" -DOPEN=0 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "cmake: Failed to generate make"
		echo "$gen_out"
		ret=1
		return
	fi

	if ! build_out="$(cmake --build "$build" 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "cmake: Failed to build project"
		echo "$gen_out"
		echo "$build_out"
		ret=1
		return
	fi

	if ! cov_out="$(cmake --build "$build" --target cov 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "cmake: Failed to cov project"
		echo "$cov_out"
		ret=1
		return
	fi

	for target in $targets; do
		if [ ! -f "$dir/bin/$p_arch-$p_config/$target" ]; then
			printf "\033[0;31mFAIL\033[0m\n"
			echo "Target not found: $target"
			echo "$out"
			echo "$gen_out"
			echo "$build_out"
			echo "$cov_out"
			ret=1
			return
		fi
	done

	printf "\033[0;32mPASS\033[0m\n"
	#rm -rf "$bin" "$build" "$tmp"
}

gen() {
	#mk "$@"
	cm "$@"
}

test() {
	#gen "$@" 00_exe "bin/00_exe"
	#gen "$@" 01_lib "lib/01_lib.a"
	#gen "$@" 02_multi "bin/a bin/b"
	#gen "$@" 03_depends "bin/exe lib/lib.a"
	#gen "$@" 04_rdepends "lib/base.a lib/lib1.a lib/lib2.a bin/exe"
	if [ "$2" = "Debug" ]; then
		gen "$@" 05_extern "lib/cbase.a ../../tmp/report/cov/index.html"
	else
		gen "$@" 05_extern "lib/cbase.a"
	fi
	#gen "$@" 06_lib_test "lib/06_lib_test.a test/06_lib_test"
	#gen "$@" 07_zip "../../tmp/dl/cbase-main.zip ext/cbase/cbase.a"
}

test x64 Debug
#test x64 Release
#test x86 Debug
#test x86 Release

exit $ret
