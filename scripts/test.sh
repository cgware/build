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
	bin="./examples/$proj/bin"
	tmp="./examples/$proj/tmp"

	printf "%s %-7s %-12s " "$p_arch" "$p_config" "$proj"

	out="$(./bin/"$arch"-"$config"/exes/build -p "$dir" -g M 2>&1)"
	res=$?

	if [ $res -ne "0" ]; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "Failed to generate makefiles"
		echo "$out"
		ret=1
		return
	fi

	if ! out="$(make -C "$dir/tmp/build" ARCH="$p_arch" CONFIG="$p_config" 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "Failed to build project"
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

test() {
	mk "$1" "$2" 00_exe bin/00_exe
	mk "$1" "$2" 01_lib lib/01_lib.a
	mk "$1" "$2" 02_depends "bin/exe lib/lib.a"
	mk "$1" "$2" 03_extern bin/03_extern
}

test x64 Debug
test x64 Release
test x86 Debug
test x86 Release

exit $ret
